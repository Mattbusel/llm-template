#define LLM_TEMPLATE_IMPLEMENTATION
#include "llm_template.hpp"
#include <iostream>

int main() {
    // Loop template
    llm::Template t(
        "Items:\n"
        "{{#items}}"
        "  - {{name}}: {{price}}\n"
        "{{/items}}"
        "{{#premium}}"
        "  [Premium Account]\n"
        "{{/premium}}"
    );

    llm::TemplateContext ctx;
    ctx.lists["items"] = {
        {{"name", "Apple"},  {"price", "$1.00"}},
        {{"name", "Banana"}, {"price", "$0.50"}},
        {{"name", "Cherry"}, {"price", "$3.00"}},
    };
    ctx.flags["premium"] = true;

    std::cout << t.render(ctx);
    return 0;
}
