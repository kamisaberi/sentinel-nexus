#pragma once
#include <string>

namespace sentinel::nexus::reporting {

class ScadaAuditEngine {
public:
    static ScadaAuditEngine& instance() {
        static ScadaAuditEngine inst;
        return inst;
    }

    std::string generate_iec62443_assessment_json() const;
    std::string generate_iec62443_assessment_text() const;

private:
    ScadaAuditEngine() = default;
};

} // namespace sentinel::nexus::reporting