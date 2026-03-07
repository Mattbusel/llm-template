#pragma once

// llm_template.hpp -- Zero-dependency single-header C++ prompt templating.
// Mustache-style: {{var}}, {{#list}}...{{/list}}, {{#flag}}...{{/flag}}, {{>partial}}, {{!comment}}
//
// USAGE:
//   #define LLM_TEMPLATE_IMPLEMENTATION  (in exactly one .cpp)
//   #include "llm_template.hpp"

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace llm {

using TemplateVars = std::map<std::string, std::string>;

struct TemplateContext {
    std::map<std::string, std::string>              vars;
    std::map<std::string, std::vector<TemplateVars>> lists;
    std::map<std::string, bool>                      flags;
};

class TemplateRegistry; // forward

class Template {
public:
    explicit Template(const std::string& tmpl);

    std::string render(const TemplateVars& vars) const;
    std::string render(const TemplateContext& ctx,
                       const TemplateRegistry* reg = nullptr) const;

    /// Render and truncate to token budget (estimates 4 chars/token).
    /// Truncates the longest substituted variable first.
    std::string render_truncated(const TemplateContext& ctx,
                                  size_t max_tokens,
                                  const TemplateRegistry* reg = nullptr) const;

    std::vector<std::string> variables() const;
    std::vector<std::string> missing_vars(const TemplateContext& ctx) const;

private:
    struct Node {
        enum class Type { Text, Variable, Loop, Conditional, Partial, Comment };
        Type               type;
        std::string        value;       // text content / var name / list name / partial name
        std::vector<Node>  children;    // loop and conditional body
    };

    std::vector<Node> m_nodes;

    static std::vector<Node> parse(const std::string& tmpl);
    static std::string render_nodes(const std::vector<Node>& nodes,
                                     const TemplateContext& ctx,
                                     const TemplateRegistry* reg);
    static void collect_vars(const std::vector<Node>& nodes,
                              std::vector<std::string>& out);
};

class TemplateRegistry {
public:
    void add(const std::string& name, const std::string& tmpl);
    void load_file(const std::string& name, const std::string& filepath);
    const Template& get(const std::string& name) const;
    bool has(const std::string& name) const;

private:
    std::map<std::string, Template> m_templates;
};

} // namespace llm

// ---------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------

#ifdef LLM_TEMPLATE_IMPLEMENTATION

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace llm {

// ---------------------------------------------------------------------------
// Parser — tokenize and build node tree
// ---------------------------------------------------------------------------

std::vector<Template::Node> Template::parse(const std::string& tmpl) {
    std::vector<Node> nodes;
    size_t pos = 0;

    // Helper: collect nodes until we hit {{/tag}}
    std::function<std::vector<Node>(size_t&, const std::string&)> parse_until =
        [&](size_t& p, const std::string& /*end_tag*/) -> std::vector<Node> {
        std::vector<Node> result;
        while (p < tmpl.size()) {
            auto tag_start = tmpl.find("{{", p);
            if (tag_start == std::string::npos) {
                Node n; n.type = Node::Type::Text; n.value = tmpl.substr(p);
                result.push_back(n); p = tmpl.size(); break;
            }
            if (tag_start > p) {
                Node n; n.type = Node::Type::Text; n.value = tmpl.substr(p, tag_start - p);
                result.push_back(n);
            }
            auto tag_end = tmpl.find("}}", tag_start + 2);
            if (tag_end == std::string::npos) break;
            std::string tag = tmpl.substr(tag_start + 2, tag_end - tag_start - 2);
            p = tag_end + 2;

            // Trim whitespace from tag
            auto ts = tag.find_first_not_of(" \t");
            auto te = tag.find_last_not_of(" \t");
            if (ts != std::string::npos) tag = tag.substr(ts, te - ts + 1);

            if (tag.empty()) continue;

            if (tag[0] == '!') {
                // Comment — skip
                Node n; n.type = Node::Type::Comment; result.push_back(n);
            } else if (tag[0] == '#') {
                std::string name = tag.substr(1);
                auto trim_n = name.find_first_not_of(" \t");
                if (trim_n != std::string::npos) name = name.substr(trim_n);
                // Could be loop (list) or conditional (flag) — determine at render time
                Node n;
                n.type     = Node::Type::Loop; // treat same structurally
                n.value    = name;
                n.children = parse_until(p, "/" + name);
                result.push_back(n);
            } else if (tag[0] == '/') {
                // End tag — stop
                break;
            } else if (tag[0] == '>') {
                std::string name = tag.substr(1);
                auto trim_n = name.find_first_not_of(" \t");
                if (trim_n != std::string::npos) name = name.substr(trim_n);
                Node n; n.type = Node::Type::Partial; n.value = name;
                result.push_back(n);
            } else {
                Node n; n.type = Node::Type::Variable; n.value = tag;
                result.push_back(n);
            }
        }
        return result;
    };

    pos = 0;
    nodes = parse_until(pos, "");
    return nodes;
}

std::string Template::render_nodes(const std::vector<Node>& nodes,
                                    const TemplateContext& ctx,
                                    const TemplateRegistry* reg) {
    std::string out;
    for (const auto& node : nodes) {
        switch (node.type) {
            case Node::Type::Text:
                out += node.value;
                break;
            case Node::Type::Variable: {
                auto it = ctx.vars.find(node.value);
                out += (it != ctx.vars.end()) ? it->second : "";
                break;
            }
            case Node::Type::Loop: {
                // Check if it's a flag (conditional)
                auto fit = ctx.flags.find(node.value);
                if (fit != ctx.flags.end()) {
                    if (fit->second) out += render_nodes(node.children, ctx, reg);
                } else {
                    // It's a list loop
                    auto lit = ctx.lists.find(node.value);
                    if (lit != ctx.lists.end()) {
                        for (const auto& row_vars : lit->second) {
                            TemplateContext inner = ctx;
                            for (const auto& [k, v] : row_vars)
                                inner.vars[k] = v;
                            out += render_nodes(node.children, inner, reg);
                        }
                    }
                }
                break;
            }
            case Node::Type::Partial:
                if (reg && reg->has(node.value)) {
                    out += reg->get(node.value).render(ctx, reg);
                }
                break;
            case Node::Type::Comment:
                break;
            case Node::Type::Conditional:
                break;
        }
    }
    return out;
}

void Template::collect_vars(const std::vector<Node>& nodes,
                              std::vector<std::string>& out) {
    for (const auto& node : nodes) {
        if (node.type == Node::Type::Variable) out.push_back(node.value);
        if (!node.children.empty()) collect_vars(node.children, out);
    }
}

Template::Template(const std::string& tmpl) : m_nodes(parse(tmpl)) {}

std::string Template::render(const TemplateVars& vars) const {
    TemplateContext ctx;
    ctx.vars = vars;
    return render_nodes(m_nodes, ctx, nullptr);
}

std::string Template::render(const TemplateContext& ctx,
                              const TemplateRegistry* reg) const {
    return render_nodes(m_nodes, ctx, reg);
}

std::string Template::render_truncated(const TemplateContext& ctx,
                                        size_t max_tokens,
                                        const TemplateRegistry* reg) const {
    // First try full render
    std::string full = render(ctx, reg);
    // Estimate tokens: 1 token ≈ 4 chars
    if (full.size() / 4 <= max_tokens) return full;

    // Find longest variable value and progressively truncate it
    TemplateContext trunc_ctx = ctx;
    std::string longest_key;
    size_t longest_len = 0;
    for (const auto& [k, v] : ctx.vars) {
        if (v.size() > longest_len) { longest_len = v.size(); longest_key = k; }
    }

    if (longest_key.empty()) return full.substr(0, max_tokens * 4);

    // Binary search for the right truncation length
    size_t lo = 0, hi = longest_len;
    while (lo < hi) {
        size_t mid = (lo + hi + 1) / 2;
        trunc_ctx.vars[longest_key] = ctx.vars.at(longest_key).substr(0, mid) + "...[truncated]";
        std::string candidate = render(trunc_ctx, reg);
        if (candidate.size() / 4 <= max_tokens) lo = mid;
        else hi = mid - 1;
    }
    trunc_ctx.vars[longest_key] = ctx.vars.at(longest_key).substr(0, lo) + "...[truncated]";
    return render(trunc_ctx, reg);
}

std::vector<std::string> Template::variables() const {
    std::vector<std::string> out;
    collect_vars(m_nodes, out);
    // deduplicate
    std::sort(out.begin(), out.end());
    out.erase(std::unique(out.begin(), out.end()), out.end());
    return out;
}

std::vector<std::string> Template::missing_vars(const TemplateContext& ctx) const {
    auto vars = variables();
    std::vector<std::string> missing;
    for (const auto& v : vars) {
        if (!ctx.vars.count(v)) missing.push_back(v);
    }
    return missing;
}

// ---------------------------------------------------------------------------
// TemplateRegistry
// ---------------------------------------------------------------------------

void TemplateRegistry::add(const std::string& name, const std::string& tmpl) {
    m_templates.emplace(name, Template(tmpl));
}

void TemplateRegistry::load_file(const std::string& name, const std::string& filepath) {
    std::ifstream f(filepath);
    if (!f) return;
    std::string content((std::istreambuf_iterator<char>(f)),
                         std::istreambuf_iterator<char>());
    add(name, content);
}

const Template& TemplateRegistry::get(const std::string& name) const {
    return m_templates.at(name);
}

bool TemplateRegistry::has(const std::string& name) const {
    return m_templates.count(name) > 0;
}

} // namespace llm

#endif // LLM_TEMPLATE_IMPLEMENTATION
