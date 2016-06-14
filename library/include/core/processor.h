#pragma once

#include "batch.h"
#include "expectation.h"
#include "phi.h"
#include "theta.h"
#include "types.h"


struct BatchProcessor {
    static void process(
        const Batch & batch,
        Phi & phi,
        Phi & phi_counters,
        size_type inner_iterations
    );

    static void calculate_expectation(
        Expectation & expectation,
        const Batch & batch,
        const Phi & phi,
        const Theta & theta
    );

    static void maximize_theta(
        Theta & theta,
        const Batch & batch,
        const Expectation & expectation
    );

    static void maximize_phi(
        Phi & phi,
        Phi & phi_counters,
        const Batch & batch,
        const Expectation & expectation
    );
};
