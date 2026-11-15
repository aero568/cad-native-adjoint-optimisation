#ifndef CAD_NATIVE_BSPLINE_HELPERS_H
#define CAD_NATIVE_BSPLINE_HELPERS_H

#include "cad_native/types.h"
#include <stdbool.h>

bool bspline_knot_vector_is_valid(const KNOTVECTOR *U);
bool bspline_derive_n(size_t m, size_t p, size_t *n_out);
bool bspline_parameter_is_in_active_domain(double u, double lower, double upper);

#endif
