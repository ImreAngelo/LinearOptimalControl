#include "LinearProgram.h"
#include "Matrix.h"
#include "Timer.h"
#include "RungeKutta.h"

template<typename T>
using Matrix = MatrixUtil::Matrix<T>;

/// <summary>
/// Integrate exp(-p * t) using trapezoidal rule
/// </summary>
inline IloNumExprArg integrate(const Matrix<IloNumVar>& y, const double dt, const size_t steps, const double t0 = 0, const double p = 0)
{
    TIMER_START("Build objective");

    IloNumExprArg obj = y(0, 0) - y(0, 0);

    for (auto i = 0; i < steps - 1; i++)
    {
        double a = t0 + i * dt;
        double b = a + dt;

        obj = obj + ((b - a) / 2 * (std::exp(-p * a) * y(0, i) /*y.col(i).sum()*/ + std::exp(-p * b) * y(0, i + 1) /*y.col(i + 1).sum()*/));
    }

    return obj;
}

Linear::Solution Linear::solve_t(const double t0, const double t1, Func Fc, MatrixT Fy, MatrixT Fu, const size_t steps)
{
    TIME_FUNCTION();

    const double dt = (t1 - t0) / steps;
    const size_t dim = Fu.cols();

    IloEnv env;
    IloModel model(env);

    // Populate matrices
    Matrix<IloNumVar> u(dim, steps);
    Matrix<IloNumVar> y(dim, steps);

    // Cheat for example 3
    constexpr double max[2] = { DBL_MAX, 0.0 };
    constexpr double min[2] = { 0.0, DBL_MIN };

    for (auto j = 0; j < steps; j++) {   // Col
        for (auto i = 0; i < dim; i++) { // Row
            u(i, j) = dim == 2 ? IloNumVar(env, min[i], max[i]) : IloNumVar(env, 0, 1);
            y(i, j) = IloNumVar(env, 0, DBL_MAX);
        }
    }

    // For timing
    RungeKutta::ButcherTable butcherTable = (RungeKutta::debug == 0) ? RungeKutta::euler : 
                                            (RungeKutta::debug == 1) ? RungeKutta::heun : RungeKutta::rk4;

    // Complete Parameterization
    RungeKutta::parameterize(model, y, u, Fc, Fy, Fu, dt, t0, butcherTable);

    // Build objective function
    const IloNumExprArg obj = integrate(y, dt, steps);
    model.add(IloMinimize(env, obj));

    TIMER_START("Set boundary");

    // Add boundary conditions
    // TODO: Function
    for(auto i = 0; i < dim; i++)
        model.add(y(i, 0) == 1);

    try {
        // Solve
        TIMER_START("CPLEX");

        IloCplex cplex(model);
        cplex.solve();

        TIMER_STOP();

        // Output
        auto control = MultiVector(dim);
        auto objective = MultiVector(dim);

        for (auto i = 0; i < dim; i++)
        {
            auto& ctrl = control[i];
            auto& obj = objective[i];

            ctrl.reserve(steps - 1);
            obj.reserve(steps - 1);

            for (auto j = 0; j < steps - 1; j++) {
                ctrl.emplace_back(cplex.getValue(u(i, j)));
                obj.emplace_back(cplex.getValue(y(i, j)));
            }
        }

        cplex.end();
        return { control, objective };
    }
    catch (IloException& e) {
        std::cerr << "Concert exception caught: " << e << std::endl;
    }
    catch (...) {
        std::cerr << "An unknown error occured.";
    }

    throw "failed";
    return {};
};


