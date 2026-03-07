# AGENTS.md — llm-template
## Purpose
Single-header C++ prompt templating: variable substitution, loops, conditionals, token-aware truncation.
## Architecture
Everything in `include/llm_template.hpp`. Guard: `#ifdef LLM_TEMPLATE_IMPLEMENTATION`. No external deps.
## Build
cmake -B build && cmake --build build
## Syntax
- {{var}} — variable substitution
- {{#list}}...{{/list}} — loop over list of maps
- {{#flag}}...{{/flag}} — conditional block
- {{!comment}} — ignored
## Constraints
Single header, no external deps, C++17, namespace llm.
