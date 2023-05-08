#include "LinearProgram.h"
#include <Eigen/Core>

template <typename T>
using Matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;
template <typename T>
using RowVec = Eigen::Matrix<T, Eigen::Dynamic, 1>;

IloNumExprArg buildObjective(Matrix<IloNum> phi0, RowVec<IloNum> phi1, RowVec<IloNum> phi2, Matrix<IloNumVar> y)
{
    assert(phi0.cols() == y.rows());
    assert(phi1.rows() == y.cols());
    assert(phi2.rows() == y.cols());

    IloNumExprArg obj = y(0,0) - y(0,0);

    // TODO: Integrate y properly
    // TODO: Include phi1 * y(0) + phi2 * y(end)
    for (auto i = 0; i < y.rows(); i++) {
        for (auto j = 0; j < y.cols(); j++) {
            for (auto r = 0; r < y.rows(); r++) {
                obj = obj + phi0(i, r) * y(r, j); // * exp(0)
            }
        }
    }

    return obj;
}


// TODO: u_steps, y_steps optional
bool LinearProgram::solve(double t0, double t1, size_t steps, size_t dim)
{
    // assert(n == phi.size());

    double dt = ((t1 - t0) / steps);

    IloEnv env;
    IloModel model(env);

    // Populate matrices
    Matrix<IloNumVar> u(dim, steps);
    Matrix<IloNumVar> y(dim, steps);

    for (auto j = 0; j < steps; j++) {   // Col
        for (auto i = 0; i < dim; i++) { // Row
            u(i, j) = IloNumVar(env, -1.0, 1.0);
            y(i, j) = IloNumVar(env, DBL_MIN, DBL_MAX);
        }
    }

    // Discretize
    for (auto j = 0; j < steps - 1; j++) {
        auto t = j * dt + t0;
        for (auto i = 0; i < dim; i++) {
            model.add(y(i, j + 1) == y(i,j) + dt * (0 + 0 * y(0, j) - 1 * u(0, j)));
        }
    }

    // Build objective function
    // TODO: Pass as (optional) parameter
    Matrix<IloNum> phi[3] = {
        Matrix<IloNum>::Constant(steps, dim, 1.0),  // integral of y(t)
        Matrix<IloNum>::Constant(steps, 1, 0.0),    // y(t0)
        Matrix<IloNum>::Constant(steps, 1, 1.0),    // y(t1)
    };

    model.add(IloMinimize(env, buildObjective(phi[0], phi[1], phi[2], y)));

    // Add boundary conditions
    model.add(y(0, 0) == 1);

    try {
        // Solve
        IloCplex cplex(model);
        cplex.solve();

        // Output
        dynamics = std::vector<double>(steps);
        control = std::vector<double>(steps);
        for (auto j = 0; j < steps; j++) {
            control[j] = cplex.getValue(u(0, j));
            dynamics[j] = cplex.getValue(y(0, j));
        }

        return true;
    }
    catch (IloException& e) {
        std::cerr << "Concert exception caught: " << e << std::endl;
    }
    catch (...) {
        std::cerr << "An unknown error occured.";
    }

    return false;
}
