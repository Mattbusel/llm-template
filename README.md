# llm-template

Mustache-style prompt templating in C++. Variable substitution, loops, conditionals, partials, token-aware truncation. One header.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)
![License: MIT](https://img.shields.io/badge/License-MIT-green)
![Single Header](https://img.shields.io/badge/library-single--header-orange)

---

## 30-second quickstart

```cpp
#define LLM_TEMPLATE_IMPLEMENTATION
#include "llm_template.hpp"

int main() {
    llm::Template tmpl(
        "You are a {{role}}. Answer: {{question}}\n"
        "{{#show_hint}}Hint: {{hint}}{{/show_hint}}"
    );

    llm::TemplateContext ctx;
    ctx.vars["role"]     = "helpful assistant";
    ctx.vars["question"] = "What is the boiling point of water?";
    ctx.vars["hint"]     = "Think about standard pressure.";
    ctx.flags["show_hint"] = true;

    std::cout << tmpl.render(ctx) << "\n";
}
```

---

## Installation

```bash
cp include/llm_template.hpp your-project/
```

No dependencies.

---

## Syntax

| Tag | Meaning |
|-----|---------|
| `{{variable}}` | Simple substitution |
| `{{#list}}...{{/list}}` | Loop over a list |
| `{{#flag}}...{{/flag}}` | Conditional block |
| `{{>partial_name}}` | Include another template |
| `{{!comment}}` | Ignored |

## API Reference

```cpp
// Render with simple vars
std::string out = tmpl.render({{"var", "value"}});

// Full render with lists + flags
llm::TemplateContext ctx;
ctx.vars["greeting"] = "Hello";
ctx.lists["items"] = { {{"name","Alice"}}, {{"name","Bob"}} };
ctx.flags["show_footer"] = true;
std::string out = tmpl.render(ctx);

// Truncate to token budget (estimates 4 chars/token)
std::string out = tmpl.render_truncated(ctx, 512);

// Introspect
auto vars    = tmpl.variables();
auto missing = tmpl.missing_vars(ctx);

// Registry for partials
llm::TemplateRegistry reg;
reg.add("header", "=== {{title}} ===\n");
reg.add("main", "{{>header}}Body: {{body}}");
auto out = reg.get("main").render({{"title","Hi"},{"body","World"}}, &reg);
```

---

## Building

```bash
cmake -B build && cmake --build build
./build/basic_template
./build/loop_template
./build/truncated
```

---

## See Also

| Repo | What it does |
|------|-------------|
| [llm-stream](https://github.com/Mattbusel/llm-stream) | Stream OpenAI & Anthropic responses token by token |
| [llm-cache](https://github.com/Mattbusel/llm-cache) | Cache responses, skip redundant calls |
| [llm-cost](https://github.com/Mattbusel/llm-cost) | Token counting + cost estimation |
| [llm-retry](https://github.com/Mattbusel/llm-retry) | Retry with backoff + circuit breaker |
| [llm-format](https://github.com/Mattbusel/llm-format) | Structured output enforcement |
| [llm-embed](https://github.com/Mattbusel/llm-embed) | Text embeddings + nearest-neighbor search |
| [llm-pool](https://github.com/Mattbusel/llm-pool) | Concurrent request pool + rate limiting |
| [llm-log](https://github.com/Mattbusel/llm-log) | Structured JSONL logger for LLM calls |
| [llm-template](https://github.com/Mattbusel/llm-template) | Prompt templating with loops + conditionals |
| [llm-agent](https://github.com/Mattbusel/llm-agent) | Tool-calling agent loop |
| [llm-rag](https://github.com/Mattbusel/llm-rag) | Full RAG pipeline |
| [llm-eval](https://github.com/Mattbusel/llm-eval) | Consistency and quality evaluation |

---

## License

MIT — see [LICENSE](LICENSE).
