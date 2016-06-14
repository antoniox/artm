#pragma once

#include "batch.h"
#include "phi.h"
#include "theta.h"
#include "types.h"


struct Perplexity {

    static float_type score_batch(
        const Batch & batch,
        const Phi & phi,
        const Theta & theta
    );

    static float_type score_batch(
        const Batch & batch,
        const Phi & phi,
        size_type inner_iterations
    );
};
