# AGENTS.md — llm-template

## Purpose
Single-header C++ library. Everything lives in `include/llm_template.hpp`.

## Build
```bash
cmake -B build && cmake --build build
```

## Rules
- Single header. Never split `include/llm_template.hpp`.
- No external deps (libcurl allowed only where needed for HTTP).
- All public API in namespace `llm`.
- C++17, zero warnings with -Wall -Wextra.
- Implementation guard: `#ifdef LLM_TEMPLATE_IMPLEMENTATION`
