# CAD-Native Aerodynamic Design Optimisation Framework

**A research-oriented, open-source framework for differentiable NURBS geometry and gradient-based aerodynamic shape optimisation.**

> **Project status:** Early development / planned prototype. The architecture, milestones, and interfaces below describe the intended implementation; they are not claims of completed functionality.

## Overview

Gradient-based aerodynamic shape optimisation commonly uses free-form deformation (FFD) or related parameterisations to move CFD surface and volume meshes. Although effective, these workflows can leave a separate geometry-reconstruction task when the optimised shape must be transferred into a native, reusable CAD model.

This project explores an alternative: **make the NURBS geometry itself the source of the design variables**, differentiate the map from those variables to discrete aerodynamic surface coordinates, and propagate aerodynamic sensitivities back to the original CAD parameters.

The goal is not to eliminate the need for volume-mesh movement. Instead, it is to **avoid relying on FFD as the design parameterisation**, while preserving a regenerable, parameterised geometric definition throughout optimisation.

The initial demonstrator is a **2D airfoil embedded in a 3D geometry engine** (`z = 0` initially). The longer-term target is a 3D wing design and multidisciplinary optimisation workflow.

## Research questions

1. Can analytical NURBS derivatives provide accurate and efficient surface-coordinate sensitivities with respect to native design parameters?
2. How do analytical derivatives compare with forward- and reverse-mode algorithmic differentiation (AD)?
3. Can geometry Jacobian-vector products (JVPs) and vector-Jacobian products (VJPs) be coupled to an aerodynamic discrete adjoint without an FFD parameterisation?
4. Can an optimised aerodynamic shape be regenerated directly from its final NURBS parameters?
5. What additional interfaces and mesh-motion sensitivities are required to extend the method to a high-fidelity CFD adjoint?

## Conceptual architecture

```text
             CAD / NURBS design variables (p)
                          |
                          v
          C11 NURBS geometry and sensitivities
              X_surface(p), JVP, VJP
                          |
                          v
          Surface discretisation / mesh motion
             (volume mesh for CFD cases)
                          |
                          v
             Aerodynamic residual R(q, X)=0
                          |
                          v
          Direct / discrete-adjoint derivatives
                          |
                          v
               Gradient-based optimiser
                          |
                          +------> Updated NURBS parameters
```

The first implementation will use a lightweight 2D panel method to verify the complete sensitivity chain. OpenMDAO integration follows; MPhys and DAFoam/OpenFOAM are subsequent integration targets, not initial dependencies.

## Mathematical foundation

For a fixed-degree, fixed-knot-vector NURBS curve,

\[
\mathbf C(u;\mathbf p) =
\frac{\sum_{i=0}^{n} N_{i,k}(u) w_i\mathbf P_i}
     {\sum_{i=0}^{n} N_{i,k}(u) w_i}
= \sum_{i=0}^{n} R_i(u)\mathbf P_i.
\]

Here, `P_i` are 3D Euclidean control points, `w_i` are weights, `N_{i,k}` are B-spline basis functions, and `p` denotes the active design variables. For fixed knots, degree, weights, and evaluation parameter,

\[
\frac{\partial \mathbf C}{\partial P_{i,x}}=R_i(u)\mathbf e_x,
\qquad
\frac{\partial \mathbf C}{\partial w_i}=
\frac{N_{i,k}(u)}{\sum_j N_{j,k}(u)w_j}
\left(\mathbf P_i-\mathbf C(u)\right).
\]

The geometry module will support both `J_g v` (forward product) and `J_g^T w` (reverse product), where `J_g = dX_surface/dp`. This permits matrix-free sensitivity propagation without requiring a dense geometry Jacobian.

For an aerodynamic objective `J(q, X)` and discrete residual `R(q, X) = 0`, define the adjoint using

\[
\left(\frac{\partial R}{\partial q}\right)^T\lambda
=-\left(\frac{\partial J}{\partial q}\right)^T.
\]

The total gradient with respect to CAD parameters follows by differentiating the flow, mesh-motion, and geometry maps. In high-fidelity CFD, **surface-to-volume mesh sensitivity must still be accounted for**; removing FFD does not remove this mathematical dependency.

## Design principles

- **CAD-native:** final shapes remain defined by NURBS parameters, not only by deformed meshes.
- **3D from the outset:** all geometric points have `x`, `y`, and `z` coordinates, even for 2D airfoil tests.
- **C11 core:** numerical geometry and sensitivity algorithms are written in portable C, with explicit ownership and error handling.
- **Solver-independent geometry:** the NURBS library must not depend on OpenMDAO or any CFD package.
- **Derivative-first verification:** compare analytical derivatives, AD, direct sensitivities, and numerical checks before optimisation.
- **Incremental, reproducible development:** small milestones, unit tests, documented assumptions, and version-controlled examples.
- **Learning-oriented implementation:** the project develops C programming, numerical methods, AD, and adjoint understanding rather than treating external tools as black boxes.

## Planned C data model

The initial C structures are inspired by the geometry data structures discussed in Chapter 13.3 of *The NURBS Book*:

| Structure | Intended responsibility |
| --- | --- |
| `POINT` | 3D Euclidean coordinate (`x`, `y`, `z`) |
| `CPOINT` | 3D control-point position plus rational weight |
| `CPOLYGON` | Ordered control points and associated ownership/count information |
| `KNOTVECTOR` | Knot values, size, and associated curve metadata as appropriate |
| `CURVE` | Degree, control polygon, knot vector, and curve-level metadata |

These names express the initial design intent; precise field layouts, memory ownership, const-correctness, and API signatures will be agreed and documented during implementation. Homogeneous coordinates may be used internally without changing the public Euclidean control-point representation.

The initial curve topology, degree, and knot vector will remain fixed during optimisation. Knot insertion, trimming, topology changes, and differentiated reparameterisation are outside the first release.

## Technology stack

| Technology | Planned role |
| --- | --- |
| C11 + CMake | NURBS evaluation, analytical sensitivities, and small AD experiments |
| Python + NumPy/SciPy | Bindings, numerical experiments, panel solver, optimisation |
| pytest / CTest | Automated tests and regression checks |
| OpenMDAO | Optimisation workflow and total-derivative orchestration |
| MPhys | Later multidisciplinary coupling |
| DAFoam / OpenFOAM | Later high-fidelity CFD and discrete-adjoint integration |
| OpenCASCADE | Optional CAD exchange and geometry validation |

**Note:** a working OpenMDAO/panel-method prototype is the baseline goal. DAFoam/MPhys integration is a stretch objective and may require version-specific adapters.

## 12-week development roadmap

The project is planned around **8–10 hours per week**, approximately **96–120 hours total**.

| Week | Focus | Expected milestone |
| --- | --- | --- |
| 1 | CMake/C11 setup; knot-span search; Cox–de Boor basis functions | Tested B-spline basis evaluator |
| 2 | Rational curve evaluation; 3D data structures; Python bindings | NURBS airfoil geometry and visualisation |
| 3 | Analytical control-point and weight sensitivities; JVP/VJP | Verified differentiable geometry module |
| 4 | Forward-mode AD with dual numbers | Forward derivative comparison |
| 5 | Reverse-mode AD and derivative benchmarking | Analytical/AD comparison report |
| 6 | 2D panel-method aerodynamic solver | Pressure and lift validation |
| 7 | Direct sensitivities and discrete aerodynamic adjoint | Verified aerodynamic design gradients |
| 8 | Constrained airfoil inverse design | End-to-end CAD-native optimisation |
| 9 | OpenMDAO geometry and aerodynamic components | Reproducible OpenMDAO example |
| 10 | Inspect DAFoam/MPhys geometry and mesh interfaces | High-fidelity integration design note |
| 11 | CFD proof of concept or strengthen 2D prototype | Verified integration or robust fallback |
| 12 | Documentation, results, release and technical article | Publicly shareable `v0.1.0` |

**Success criterion:** by Week 8, the project should demonstrate a verified adjoint-based 2D optimisation whose final airfoil is regenerated from saved NURBS parameters. Week 12 focuses on reproducibility and publication, regardless of whether high-fidelity CFD coupling is ready.

## Planned repository layout

```text
.
├── CMakeLists.txt
├── README.md
├── AGENTS.md                    # Codex development guidance, when added
├── src/
│   ├── include/
│   ├── nurbs/                   # Basis, curve, geometry sensitivities
│   └── ad/                      # Forward/reverse AD experiments
├── python/
│   └── cad_aero/                # Bindings, panel solver, adjoint, optimisation
├── tests/                       # C and Python tests
├── examples/                    # Reproducible demonstrations
├── integrations/
│   ├── openmdao/
│   └── dafoam/
├── docs/
│   ├── mathematics/
│   ├── architecture/
│   └── results/
└── results/                     # Generated results; avoid committing large data
```

The tree is **proposed**, not a guarantee that every directory currently exists.

## Building and running

> **Not yet available:** installation and execution commands will be added once the first CMake targets, Python bindings, and example scripts exist. This README intentionally avoids advertising unimplemented commands.

The intended development environment is a C11-capable compiler, CMake, Python 3, and a test runner. Debug builds should enable strict compiler warnings and, where supported, sanitizers. A clean build and test procedure will be published with the first runnable milestone.

## Verification and numerical quality

Derivative correctness is a central deliverable, not an afterthought.

| Area | Verification approach |
| --- | --- |
| B-spline basis | Partition of unity, non-negativity, local support, endpoints, repeated knots |
| NURBS geometry | Reference curves, rational arcs, continuity, finite outputs |
| Geometry sensitivities | Analytical vs AD; central finite differences; JVP/VJP transpose identity |
| AD prototype | Known scalar functions, forward/reverse consistency |
| Aerodynamic solver | Panel refinement, symmetry, lift trend, reference comparison |
| Discrete adjoint | Direct sensitivities and finite differences of converged analysis |
| Optimisation | Constraint satisfaction, convergence, reproducible geometry regeneration |
| Integration | Component partial/total derivative checks; mesh consistency where applicable |

Numerical comparisons will report absolute and scaled relative errors, perturbation-size studies, and relevant conditioning information. A single finite-difference step is not sufficient evidence of derivative correctness.

## Planned demonstration

The initial optimisation problem will use a NURBS-parameterised airfoil and a 2D inviscid aerodynamic model. One candidate objective is pressure-distribution inverse design:

\[
\min_{\mathbf p}\;\frac12\sum_j
\left[C_{p,j}(\mathbf p)-C_{p,j}^{\mathrm{target}}\right]^2,
\]

subject to selected control-point bounds and minimum-thickness constraints.

A panel method is suitable for studying the discrete adjoint but **does not predict viscous profile drag**. High-fidelity drag optimisation will be reserved for a CFD-capable extension.

The published demonstration should include initial/final airfoils, control polygons, pressure distributions, gradient checks, convergence histories, and the final saved geometry parameters.

## Known limitations and future work

- Initial tests use 2D airfoils (`z = 0`), although the geometry types are 3D.
- Fixed NURBS topology and fixed sampling parameters simplify differentiability.
- The panel solver is inviscid and intended for method validation.
- Mesh movement remains necessary for volume-mesh CFD; a CAD-native parameterisation is not a substitute for mesh-quality management.
- Full OpenFOAM/DAFoam and MPhys integration is exploratory until demonstrated and verified.
- Future releases may add NURBS surfaces, wing planform variables, CAD exchange, multi-point optimisation, and aerostructural coupling.

## Development approach

Development is incremental and test-driven. Codex may assist with mathematical explanations, C implementation, code reviews, and test design, but numerical algorithms and derivative conventions should be understood and verified before they are accepted. `AGENTS.md`, when present, provides persistent instructions for this workflow.

## References

- Piegl, L. and Tiller, W., *The NURBS Book*, Springer.
- Griewank, A. and Walther, A., *Evaluating Derivatives: Principles and Techniques of Algorithmic Differentiation*, SIAM.
- Katz, J. and Plotkin, A., *Low-Speed Aerodynamics*, Cambridge University Press.
- [OpenMDAO](https://openmdao.org/)
- [MPhys](https://openmdao.github.io/mphys/)
- [DAFoam](https://dafoam.github.io/)
- [OpenFOAM](https://www.openfoam.com/)
- [CMake](https://cmake.org/cmake/help/latest/guide/tutorial/)

## Contributing and licence

Contribution guidelines and a licence will be added before public release. Until a licence is selected and included in the repository, **no open-source reuse rights are granted by default**; the project is *intended* to become open source.

---

**Project vision:** from CAD parameters to aerodynamic adjoints and back to a regenerable CAD geometry—without using FFD as the primary design representation.
