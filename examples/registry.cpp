#define LLM_TEMPLATE_IMPLEMENTATION
#include "llm_template.hpp"
#include <iostream>

int main() {
    llm::TemplateRegistry reg;

    // Register a partial — reusable header
    reg.add("header", "=== {{title}} ===\nDate: {{date}}\n\n");

    // Register a template that uses the partial
    reg.add("report",
        "{{>header}}"
        "Summary: {{summary}}\n\n"
        "{{#show_details}}"
        "Details:\n{{details}}\n"
        "{{/show_details}}"
        "End of report."
    );

    llm::TemplateContext ctx;
    ctx.vars["title"]   = "Monthly Analysis";
    ctx.vars["date"]    = "2026-03-06";
    ctx.vars["summary"] = "Overall performance improved 12% month-over-month.";
    ctx.vars["details"] = "Region A: +18%\nRegion B: +5%\nRegion C: -2%";
    ctx.flags["show_details"] = true;

    // render() with registry so {{>header}} partial resolves
    std::string output = reg.get("report").render(ctx, &reg);
    std::cout << output << "\n";

    ctx.flags["show_details"] = false;
    std::cout << "--- Without details ---\n";
    std::cout << reg.get("report").render(ctx, &reg) << "\n";

    return 0;
}
