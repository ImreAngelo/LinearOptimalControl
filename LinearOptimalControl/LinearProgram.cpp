#include "LinearProgram.h"
#include "Matrix.h"
#include "Timer.h"

template<typename T>
using Matrix = MatrixUtil::Matrix<T>;

/// <summary>
/// Integrate exp(-p * t) using trapezoidal rule
/// </summary>
inline IloNumExprArg integrate(const IloEnv& env, const Matrix<IloNumVar>& u, const Matrix<IloNumVar>& y, const Eigen::MatrixXd yPhi, const double dt, const size_t steps, const double t0, const double p)
{
    TIME_FUNCTION();
    
    IloNumVar zero(env, 0, 0);
    IloNumExprArg obj = zero - zero;

    for (auto i = 0; i < steps - 1; i++)
    {
        double a = t0 + i * dt;
        double b = a + dt;

        for (auto j = 0; j < y.rows(); j++)
            obj = obj + ((b - a) / 2.0) * (
                /*std::exp(-p * a) * */(yPhi(0, j) * y(j, i) * y(j, i)              + u(j, i) * u(j, i)) +
                /*std::exp(-p * b) * */(yPhi(0, j) * y(j, i + 1) * y(j, i + 1)      + u(j, i + 1) * u(j, i + 1))
            );
    }

    zero.end(); // This does nothing
    return (obj/2);
}

Linear::Solution Linear::solve_t(const double t0, const double t1, RungeKutta::ButcherTable butcherTable, Func Fc, MatrixT Fy, MatrixT Fu, size_t steps, const Eigen::MatrixXd yPhi, double p)
{
    //TIME_FUNCTION();

    const double dt = (t1 - t0) / steps;
    const size_t m = Fu.cols();

    IloEnv env;
    IloModel model(env);

    // Parameterize u, y
    Matrix<IloNumVar> u(m, steps);
    Matrix<IloNumVar> y(m, steps);

    for (auto j = 0; j < steps; j++) {   // Col
        for (auto i = 0; i < m; i++) { // Row
            u(i, j) = IloNumVar(env, -FLT_MAX, FLT_MAX);
            y(i, j) = IloNumVar(env, -FLT_MAX, FLT_MAX);
        }
    }

    // Build objective function
    const IloNumExprArg obj = integrate(env, u, y, yPhi, dt, steps, t0, p);
    model.add(IloMinimize(env, obj));

    // Discretize State
    RungeKutta::parameterize(model, y, u, Fc, Fy, Fu, dt, t0, butcherTable);

    // Add boundary conditions
    for(auto i = 0; i < m; i++)
        model.add(y(i, 0) == 1);

    //model.add(u(0, 0) == -1.728328996);

    try {
        TIMER_START("CPLEX");

        IloCplex cplex(model);

        cplex.setOut(env.getNullStream());
        cplex.solve();

        TIMER_STOP();

        // Output
        auto control = MultiVector(m);
        auto objective = MultiVector(m);

        for (auto i = 0; i < m; i++)
        {
            auto& ctrl = control[i];
            auto& obj = objective[i];

            ctrl.reserve(steps - 1);
            obj.reserve(steps);

            for (auto j = 0; j < steps - 1; j++) {
                ctrl.emplace_back(cplex.getValue(u(i, j)));
                obj.emplace_back(cplex.getValue(y(i, j)));
            }

            obj.emplace_back(cplex.getValue(y(i, steps - 1)));
        }

        env.end();

        return { control, objective };
    }
    catch (IloException& e) {
        std::cerr << "\n[Error] Concert exception caught: " << e;
    }
    catch (...) {
        std::cerr << "\n[Error] An unknown error occured.";
    }

    std::cerr << "\n[Error] Could not solve, returning 0\n\n";
    return { MultiVector(m, { (double)(steps - 1), 0 }), MultiVector(m, { (double)steps, 0 })};
};
