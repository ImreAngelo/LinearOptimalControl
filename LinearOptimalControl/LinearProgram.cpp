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

    return obj;
}

/// <summary>
/// Set algebraic constraints
/// </summary>
inline void setConstraints(const IloEnv& env, const Matrix<IloNumVar>& u, const Matrix<IloNumVar>& y)
{
    // ...
}

Linear::Solution Linear::solve(const double t0, const double t1, RungeKutta::ButcherTable butcherTable, Func Fc, MatrixT Fy, MatrixT Fu, Func Gt, MatrixT Gy, MatrixT Gu, Func Ht, MatrixT Hy, MatrixT Hu, size_t steps, const Eigen::MatrixXd yPhi, double p, std::vector<Bound> ub, std::vector<Bound> yb)
{
    //assert(...);
    TIME_FUNCTION();

    const double dt = (t1 - t0) / steps;
    const size_t dim = Fu.cols();

    IloEnv env;
    IloModel model(env);

    // Populate matrices
    Matrix<IloNumVar> u(dim, steps);
    Matrix<IloNumVar> y(dim, steps);

    for (auto j = 0; j < steps; j++) {   // Col
        for (auto i = 0; i < dim; i++) { // Row
            const auto ubi = ub[i % ub.size()], ybi = yb[i % yb.size()];
            u(i, j) = IloNumVar(env, ubi.l, ubi.u, ubi.type);
            y(i, j) = IloNumVar(env, ybi.l, ybi.u, ybi.type);
        }
    }

    // Set algebraic constraints
    // TODO: Consolidate with RungeKutta::parameterize() -> abstract into same helper function
    for (auto n = 0; n < steps; n++)
    {
        const double t = t0 + dt * n;

        const auto gu = MatrixUtil::eval(Gu, t);
        const auto hu = MatrixUtil::eval(Hu, t);
        const auto gy = MatrixUtil::eval(Gy, t);
        const auto hy = MatrixUtil::eval(Hy, t);

        const auto _gu = MatrixUtil::mul(gu, static_cast<Matrix<IloNumVar>>(u.col(n)));
        const auto _hu = MatrixUtil::mul(hu, static_cast<Matrix<IloNumVar>>(u.col(n)));
        const auto _gy = MatrixUtil::mul(gy, static_cast<Matrix<IloNumVar>>(y.col(n)));
        const auto _hy = MatrixUtil::mul(hy, static_cast<Matrix<IloNumVar>>(y.col(n)));

        for (auto d = 0; d < dim; d++)
        {
            model.add(Gt(t) == _gy(d, 0) + _gu(d, 0));
            model.add(Ht(t) <= _hy(d, 0) + _hu(d, 0));
        }
    }

    // Build objective function
    const IloNumExprArg obj = integrate(env, y, yPhi, dt, steps, t0, p);
    model.add(IloMinimize(env, obj));

    // Complete Parameterization
    RungeKutta::parameterize(model, y, u, Fc, Fy, Fu, dt, t0, butcherTable);

    // Add boundary conditions
    // TODO: Boundary matrices as function parameters
    for(auto i = 0; i < dim; i++)
        model.add(y(i, 0) == 1);

    try {
        // Solve
        TIMER_START("CPLEX");

        IloCplex cplex(model);
        cplex.setParam(IloCplex::Threads, 2);
        cplex.setOut(env.getNullStream());

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
