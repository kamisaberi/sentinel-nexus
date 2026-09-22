#include "UncertaintySampler.hpp"

namespace sentinel::nexus::telemetry {

void UncertaintySampler::set_active_learning_window(float min_unc, float max_unc, float min_loss) {
    min_uncertainty_ = min_unc;
    max_uncertainty_ = max_unc;
    min_recon_loss_ = min_loss;
}

bool UncertaintySampler::should_sample(const ::sentinel::nexus::CandidateVector& vector) const {
    // 1. Mandatory inclusion: caused a hardware kernel drop
    if (vector.triggered_kernel_drop()) {
        return true;
    }

    // 2. High Model Uncertainty Window
    float unc = vector.inference_uncertainty();
    if (unc >= min_uncertainty_ && unc <= max_uncertainty_) {
        return true;
    }

    // 3. Autoencoder Outlier/Novelty Loss
    if (vector.autoencoder_recon_loss() >= min_recon_loss_) {
        return true;
    }

    return false;
}

} // namespace sentinel::nexus::telemetry