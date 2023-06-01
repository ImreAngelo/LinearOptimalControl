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
            assert(order > 0);
        }
    };

    static const ButcherTable backward_euler{ {1},{{1}},{1} };
    static const ButcherTable euler{ {0},{{0}},{1} };
    static const ButcherTable heun{ {0, 1}, { {0,0},{1,0} }, { 1.0 / 2.0, 1.0 / 2.0 } };
    static const ButcherTable rk4{ {0,.5,.5,1}, {{ 0, 0, 0, 0 },{ .5, 0, 0, 0 },{ 0, .5, 0, 0 },{ 0, 0, 1, 0},},{(1/6.0), (1/3.0), (1/3.0), (1/6.0)} };
    static const ButcherTable ralston4{ {0,.4,.45573725,1}, {{ 0, 0, 0, 0 },{ .4, 0, 0, 0 },{ 0, .29697761, .15875964, 0 },{ .21810040, -3.05096516, 3.83286476, 0},},{(.17476028), (-.55148066), (1.20553560), (.17118478)} };


    // ===== PARAMETERIZATION


	typedef std::function<double(double)> func;
	typedef Eigen::Matrix<func, Eigen::Dynamic, Eigen::Dynamic> FMatrix;
	typedef Eigen::Matrix<IloNumVar, Eigen::Dynamic, Eigen::Dynamic> IloMatrix;

	/// <summary>
	/// Complete parameterization using Runge-Kutta
	/// </summary>
	void parameterize(IloModel& model, const IloMatrix y, const IloMatrix u, const func& Fc, const FMatrix& Fy, const FMatrix& Fu, double dt, double t0 = 0, ButcherTable table = euler);
    
    ButcherTable getTable(int method)
    {
        switch (method)
        {
        default:
        case 0: return RungeKutta::euler;
        case 1: return RungeKutta::backward_euler;
        case 2: return RungeKutta::heun;
        case 3: return RungeKutta::rk4;
        case 4: return RungeKutta::ralston4;
        }
    }


    // ===== TIMING


    typedef std::tuple<std::vector<Eigen::MatrixXd>, std::vector<double>> ret;
    
    /// <summary>
    /// Solve ODE with Runge-Kutta method, included for performance testing (see timing branch)
    /// </summary>
    ret solve(const Eigen::MatrixXd& y0, const FMatrix& Fc, const FMatrix& Fy, const FMatrix& Fu, size_t steps, double t1 = 1, double t0 = 0, ButcherTable table = euler);
};
