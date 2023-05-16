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

    const size_t dims = y.rows(); // dimensions
    const size_t steps = y.cols(); // steps

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
            model.add(y(d, n + 1) == y(d, n) + ks.col(d).sum());
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
