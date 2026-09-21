#pragma once
#include <string>

namespace sentinel::nexus::reporting {

class ReportGenerator {
public:
    static ReportGenerator& instance() {
        static ReportGenerator inst;
        return inst;
    }

    // Compiles fleet metrics, drops, and microsecond SLA proofs into executive JSON/Text
    std::string generate_text_audit_report() const;
    std::string generate_json_compliance_report() const;

private:
    ReportGenerator() = default;
};

} // namespace sentinel::nexus::reporting