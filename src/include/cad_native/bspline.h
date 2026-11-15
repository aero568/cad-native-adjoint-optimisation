#ifndef CAD_NATIVE_BSPLINE_H
#define CAD_NATIVE_BSPLINE_H

#include <stdbool.h>
#include <stddef.h>
#include "cad_native/types.h"

/*
 * Finds the span for parameter u using borrowed knot vector U and degree p.
 * Requires nonnull U, U->knots, and span_out; p <= U->m; and
 * n = U->m - p - 1 >= p. All knots and u must be finite, and knots must
 * be nondecreasing. The active domain [U->knots[p], U->knots[n + 1]]
 * must have positive width, and u must lie within it.
 *
 * Normally, U->knots[span] <= u < U->knots[span + 1]. At the right
 * endpoint, u == U->knots[n + 1], span is n. Repeated interior knots use
 * the right-hand nonzero span. Returns true on success; on failure returns
 * false and leaves *span_out unchanged.
 */
bool bspline_find_span(size_t p,
                       double u,
                       const KNOTVECTOR *U,
                       size_t *span_out);

/*
 * Computes the p + 1 nonrational B-spline basis functions at u. span must
 * follow the project's knot-span convention: normally
 * U->knots[span] <= u < U->knots[span + 1], with the right endpoint mapped
 * to n and repeated interior knots using the right-hand nonzero span.
 *
 * Requires nonnull U, U->knots, and basis_out; basis_capacity >= p + 1;
 * finite, nondecreasing knots and finite u; and consistent dimensions, with
 * n = U->m - p - 1, n >= p, and p <= span <= n. u must be in the active
 * domain [U->knots[p], U->knots[n + 1]], and span must be consistent with u.
 *
 * Results are ordered from N[span - p, p](u) through N[span, p](u).
 * A zero denominator from repeated knots contributes zero. Returns false
 * for invalid input or workspace-allocation failure; on failure,
 * basis_out is unchanged.
 */
bool bspline_basis_functions(size_t p,
                             double u,
                             size_t span,
                             const KNOTVECTOR *U,
                             double *basis_out,
                             size_t basis_capacity);
#endif
