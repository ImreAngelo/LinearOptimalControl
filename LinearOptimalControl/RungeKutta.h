#pragma once
#include <vector>
#include "Matrix.h"

// TODO: Fix wierd vector include

namespace RungeKutta
{
    // TODO: Simplify ButcherTable interface (use matrices)

    struct ButcherTable
    {
        const size_t order;

        const std::vector<std::vector<double>> a;
        const std::vector<double> b;
        const std::vector<double> c;

        ButcherTable(std::vector<double> c, std::vector<std::vector<double>> a, std::vector<double> b)
            : order(c.size()), a(a), b(b), c(c)
        {
            assert(c.size() == b.size());
            assert(a.size() == a[0].size());
            assert(a.size() == c.size());
        }
    };

    static const ButcherTable euler{ {0},{{0}},{1} };
    static const ButcherTable heun{ {0,1},{{0,0},{1,0}},{1.0 / 2.0, 1.0 / 2.0} };
    static const ButcherTable rk4{ {0,.5,.5,1}, {{ 0, 0, 0, 0 },{ .5, 0, 0, 0 },{ 0, .5, 0, 0 },{ 0, 0, 1, 0},},{(1/6.0), (1/3.0), (1/3.0), (1/6.0)} };


    // ===== PARAMETERIZATION


	typedef std::function<double(double)> func;
	typedef Eigen::Matrix<func, Eigen::Dynamic, Eigen::Dynamic> Matrix;
	typedef Eigen::Matrix<IloNumVar, Eigen::Dynamic, Eigen::Dynamic> IloMatrix;

	/// <summary>
	/// Complete parameterization using Runge-Kutta
	/// </summary>
	void parameterize(IloModel& model, const IloMatrix& y, const IloMatrix& u, const func& Fc, const Matrix& Fy, const Matrix& Fu, double dt, double t0 = 0, ButcherTable table = euler);
    

    // ===== TIMING


    int debug = 0;

    typedef std::tuple<std::vector<Eigen::MatrixXd>, std::vector<double>> ret;
    
    /// <summary>
    /// Solve ODE with Runge-Kutta method, included for performance testing (see timing branch)
    /// </summary>
    ret solve(const Eigen::MatrixXd& y0, const Matrix& Fc, const Matrix& Fy, const Matrix& Fu, size_t steps, double t1 = 1, double t0 = 0, ButcherTable table = euler);
};
