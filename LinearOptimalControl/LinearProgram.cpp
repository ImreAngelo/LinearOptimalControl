#include "LinearProgram.h"
#include <ilcplex/ilocplex.h>
#include <Eigen/Core>

template <typename T>
using Matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;

void LinearProgram::solve(double t0, double t1, size_t n)
{
	float dt = ((t1 - t0) / n);
    
    dynamics = std::vector<double>(n);
    control = std::vector<double>(n);
	
	IloEnv env;
	IloModel model(env);

	IloNumVarArray u(env, n, 0.0, 1.0);
	IloNumVarArray y(env, n, DBL_MIN, DBL_MAX);

    // Constraint on each point u_i (Eulers method)
    for (auto i = 0; i < n - 1; i++) {
        auto t = i * dt + t0;
        model.add(y[i + 1] == y[i] + dt * (-0.8f * u[i]));
    }

    // Objective = Integral of y
    IloNumExprArg obj = u[n - 1].asNumExpr();
    for (auto i = 0; i < n; i++)
        obj = obj + (y[i] / 2);

    model.add(IloMinimize(env, obj));

    // Initial value
    model.add(y[0] == 1);

	try {   
        // Solve
        IloCplex cplex(model);
        cplex.solve();

        for (auto i = 0; i < n; i++) {
             control[i] = cplex.getValue(u[i]);
            dynamics[i] = cplex.getValue(y[i]);
        }
    }
    catch (IloException& e) {
        std::cerr << "Concert exception caught: " << e << std::endl;
    }
    catch(...) {
        std::cerr << "An unknown error occured.";
    }
}

// TODO: u_steps, y_steps optional
void LinearProgram::solve_mat(double t0, double t1, size_t steps, size_t dim)
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
            u(i, j) = IloNumVar(env, 0.0, 1.0);
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
    IloNumExprArg obj = y(0, 0);
    for (auto j = 0; j < steps; j++) {
        for (auto i = 0; i < dim; i++) {
            obj = obj + y(i, j);
        }
    }

    model.add(IloMinimize(env, obj - y(0, 0)));

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
    }
    catch (IloException& e) {
        std::cerr << "Concert exception caught: " << e << std::endl;
    }
    catch (...) {
        std::cerr << "An unknown error occured.";
    }
}
