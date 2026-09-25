# llm-template

[![CI](https://github.com/Mattbusel/llm-template/actions/workflows/ci.yml/badge.svg)](https://github.com/Mattbusel/llm-template/actions/workflows/ci.yml)
![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

**Mustache-style prompt templates for C++, with token-budget truncation.** One header, `llm_template.hpp`. Needs nothing beyond the C++17 standard library.

Prompts built by string concatenation get messy fast. llm-template keeps them as readable templates with variables, loops and conditionals, and can shrink the rendered prompt to fit a token budget by trimming the longest variable first, which is usually the pasted document rather than your instructions.

## Features

- `{{var}}` substitution, `{{#list}}...{{/list}}` loops, `{{#flag}}...{{/flag}}` conditionals, `{{!comment}}`
- Render from a simple map or a `TemplateContext` with vars, lists and flags
- `render_truncated(ctx, max_tokens)`: estimates tokens as characters / 4 and trims the longest variable until it fits
- `variables()` and `missing_vars()` to inspect a template
- `TemplateRegistry` to keep named templates in memory or load them from files
- Standard library only

## Quick start

Copy the header into your project:

```bash
curl -fsSLO https://raw.githubusercontent.com/Mattbusel/llm-template/main/include/llm_template.hpp
```

Define `LLM_TEMPLATE_IMPLEMENTATION` in exactly one `.cpp` file before including it; every other file just includes the header. Save this as `main.cpp` next to the header:

```cpp
#define LLM_TEMPLATE_IMPLEMENTATION
#include "llm_template.hpp"
#include <iostream>

int main() {
    llm::Template t(
        "{{!system prompt for a code reviewer}}"
        "You review {{language}} code.\n"
        "{{#strict}}Be strict about style.\n{{/strict}}"
        "Files:\n{{#files}}- {{path}} ({{lines}} lines)\n{{/files}}");

    llm::TemplateContext ctx;
    ctx.vars["language"] = "C++";
    ctx.flags["strict"]  = true;
    ctx.lists["files"]   = {{{"path", "main.cpp"}, {"lines", "120"}},
                            {{"path", "util.hpp"}, {"lines", "45"}}};

    std::cout << t.render(ctx);

    // Fit a token budget (estimated as chars / 4); trims the longest variable first.
    std::cout << t.render_truncated(ctx, 30) << "\n";

    for (const auto& v : t.variables()) std::cout << "uses: " << v << "\n";
}
```

```bash
g++ -std=c++17 -O2 main.cpp -o demo
```

## API at a glance

| Call | Purpose |
|---|---|
| `Template(text)` | Parse a template |
| `render(vars)` / `render(ctx)` | Render with a map or a full context |
| `render_truncated(ctx, max_tokens)` | Render within an approximate token budget |
| `variables()` / `missing_vars(ctx)` | Inspect placeholders |
| `TemplateRegistry` | `add`, `load_file`, `get`, `has` |

## Notes and limitations

- The token estimate is a rough characters / 4 rule. For accurate counts and prices use [llm-cost](https://github.com/Mattbusel/llm-cost).
- `missing_vars()` checks top-level variables only; names used inside `{{#list}}` blocks are reported as missing even when the list provides them.

## Build the examples

The repo builds `examples/basic_template.cpp`, `examples/loop_template.cpp`, `examples/truncated.cpp`, `examples/registry.cpp` with CMake:

```bash
cmake -B build
cmake --build build
```

## Part of llm-cpp

llm-template is one of 26 single-header C++ libraries in [llm-cpp](https://github.com/Mattbusel/llm-cpp), a toolkit for building LLM features into native code. Each library stands alone; combine them by giving each `*_IMPLEMENTATION` define its own `.cpp` file. See the [llm-cpp README](https://github.com/Mattbusel/llm-cpp#using-several-together) for the full list and examples of using several together.

## License

MIT. See [LICENSE](LICENSE).
