#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <unordered_set>
#include "common.pb.h"

namespace sentinel::nexus::fleet {

class AttestationValidator {
public:
    static AttestationValidator& instance() {
        static AttestationValidator inst;
        return inst;
    }

    // Cryptographically validates TPM 2.0 quote and machine uniqueness
    bool validate_hardware_integrity(const ::sentinel::nexus::HardwareIdentity& identity,
                                     const std::string& tpm_signature,
                                     std::string& out_reason);

    void add_authorized_machine_uuid(const std::string& uuid);
    void revoke_machine_uuid(const std::string& uuid);

private:
    AttestationValidator() = default;

    mutable std::mutex mutex_;
    std::unordered_set<std::string> revoked_uuids_;
    std::unordered_set<std::string> golden_uuids_;
};

} // namespace sentinel::nexus::fleet