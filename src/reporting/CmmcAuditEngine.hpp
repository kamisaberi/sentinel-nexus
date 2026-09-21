#pragma once
#include <string>
#include <vector>

namespace sentinel::nexus::reporting {

struct ComplianceFinding {
    std::string control_id;
    std::string title;
    bool passed;
    std::string evidence;
};

class CmmcAuditEngine {
public:
    static CmmcAuditEngine& instance() {
        static CmmcAuditEngine inst;
        return inst;
    }

    // Evaluates the live fleet against CMMC 2.0 Level 2 / NIST SP 800-171 controls
    std::string generate_cmmc_assessment_json() const;
    std::string generate_cmmc_assessment_text() const;

private:
    CmmcAuditEngine() = default;
    std::vector<ComplianceFinding> evaluate_controls() const;
};

} // namespace sentinel::nexus::reporting