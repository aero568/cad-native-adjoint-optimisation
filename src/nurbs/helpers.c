#include "cad_native/bspline_helpers.h"
#include <math.h>

bool bspline_knot_vector_is_valid(const KNOTVECTOR *U) {
    size_t i;

    if (U == NULL || U->knots == NULL) {
        return false;
    }

    if (!isfinite(U->knots[0])) {
        return false;
    }

    for (i = 0; i < U->m; ++i) {
        if (!isfinite(U->knots[i + 1])) {
            return false;
        }

        if (U->knots[i] > U->knots[i + 1]) {
            return false;
        }
    }

    return true;
}

bool bspline_derive_n(size_t m, size_t p, size_t *n_out) {
    size_t n_plus_one;

    if (n_out == NULL) {
        return false;
    }

    if (p > m) {
        return false;
    }

    n_plus_one = m - p;

    if (n_plus_one <= p) {
        return false;
    }

    *n_out = n_plus_one - 1;
    return true;
}

bool bspline_parameter_is_in_active_domain(double u, double lower, double upper) {
    if (!isfinite(u) || !isfinite(lower) || !isfinite(upper)) {
        return false;
    }

    if (lower >= upper) {
        return false;
    }

    return u >= lower && u <= upper;
}
