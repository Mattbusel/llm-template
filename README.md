# llm-template

Prompt templating for C++. Variable substitution, loops, conditionals, token-aware truncation. Single header.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![License MIT](https://img.shields.io/badge/license-MIT-green.svg)
![Single Header](https://img.shields.io/badge/single-header-orange.svg)

## Quickstart

```cpp
#define LLM_TEMPLATE_IMPLEMENTATION
#include "llm_template.hpp"

llm::Template tmpl("Hello {{name}}, you are a {{role}} assistant.");

llm::TemplateVars vars;
vars["name"] = "Claude";
vars["role"] = "helpful";

std::cout << tmpl.render(vars) << "\n";
// Hello Claude, you are a helpful assistant.
```

## Installation

Copy `include/llm_template.hpp`. No external dependencies.

## Syntax

| Tag | Meaning |
|-----|---------|
| `{{variable}}` | String substitution |
| `{{#list}}...{{/list}}` | Loop over list items; use `{{field}}` inside |
| `{{#flag}}...{{/flag}}` | Render block only if flag is true |
| `{{!comment}}` | Ignored |

## API

```cpp
// Simple render
llm::Template tmpl("Hello {{name}}!");
std::cout << tmpl.render({{"name", "Alice"}}) << "\n";

// Full context: vars + lists + flags
llm::TemplateContext ctx;
ctx.vars["topic"]   = "recursion";
ctx.flags["verbose"] = true;
ctx.lists["examples"] = {
    {{"code", "fib(n)"},  {"lang", "C++"}},
    {{"code", "fact(n)"}, {"lang", "Python"}},
};
std::string prompt = tmpl.render(ctx);

// Truncate to token budget
std::string prompt = tmpl.render_truncated(ctx, /*max_tokens=*/512);

// Introspection
auto vars    = tmpl.variables();        // all {{var}} names
auto missing = tmpl.missing_vars(ctx);  // vars not in context

// Registry
llm::TemplateRegistry reg;
reg.add("system", "You are {{persona}}.");
reg.load_file("rag", "prompts/rag.txt");
std::cout << reg.get("system").render({{"persona","expert"}}) << "\n";
```

## Examples

- [`examples/basic_template.cpp`](examples/basic_template.cpp) — variable substitution, missing var detection
- [`examples/loop_template.cpp`](examples/loop_template.cpp) — render a list of items
- [`examples/truncated.cpp`](examples/truncated.cpp) — truncate a long context to token budget
- [`examples/registry.cpp`](examples/registry.cpp) — load and use multiple named templates

## Building

```bash
cmake -B build && cmake --build build
./build/basic_template
./build/loop_template
./build/truncated
./build/registry
```

## Requirements

C++17. No external dependencies.

## See Also

| Repo | What it does |
|------|-------------|
| [llm-stream](https://github.com/Mattbusel/llm-stream) | Stream OpenAI and Anthropic responses via SSE |
| [llm-cache](https://github.com/Mattbusel/llm-cache) | LRU response cache |
| [llm-cost](https://github.com/Mattbusel/llm-cost) | Token counting and cost estimation |
| [llm-retry](https://github.com/Mattbusel/llm-retry) | Retry and circuit breaker |
| [llm-format](https://github.com/Mattbusel/llm-format) | Structured output / JSON schema |
| [llm-embed](https://github.com/Mattbusel/llm-embed) | Embeddings and vector search |
| [llm-pool](https://github.com/Mattbusel/llm-pool) | Concurrent request pool |
| [llm-log](https://github.com/Mattbusel/llm-log) | Structured JSONL logging |
| [llm-template](https://github.com/Mattbusel/llm-template) | Prompt templating |
| [llm-agent](https://github.com/Mattbusel/llm-agent) | Tool-calling agent loop |
| [llm-rag](https://github.com/Mattbusel/llm-rag) | RAG pipeline |
| [llm-eval](https://github.com/Mattbusel/llm-eval) | Evaluation and consistency scoring |
| [llm-chat](https://github.com/Mattbusel/llm-chat) | Conversation memory manager |
| [llm-vision](https://github.com/Mattbusel/llm-vision) | Multimodal image+text |
| [llm-mock](https://github.com/Mattbusel/llm-mock) | Mock LLM for testing |
| [llm-router](https://github.com/Mattbusel/llm-router) | Model routing by complexity |
| [llm-guard](https://github.com/Mattbusel/llm-guard) | PII detection and injection guard |
| [llm-compress](https://github.com/Mattbusel/llm-compress) | Context compression |
| [llm-batch](https://github.com/Mattbusel/llm-batch) | Batch processing and checkpointing |

## License

MIT — see [LICENSE](LICENSE).
