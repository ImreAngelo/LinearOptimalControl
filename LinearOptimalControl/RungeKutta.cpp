#include "RungeKutta.h"
#include "Timer.h"

MatrixUtil::Matrix<IloNumExprArg> makeZero(IloNumVar temp, size_t dims)
{
    MatrixUtil::Matrix<IloNumExprArg> sum(dims, dims);
    for (auto ii = 0; ii < dims; ii++)
        for (auto jj = 0; jj < dims; jj++)
        {
            IloNumExprArg obj = temp - temp;
            sum(ii, jj) = obj;
        }
    return sum;
}

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

    // y_n+1
    for (auto n = 0; n < steps - 1; n++)
    {
        const auto t = n * dt + t0;

        std::vector<MatrixUtil::Matrix<IloNumExprArg>> k;
        k.reserve(table.order);

        for (auto i = 0; i < table.order; i++)
        {
            const double ct = t + table.c[i] * dt;
            Eigen::MatrixXd fy = eval(Fy, ct);

            // TODO: simply a.row(i) * k
            // sum of a_{i,j} * k_j
            MatrixUtil::Matrix<IloNumExprArg> sum = makeZero(y(0,0), dims);
          
            // explicit
            for (auto j = 0; j < i; j++)
            {
                auto intermediate = k[j];
                for (auto ii = 0; ii < dims; ii++)
                    for (auto jj = 0; jj < dims; jj++)
                        intermediate(ii, jj) = k[j](ii, jj) * fy(ii, jj) * table.a[i][j];

                sum = intermediate + sum;
            }

            MatrixUtil::Matrix<IloNumExprArg> fyy = mul(fy, y).col(n);
            //MatrixUtil::Matrix<IloNumExprArg> fyc = mul(fy, sum); // TODO: This line causes problems!
            MatrixUtil::Matrix<IloNumExprArg> fu = mul(eval(Fu, ct), u).col(n);

            MatrixUtil::Matrix<IloNumExprArg> f = (sum + fyy /*+ fyc*/ + fu);

            // dt * f(t_{n+c_i}, y_n + sum)
            auto ki = scalarAdd(scalarMul(f, dt), Fc(ct) * dt);
            k.emplace_back(ki);
        }

        // ===== 

        MatrixUtil::Matrix<IloNumExprArg> ks = makeZero(y(0,0), dims); // scalarMul(k[0], table.b[0]);

        for (auto i = 0; i < table.order; i++)
        {
            auto temp1 = k[i];
            auto temp2 = ks; // scalarMul(k[i], table.b[i]) + ks;
            MatrixUtil::Matrix<IloNumExprArg> temp3 = temp1 + temp2;
            ks = temp3;
        }

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

        //Eigen::MatrixX2d b();

        Eigen::MatrixXd kSum = k[0] * table.b[0];

        for (auto i = 1; i < table.order; i++)
            kSum = kSum + table.b[i] * k[i];

        y.emplace_back(y[n] + kSum);
        t.emplace_back(tn);
    }

    return RungeKutta::ret(y, t);
}
