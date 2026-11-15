#ifndef CAD_NATIVE_TYPES_H
#define CAD_NATIVE_TYPES_H

#include <stddef.h>

/* KNOTVECTOR is a non-owning, read-only view; the caller keeps the array alive.
 * m is the highest accessible knot index. The vector must be nonempty, and
 * knot values must be finite and nondecreasing; repeated knots are allowed.
 */
typedef struct {
    const double *knots;
    size_t m;
} KNOTVECTOR;


#endif
