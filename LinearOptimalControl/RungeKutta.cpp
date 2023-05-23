#include "RungeKutta.h"
#include "Timer.h"

template<typename T>
using Matrix = MatrixUtil::Matrix<T>;

// TODO: Keep as implicit version!
void RungeKutta::parameterize(IloModel& model, const IloMatrix y, const IloMatrix u, const func& Fc, const FMatrix& Fy, const FMatrix& Fu, double dt, double t0, ButcherTable table)
{
    TIME_SCOPE("Runge Kutta (parameterization)");

    const size_t dims = y.rows();
    const size_t steps = y.cols();
    const IloEnv env = model.getEnv();

    for (auto n = 0; n < steps - 1; n++)
    {
        const double t = n * dt + t0;

        // Each row is a order, and each column is a dimension
        IloNumVar z(env, -FLT_MAX, FLT_MAX, ILOFLOAT);
        IloNumExpr zero = z - z;

        Matrix<IloNumExpr> k(table.order, dims);
        for (auto i = 0; i < table.order; i++)
            for(auto j = 0; j < dims; j++)
                k(i,j) = zero;

        // ===== Intermediary step
        // *** TODO: 
        //  - Use matrix operations instead of std::vectors
        //  - I.e. sum = [a.row(i) * k(i)].sum()
        //  - Rename variables (dims = s etc.)

        for (auto i = 0; i < table.order; i++)
        {
            // Sum of a_{i,j} * k_j
            Matrix<IloNumExpr> sum(dims, 1);

            for (auto j = 0; j < dims; j++)
            {
                // NOTE: 
                IloNumExpr expr = k(0, j) * table.a[i][0];

                for (auto ii = 1; ii < table.order; ii++)
                    expr = expr + k(ii, j) * table.a[i][ii];
                
                sum(j) = expr;
            }

            // Build k_i
            const double ct = t + table.c[i] * dt;

            Eigen::MatrixXd fy = MatrixUtil::eval(Fy, ct);
            Eigen::MatrixXd fu = MatrixUtil::eval(Fu, ct);

            auto _c = Fc(ct);
            auto _s = MatrixUtil::mul(fy, sum);
            auto _y = MatrixUtil::mul(fy, (IloMatrix)(y.col(n)));
            auto _u = MatrixUtil::mul(fu, (IloMatrix)(u.col(n)));
            //auto _v = MatrixUtil::mul(fu, sum);

            for (auto j = 0; j < dims; j++)
                k(i, j) = dt * (_c + _y(j, 0) + _s(j) + _u(j, 0));
        }

        // ===== y_{n+1}

        for (auto d = 0; d < dims; d++)
        {
            IloNumExpr biki = k(0, d) * table.b[0];

            for (auto i = 1; i < table.order; i++)
                biki = biki + k(i, d) * table.b[i];

            model.add(y(d, n + 1) == y(d, n) + biki);
        }
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
