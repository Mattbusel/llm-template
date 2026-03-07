#define LLM_TEMPLATE_IMPLEMENTATION
#include "llm_template.hpp"
#include <iostream>

int main() {
    llm::Template t("Hello, {{name}}! You have {{count}} new messages.");

    llm::TemplateVars vars;
    vars["name"]  = "Alice";
    vars["count"] = "3";

    std::string result = t.render(vars);
    std::cout << result << "\n";

    // Show which variables the template uses
    auto v = t.variables();
    std::cout << "Variables: ";
    for (const auto& var : v) std::cout << var << " ";
    std::cout << "\n";

    // Show missing variables
    llm::TemplateContext ctx;
    ctx.vars["name"] = "Bob"; // count is missing
    auto missing = t.missing_vars(ctx);
    std::cout << "Missing: ";
    for (const auto& m : missing) std::cout << m << " ";
    std::cout << "\n";

    return 0;
}
