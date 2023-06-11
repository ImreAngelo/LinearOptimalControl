#include "LinearProgram.h"
#include "Matrix.h"
#include "Timer.h"

template<typename T>
using Matrix = MatrixUtil::Matrix<T>;

/// <summary>
/// Integrate exp(-p * t) using trapezoidal rule
/// </summary>
inline IloNumExprArg integrate(const IloEnv& env, const Matrix<IloNumVar>& y, const Eigen::MatrixXd yPhi, const double dt, const size_t steps, const double t0, const double p)
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
                std::exp(-p * a) * (yPhi(0, j) * y(j, i)) + 
                std::exp(-p * b) * (yPhi(0, j) * y(j, i + 1))
            );
    }

    zero.end(); // This does nothing
    return obj;
}

Linear::Solution Linear::solve_t(const double t0, const double t1, RungeKutta::ButcherTable butcherTable, Func Fc, MatrixT Fy, MatrixT Fu, size_t steps, const Eigen::MatrixXd yPhi, double p)
{
    // TIME_FUNCTION();

    const double dt = (t1 - t0) / steps;
    const size_t dim = Fu.cols();

    IloEnv env;
    IloModel model(env);

    // Populate matrices
    Matrix<IloNumVar> u(dim, steps);
    Matrix<IloNumVar> y(dim, steps);

    for (auto j = 0; j < steps; j++) {   // Col
        for (auto i = 0; i < dim; i++) { // Row
            // Cheat for example 3
            u(i, j) = (dim == 2) ? IloNumVar(env, 0, 10, IloNumVar::Float) : IloNumVar(env, 0, 1);
            y(i, j) = IloNumVar(env, 0, FLT_MAX);
        }
    }

    //u(dim - 1, steps - 1).end();

    // Debug example 3 - TODO: Build algebraic constraints from function parameters, see solve()
    if (dim == 2)
    {
        constexpr double a = 5.0;
        constexpr float k1 = 2.0f, k2 = 8.0f;

        for (auto n = 0; n < steps; n++)
        {
            model.add(0 == u(0, n) - a * u(1, n));
            model.add(y(1, n) - u(0, n) / k1 + u(1, n) / k2 >= 0);
        }
    }

    // Build objective function
    const IloNumExprArg obj = integrate(env, y, yPhi, dt, steps, t0, p);
    model.add(IloMinimize(env, obj));

    // Complete Parameterization
    RungeKutta::parameterize(model, y, u, Fc, Fy, Fu, dt, t0, butcherTable);

    // TIMER_START("Set boundary");

    // Add boundary conditions
    // TODO: Boundary matrices as function parameters
    for(auto i = 0; i < dim; i++)
        model.add(y(i, 0) == 1);

    try {
        // Solve
        TIMER_START("CPLEX");

        IloCplex cplex(model);
        cplex.setParam(IloCplex::Threads, 8);

#ifdef FALSE
        std::cout << "\n[CPLEX] ";
#else
        cplex.setOut(env.getNullStream());
#endif // _DEBUG

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
    return { MultiVector(dim, { (double)steps - 1, 0 }), MultiVector(dim, { (double)steps, 0 })};
};
