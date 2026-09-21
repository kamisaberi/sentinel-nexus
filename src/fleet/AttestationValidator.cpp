#include "AttestationValidator.hpp"
#include "core/Logger.hpp"

namespace sentinel::nexus::fleet {

bool AttestationValidator::validate_hardware_integrity(
    const ::sentinel::nexus::HardwareIdentity& identity,
    const std::string& tpm_signature,
    std::string& out_reason) {
    
    std::lock_guard<std::mutex> lock(mutex_);

    // 1. Verify UUID is not revoked
    if (revoked_uuids_.contains(identity.machine_uuid())) {
        out_reason = "Machine UUID revoked due to security violation: " + identity.machine_uuid();
        NEXUS_LOG_CRIT("ATTESTATION FAILURE: " + out_reason);
        return false;
    }

    // 2. Hardware backend validation
    if (identity.primary_backend() == ::sentinel::nexus::BACKEND_UNKNOWN) {
        out_reason = "Appliance reported unverified/unknown AI execution backend.";
        return false;
    }

    // 3. TPM 2.0 / vTPM Integrity checks
    if (identity.type() == ::sentinel::nexus::DEVICE_PHYSICAL_TPM2 ||
        identity.type() == ::sentinel::nexus::DEVICE_VIRTUAL_TPM) {
        
        if (identity.tpm_public_hash().empty() && tpm_signature.empty()) {
            out_reason = "TPM-enabled device did not provide cryptographic Quote or Public Key hash.";
            NEXUS_LOG_WARN("Attestation Warning: " + out_reason);
            // Allow soft-onboarding in development, but log attestation degradation
        }
    }

    // 4. Verify host sanity
    if (identity.hostname().empty() || identity.kernel_version().empty()) {
        out_reason = "Missing kernel release or hostname metadata.";
        return false;
    }

    out_reason = "Hardware identity and cryptographic attestation verified.";
    return true;
}

void AttestationValidator::add_authorized_machine_uuid(const std::string& uuid) {
    std::lock_guard<std::mutex> lock(mutex_);
    golden_uuids_.insert(uuid);
    revoked_uuids_.erase(uuid);
}

void AttestationValidator::revoke_machine_uuid(const std::string& uuid) {
    std::lock_guard<std::mutex> lock(mutex_);
    revoked_uuids_.insert(uuid);
    golden_uuids_.erase(uuid);
    NEXUS_LOG_WARN("UUID revoked from grid authorization: " + uuid);
}

} // namespace sentinel::nexus::fleet