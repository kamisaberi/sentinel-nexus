#pragma once
#include <vector>
#include "telemetry.pb.h"

namespace sentinel::nexus::telemetry {

class UncertaintySampler {
public:
    static UncertaintySampler& instance() {
        static UncertaintySampler inst;
        return inst;
    }

    void set_active_learning_window(float min_uncertainty, float max_uncertainty, float min_recon_loss);

    // Returns true if candidate vector possesses high active-learning utility
    bool should_sample(const ::sentinel::nexus::CandidateVector& vector) const;

private:
    UncertaintySampler() = default;

    float min_uncertainty_{0.40f};
    float max_uncertainty_{0.60f};
    float min_recon_loss_{0.75f};
};

} // namespace sentinel::nexus::telemetry