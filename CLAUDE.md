# CLAUDE.md — llm-template
## Build & Run
cmake -B build && cmake --build build
./build/basic_template
## THE RULE: Single Header
include/llm_template.hpp is the entire library.
## API
- Template(str) — construct from string
- render(vars) → string — simple var substitution
- render(ctx) → string — full: vars + lists + flags
- render_truncated(ctx, max_tokens) → string — trims longest var until fits
- variables() → vector<string> — all {{var}} names
- missing_vars(ctx) → vector<string> — vars not in context
- TemplateRegistry: add, load_file, get, has
## Token estimation
chars/4 — same heuristic as llm-cost
