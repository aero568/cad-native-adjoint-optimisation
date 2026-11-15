#include "cad_native/bspline.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static int expect_span(const KNOTVECTOR *U, size_t p, double u, size_t expected_span) {
    size_t span = 0;

    if (!bspline_find_span(p, u, U, &span)) {
        fprintf(stderr, "expected success for u=%.17g\n", u);
        return 1;
    }

    if (span != expected_span) {
        fprintf(stderr, "for u=%.17g, expected span %zu but received %zu\n", u, expected_span, span);
        return 1;
    }

    return 0;
}

static int expect_failure(const KNOTVECTOR *U, size_t p, double u) {
    const size_t sentinel = 123;
    size_t span = sentinel;

    if (bspline_find_span(p, u, U, &span)) {
        fprintf(stderr, "expected failure for u=%.17g\n", u);
        return 1;
    }

    if (span != sentinel) {
        fprintf(stderr, "span_out changed during a failed call\n");
        return 1;
    }

    return 0;
}

static int test_degree_zero(void) {
    static const double knots[] = {0.0, 1.0};
    const KNOTVECTOR U = {
        .knots = knots,
        .m = 1,
    };
    int failures = 0;

    failures += expect_span(&U, 0, 0.0, 0);
    failures += expect_span(&U, 0, 0.5, 0);
    failures += expect_span(&U, 0, 1.0, 0);

    return failures;
}

static int test_clamped_quadratic(void) {
    static const double knots[] = {0.0, 0.0, 0.0, 1.0, 2.0, 3.0, 3.0, 3.0};
    const KNOTVECTOR U = {
        .knots = knots,
        .m = 7,
    };
    int failures = 0;

    failures += expect_span(&U, 2, 0.0, 2);
    failures += expect_span(&U, 2, 0.5, 2);
    failures += expect_span(&U, 2, 1.0, 3);
    failures += expect_span(&U, 2, 1.5, 3);
    failures += expect_span(&U, 2, 2.0, 4);
    failures += expect_span(&U, 2, 3.0, 4);

    return failures;
}

static int test_repeated_interior_knot(void) {
    static const double knots[] = {0.0, 0.0, 0.0, 1.0, 1.0, 2.0, 2.0, 2.0};
    const KNOTVECTOR U = {
        .knots = knots,
        .m = 7,
    };
    int failures = 0;

    failures += expect_span(&U, 2, 0.5, 2);
    failures += expect_span(&U, 2, 1.0, 4);
    failures += expect_span(&U, 2, 1.5, 4);

    return failures;
}

static int test_invalid_inputs(void) {
    static const double decreasing_knots[] = {0.0, 0.0, 0.0, 2.0, 1.0, 3.0, 3.0, 3.0};
    static const double nonfinite_knots[] = {0.0, 0.0, 0.0, 1.0, NAN, 3.0, 3.0, 3.0};
    static const double zero_width_knots[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    static const double knots[] = {0.0, 0.0, 0.0, 1.0, 2.0, 3.0, 3.0, 3.0};

    const KNOTVECTOR U = {
        .knots = knots,
        .m = 7,
    };

    const KNOTVECTOR null_knots = {
        .knots = NULL,
        .m = 7,
    };
    const KNOTVECTOR decreasing_U = {
        .knots = decreasing_knots,
        .m = 7,
    };
    const KNOTVECTOR nonfinite_U = {
        .knots = nonfinite_knots,
        .m = 7,
    };
    const KNOTVECTOR zero_width_U = {
        .knots = zero_width_knots,
        .m = 7,
    };
    int failures = 0;

    failures += expect_failure(NULL, 2, 0.5);
    failures += expect_failure(&null_knots, 2, 0.5);
    failures += expect_failure(&U, 8, 0.5);
    failures += expect_failure(&U, 4, 0.5);
    failures += expect_failure(&decreasing_U, 2, 0.5);
    failures += expect_failure(&nonfinite_U, 2, 0.5);
    failures += expect_failure(&U, 2, NAN);
    failures += expect_failure(&U, 2, INFINITY);
    failures += expect_failure(&U, 2, -0.1);
    failures += expect_failure(&U, 2, 3.1);
    failures += expect_failure(&zero_width_U, 2, 0.0);

    if (bspline_find_span(2, 0.5, &U, NULL)) {
        fprintf(stderr, "expected failure for null span_out\n");
        failures += 1;
    }

    return failures;
}

int main(void) {
    int failures = 0;

    failures += test_degree_zero();
    failures += test_clamped_quadratic();
    failures += test_repeated_interior_knot();
    failures += test_invalid_inputs();

    if (failures != 0) {
        fprintf(stderr, "%d find-span test(s) failed\n", failures);
        return EXIT_FAILURE;
    }

    printf("all find-span tests passed\n");
    return EXIT_SUCCESS;
}
