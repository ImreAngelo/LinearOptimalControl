#include "LinearProgram.h"
#include "Matrix.h"

template<typename T>
using Matrix = MatrixUtil::Matrix<T>;

/// <summary>
/// 
/// </summary>
/// <param name="model"></param>
//void RKDiscretize(IloModel& model, Matrix<IloNum> Fc, Matrix<IloNum> Fy, Matrix<IloNum> Fu, Matrix<IloNumVar> y, Matrix<IloNumVar> u, double dt, double t0 = 0)
//{
//    const size_t n = y.rows();
//    const size_t m = y.cols();
//
//    for (auto j = 0; j < m - 1; j++) {
//        auto t = j * dt + t0;
//        for (auto i = 0; i < n; i++) {
//            auto _c = Fc(j, 0);
//            auto _y = MatrixUtil::dot(Fy.row(j), y.col(j));
//            auto _u = MatrixUtil::dot(Fu.row(j), u.col(j));
//
//            model.add(y(i, j + 1) == y(i, j) + dt * (_c + _y + _u));
//        }
//    }
//}
//
//// TODO: u_steps, y_steps optional
//bool solve(double t0, double t1, size_t steps, size_t dim)
//{
//    double dt = ((t1 - t0) / steps);
//
//    IloEnv env;
//    IloModel model(env);
//
//    // Populate matrices
//    Matrix<IloNumVar> u(dim, steps);
//    Matrix<IloNumVar> y(dim, steps);
//
//    for (auto j = 0; j < steps; j++) {   // Col
//        for (auto i = 0; i < dim; i++) { // Row
//            u(i, j) = IloNumVar(env, -1.0, 1.0);
//            y(i, j) = IloNumVar(env, DBL_MIN, DBL_MAX);
//        }
//    }
//
//    // Discretize
//    Matrix<IloNum> F[3] = {
//        Matrix<IloNum>::Constant(steps, dim, 1.0),      // F_c(t)
//        Matrix<IloNum>::Constant(steps, dim, 0.0),      // F_y(t)
//        Matrix<IloNum>::Constant(steps, dim, -1.0),     // F_u(t)
//    };
//
//    RKDiscretize(model, F[0], F[1], F[2], y, u, dt, t0);
//
//    // Build objective function
//    // TODO: Pass as (optional) parameter
//    Matrix<IloNum> phi[3] = {
//        Matrix<IloNum>::Constant(steps, dim, 1.0),  // integral of y(t)
//        Matrix<IloNum>::Constant(steps, 1, 0.0),    // y(t0)
//        Matrix<IloNum>::Constant(steps, 1, 0.0),    // y(t1)
//    };
//
//    // buildObjective(phi[0], phi[1], phi[2], y)
//    IloNumExprArg obj = MatrixUtil::mulSum(phi[0], y);
//    model.add(IloMinimize(env, obj));
//
//    // Add boundary conditions
//    // TODO: Function
//    model.add(y(0, 0) == 1);
//
//    try {
//        // Solve
//        IloCplex cplex(model);
//        cplex.solve();
//
//        // Output
//        auto dynamics = std::vector<double>(steps);
//        auto control = std::vector<double>(steps);
//        for (auto j = 0; j < steps; j++) {
//            control[j] = cplex.getValue(u(0, j));
//            dynamics[j] = cplex.getValue(y(0, j));
//        }
//
//        return true;
//    }
//    catch (IloException& e) {
//        std::cerr << "Concert exception caught: " << e << std::endl;
//    }
//    catch (...) {
//        std::cerr << "An unknown error occured.";
//    }
//
//    return false;
//}
//
//Linear::Solution Linear::solve(double t0, double t1, MatrixXd Fc, MatrixXd Fy, MatrixXd Fu)
//{
//    const size_t steps = Fc.rows();
//    const double dt = ((t1 - t0) / steps);
//    const size_t dim = Fc.cols();
//
//    std::vector<double> control(steps);
//    std::vector<double> objective(steps);
//
//    IloEnv env;
//    IloModel model(env);
//
//    // Populate matrices
//    Matrix<IloNumVar> u(dim, steps);
//    Matrix<IloNumVar> y(dim, steps);
//
//    for (auto j = 0; j < steps; j++) {   // Col
//        for (auto i = 0; i < dim; i++) { // Row
//            u(i, j) = IloNumVar(env, -1.0, 1.0);
//            y(i, j) = IloNumVar(env, DBL_MIN, DBL_MAX);
//        }
//    }
//
//    // Discretize
//    RKDiscretize(model, Fc, Fy, Fu, y, u, dt, t0);
//
//    // Build objective function
//    // TODO: Pass as (optional) parameter
//    Matrix<IloNum> phi[3] = {
//        Matrix<IloNum>::Constant(steps, dim, 1.0),  // integral of y(t)
//        Matrix<IloNum>::Constant(steps, 1, 0.0),    // y(t0)
//        Matrix<IloNum>::Constant(steps, 1, 0.0),    // y(t1)
//    };
//
//    // buildObjective(phi[0], phi[1], phi[2], y)
//    IloNumExprArg obj = MatrixUtil::mulSum(phi[0], y);
//    model.add(IloMinimize(env, obj));
//
//    // Add boundary conditions
//    // TODO: Function
//    model.add(y(0, 0) == 1);
//
//    try {
//        // Solve
//        IloCplex cplex(model);
//        cplex.solve();
//
//        // Output
//        for (auto j = 0; j < steps; j++) {
//            control[j] = cplex.getValue(u(0, j));
//            objective[j] = cplex.getValue(y(0, j));
//        }
//    }
//    catch (IloException& e) {
//        std::cerr << "Concert exception caught: " << e << std::endl;
//    }
//    catch (...) {
//        std::cerr << "An unknown error occured.";
//    }
//
//    return { control, objective };
//}

Linear::Solution Linear::solve_t(const double t0, const double t1, Func Fc, MatrixT Fy, MatrixT Fu, const size_t steps)
{
    const double dt = (t1 - t0) / steps;
    const size_t dim = Fu.cols();

    IloEnv env;
    IloModel model(env);

    // Populate matrices
    Matrix<IloNumVar> u(dim, steps);
    Matrix<IloNumVar> y(dim, steps);

    // Cheat
    constexpr double max[2] = { DBL_MAX, 0.0 };
    constexpr double min[2] = { 0.0, DBL_MIN };

    for (auto j = 0; j < steps; j++) {   // Col
        for (auto i = 0; i < dim; i++) { // Row
            u(i, j) = dim == 2 ? IloNumVar(env, min[i], max[i]) : IloNumVar(env, 0, 1);
            y(i, j) = IloNumVar(env, 0, DBL_MAX);
        }
    }

    // Discretize
    const size_t n = y.rows();
    const size_t m = y.cols();

    for (auto j = 0; j < m - 1; j++) {
        auto t = j * dt + t0;

        auto _c = Fc(t);
        auto _y = MatrixUtil::mul(MatrixUtil::eval(Fy, t), y.col(j));
        auto _u = MatrixUtil::mul(MatrixUtil::eval(Fu, t), u.col(j));

        for (auto i = 0; i < n; i++)
        {
            model.add(y(i, j + 1) == y(i, j) + dt * (_c + _y(i, 0) + _u(i, 0)));
        }
    }

    // Build objective function
    IloNumExprArg obj = MatrixUtil::mulSum(Matrix<IloNum>::Constant(steps, dim, 1.0), y);
    model.add(IloMinimize(env, obj));

    // Add boundary conditions
    // TODO: Function
    for(auto i = 0; i < dim; i++)
        model.add(y(i, 0) == 1);

    try {
        // Solve
        IloCplex cplex(model);
        cplex.solve();

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


