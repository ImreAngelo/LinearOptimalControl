#include "RungeKutta.h"
#include "Timer.h"

void RungeKutta::parameterize(IloModel& model, const IloMatrix& y, const IloMatrix& u, const func& Fc, const Matrix& Fy, const Matrix& Fu, double dt, double t0, ButcherTable table)
{
    TIME_SCOPE("Runge Kutta (parameterization)");

    using MatrixUtil::mul;
    using MatrixUtil::eval;
    using MatrixUtil::add;
    using MatrixUtil::scalarMul;
    using MatrixUtil::scalarAdd;

    const size_t dims = y.rows();
    const size_t steps = y.cols();

    for (auto n = 0; n < steps - 1; n++)
    {
        const auto t = n * dt + t0;

        std::vector<MatrixUtil::Matrix<IloNumExprArg>> k;
        k.reserve(table.order);

        // ===== k_i

        for (auto i = 0; i < table.order; i++)
        {
            const double ct = t + table.c[i] * dt;
            Eigen::MatrixXd fy = eval(Fy, ct);
            Eigen::MatrixXd fu = eval(Fu, ct);

            //// TODO: simply a.row(i) * k
            //// sum of a_{i,j} * k_j
            MatrixUtil::Matrix<IloNumExprArg> sum = MatrixUtil::Matrix<IloNumExprArg>::Constant(dims, 1, u(0,0) - u(0,0));
          
            // explicit
            for (auto j = 0; j < i; j++)
            {
                if (table.a[i][j] == 0)
                    continue;

                // Fy * sum(a*k)
                for (auto d = 0; d < dims; d++) {
                    IloNumExprArg s = u(0, 0) - u(0, 0);

                    for (auto r = 0; r < dims; r++) {
                        s = s + fy(d, r) * sum(r, 0);
                    }

                    sum(d, 0) = s;
                }
            }

            // TODO: MAKE SURE THE DIMENSIONS ARE CORRECT
            MatrixUtil::Matrix<IloNumVar> yCol = y.col(n);
            MatrixUtil::Matrix<IloNumVar> uCol = u.col(n);

            MatrixUtil::Matrix<IloNumExprArg> _y = mul(fy, yCol);
            MatrixUtil::Matrix<IloNumExprArg> _u = mul(fu, uCol);

            MatrixUtil::Matrix<IloNumExprArg> ki = (sum + _y + _u);

            for (auto ii = 0; ii < dims; ii++)
                ki(ii, 0) = (ki(ii, 0) + Fc(ct)) * dt;

            // dt * f(t_{n+c_i}, y_n + sum)
            k.emplace_back(ki);
        }

        // ===== y_{n+1}

        MatrixUtil::Matrix<IloNumExprArg> ks = MatrixUtil::Matrix<IloNumExprArg>::Constant(dims, 1, u(0, 0) - u(0, 0));

        for (auto i = 0; i < table.order; i++)
        {
            /*if (table.b[i] == 0)
                continue;*/

            auto inter = k[i];
            for (auto ii = 0; ii < dims; ii++)
                ks(ii, 0) = table.b[i] * inter(ii, 0) + ks(ii, 0);
        }

        for (auto d = 0; d < dims; d++)
            model.add(y(d, n + 1) == y(d, n) + ks(d,0));
    }
}

RungeKutta::ret RungeKutta::solve(const Eigen::MatrixXd& y0, const Matrix& Fc, const Matrix& Fy, const Matrix& Fu, size_t steps, double t1, double t0, ButcherTable table)
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
