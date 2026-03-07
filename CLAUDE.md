# CLAUDE.md — llm-template

## Build
```bash
cmake -B build && cmake --build build
```

## Key Constraint: SINGLE HEADER
`include/llm_template.hpp` is the entire library. Never split into multiple files.

## Implementation Guard
```cpp
#define LLM_TEMPLATE_IMPLEMENTATION
#include "llm_template.hpp"
```

## Common Mistakes
- Splitting the header
- Adding dependencies beyond libcurl
- Using exceptions in hot paths
- Forgetting RAII for resource handles
