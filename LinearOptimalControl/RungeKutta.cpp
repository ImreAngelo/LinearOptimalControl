#include "RungeKutta.h"
#include "Timer.h"

template<typename T>
using Matrix = MatrixUtil::Matrix<T>;

void RungeKutta::parameterize(IloModel& model, const IloMatrix y, const IloMatrix u, const func& Fc, const FMatrix& Fy, const FMatrix& Fu, double dt, double t0, ButcherTable table)
{
    TIME_SCOPE("Runge Kutta (parameterization)");

    //using MatrixUtil::mul;
    //using MatrixUtil::eval;
    //using MatrixUtil::add;
    //using MatrixUtil::scalarMul;
    //using MatrixUtil::scalarAdd;

    const size_t dims = y.rows();
    const size_t steps = y.cols();
    const IloEnv env = model.getEnv();

    assert(dims == 1);

    std::cout << "Solving problem of " << dims << " dimensions and " << steps << " steps\n\n";

    for (auto n = 0; n < steps - 1; n++)
    {
        const double t = n * dt + t0;

        Matrix<IloNumVar> k(table.order, 1);
        for (auto i = 0; i < table.order; i++)
            k(i) = IloNumVar(env, -FLT_MAX, FLT_MAX);

        // ===== Intermediary step
        // *** TODO: 
        //  - Use matrix operations instead of vectors
        //  - I.e. sum = [a.row(i) * k(i)].sum()
        //  - Try implicit methods
        //  - Rename variables to fit (dims = s)

        for (auto i = 0; i < table.order; i++)
        {
            // Sum of a_{i,j} * k_j
            IloNumExpr sum = k(0) * table.a[i][0];

            // Implicit
            for (auto j = 1; j < table.order; j++)
                sum = sum + k(j) * table.a[i][j];

            // Build function
            const double ct = t + table.c[i] * dt;

            //Eigen::MatrixXd fc = Eigen::MatrixXd::Constant(dims, dims, Fc(ct));
            Eigen::MatrixXd fy = MatrixUtil::eval(Fy, ct);
            Eigen::MatrixXd fu = MatrixUtil::eval(Fu, ct);

            //for(auto d = 0; d < dims; d++)
            //{
                auto _c = Fc(ct); // fc.col(d).sum();
                auto _y = fy(0,0) * y(0, n) + fy(0, 0) * sum;
                auto _u = fu(0,0) * u(0, n);

                model.add(k(i) == dt * (_c + _y + _u));
            //}
        }

        // ===== y_{n+1}

        IloNumExpr biki = k(0) * table.b[0];

        for (auto i = 1; i < table.order; i++)
            biki = biki + k(i) * table.b[i];

        model.add(y(0, n + 1) == y(0, n) + biki);
    }
}

RungeKutta::ret RungeKutta::solve(const Eigen::MatrixXd& y0, const FMatrix& Fc, const FMatrix& Fy, const FMatrix& Fu, size_t steps, double t1, double t0, ButcherTable table)
{
    TIME_SCOPE("Runge-Kutta (literal types)");

    const size_t dims = Fy.rows();
    const double dt = (t1 - t0)/steps;

    std::vector<Eigen::MatrixXd> y = { y0 };
    y.reserve(steps);

    std::vector<double> t;
    t.reserve(steps);

    for (auto n = 0; n < steps - 1; n++)
    {
        std::vector<Eigen::MatrixXd> k;
        k.reserve(table.order);

        const double tn = n * dt + t0;

        for (auto i = 0; i < table.order; i++)
        {
            Eigen::MatrixXd sum = Eigen::MatrixXd::Constant(dims, dims, 0.0);

            for (auto j = 0; j < i; j++)
            {
                sum = sum + table.a[i][j] * k[j];
            }

            const double t = tn + table.c[i] * dt;

            auto fc = MatrixUtil::eval(Fc, t);
            auto fy = MatrixUtil::eval(Fy, t);
            //auto fu = MatrixUtil::eval(Fu, t);

            k.emplace_back(dt * (fc + fy * (y[n] + sum)));
        }

        //Eigen::MatrixX2d b();

        Eigen::MatrixXd kSum = k[0] * table.b[0];

        for (auto i = 1; i < table.order; i++)
            kSum = kSum + table.b[i] * k[i];

        y.emplace_back(y[n] + kSum);
        t.emplace_back(tn);
    }

    return RungeKutta::ret(y, t);
}
