#include "RungeKutta.h"
#include "Timer.h"

void RungeKutta::parameterize(IloModel& model, const IloMatrix& y, const IloMatrix& u, const func& Fc, const Matrix& Fy, const Matrix& Fu, double dt, double t0, ButcherTable table)
{
    TIME_SCOPE("Runge Kutta");

    using MatrixUtil::mul;
    using MatrixUtil::eval;
    using MatrixUtil::add;
    using MatrixUtil::scalarMul;
    using MatrixUtil::scalarAdd;

    const size_t dims = y.rows();
    const size_t steps = y.cols();

    // y_n+1
    for (auto n = 0; n < steps - 1; n++)
    {
        const auto t = n * dt + t0;

        std::vector<MatrixUtil::Matrix<IloNumExprArg>> k;
        k.reserve(table.order);

        for (auto i = 0; i < table.order; i++)
        {
            // TODO: simply a.row(i) * k
            // sum of a_{i,j} * k_j
            MatrixUtil::Matrix<IloNumExprArg> sum = MatrixUtil::Matrix<IloNumExprArg>::Constant(dims, dims, u(0,0) - u(0,0));
          
            // explicit
            for (auto j = 0; j < i; j++)
            {
                sum = scalarMul(k[j], table.a[i][j]) + sum;
            }

            auto ct = t + table.c[i] * dt;
            MatrixUtil::Matrix<IloNumExprArg> fyy = mul(eval(Fy, ct), y).col(n);
            MatrixUtil::Matrix<IloNumExprArg> fyc = mul(eval(Fy, ct), sum);
            MatrixUtil::Matrix<IloNumExprArg> fu = mul(eval(Fu, ct), u).col(n);

            MatrixUtil::Matrix<IloNumExprArg> f = (fyy + fyc + fu);

            //std::cout << "f: (" << f.rows() << ", " << f.cols() << ")" << std::endl;

            // dt * f(t_{n+c_i}, y_n + sum)
            k.emplace_back(scalarAdd(scalarMul(f, dt), Fc(ct) * dt));
        }

        // ===== 

        MatrixUtil::Matrix<IloNumExprArg> ks = MatrixUtil::Matrix<IloNumExprArg>::Constant(dims, dims, u(0, 0) - u(0, 0));

        for (auto i = 0; i < table.order; i++)
            ks = scalarMul(k[i], table.b[i]) + ks;

        for (auto d = 0; d < dims; d++)
            model.add(y(d, n + 1) == y(d, n) + ks.row(d).sum());
    }

    /*
    for (auto j = 0; j < m - 1; j++) {
        auto t = j * dt + t0;

        auto _c = Fc(t);
        auto _y = mul(eval(Fy, t), y.col(j));
        auto _u = mul(eval(Fu, t), u.col(j));

        for (auto i = 0; i < n; i++)
            model.add(y(i, j + 1) == y(i, j) + dt * (_c + _y(i, 0) + _u(i, 0)));
    }
    */
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

        Eigen::MatrixXd kSum = k[0] * table.b[0];

        for (auto i = 1; i < table.order; i++)
            kSum = kSum + table.b[i] * k[i];

        y.emplace_back(y[n] + kSum);
        t.emplace_back(tn);
    }

    return RungeKutta::ret(y, t);
}
