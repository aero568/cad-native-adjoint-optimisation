#include "cad_native/bspline.h"
#include "cad_native/bspline_helpers.h"

bool bspline_find_span(size_t p, double u, const KNOTVECTOR *U,
                       size_t *span_out) {
    size_t n;
    size_t low;
    size_t high;
    size_t mid;
    double lower;
    double upper;

    // input validation
    if (span_out == NULL) {
        return false;
    }

    if (!bspline_knot_vector_is_valid(U)) {
        return false;
    }

    if (!bspline_derive_n(U->m, p, &n)) {
        return false;
    }

    lower = U->knots[p];
    upper = U->knots[n + 1];

    if (!bspline_parameter_is_in_active_domain(u, lower, upper)) {
        return false;
    }

    // binary search ( find span )
    if (u == upper) {
        *span_out = n;
        return true;
    }

    low = p;
    high = n + 1;

    while ((high - low) > 1) {
        mid = low + (high - low) / 2;
        if (u < U->knots[mid]) {
            high = mid;
        } else {
            low = mid;
        }
    }
    *span_out = low;
    return true;
}
