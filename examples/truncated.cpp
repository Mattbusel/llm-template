#define LLM_TEMPLATE_IMPLEMENTATION
#include "llm_template.hpp"
#include <iostream>

int main() {
    llm::Template t("Summarize: {{document}}\n\nAnswer in one sentence.");

    llm::TemplateContext ctx;
    // Large document
    ctx.vars["document"] = std::string(5000, 'x');

    size_t max_tokens = 100;
    std::string result = t.render_truncated(ctx, max_tokens);
    std::cout << "Truncated to ~" << max_tokens << " tokens ("
              << result.size() << " chars):\n";
    std::cout << result.substr(0, 120) << "...\n";
    return 0;
}
