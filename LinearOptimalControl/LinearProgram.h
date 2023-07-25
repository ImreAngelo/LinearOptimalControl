#pragma once
#include <vector>
#include <ilcplex/ilocplex.h>
#include <Eigen/Core>
#include "RungeKutta.h"

namespace Linear
{
	typedef std::function<double(double)> Func;
	typedef Eigen::Matrix<Func, Eigen::Dynamic, Eigen::Dynamic> MatrixT;
	typedef std::vector<std::vector<double>> MultiVector;

	struct Solution {
		const MultiVector control;
		const MultiVector objective;
	};

	struct Bound {
		const float l = -FLT_MAX;
		const float u = FLT_MAX;
		const IloNumVar::Type type = IloNumVar::Float;
	};

	const std::vector<Bound> def_ub{ {0,1} };
	const std::vector<Bound> def_yb{ {0} };

	/// <summary>
	/// Solve the given linear optimal control problem by complete parameterization
	/// </summary>
	/// <param name="t0">Start of time horizon</param>
	/// <param name="t0">End of time horizon</param>
	/// <param name="bt">Butcher table used to discretize state</param>
	/// <param name="Fc">Constant coefficients</param>
	/// <param name="Fy">State coefficients matrix</param>
	/// <param name="Fu">Control coefficients matrix</param>
	/// <param name="steps">Number of steps used in state discretization</param>
	/// <param name="yPhi">Objective coefficients matrix</param>
	/// <param name="p">Objective (negative) growth term</param>
	/// <param name="ub">Upper and lower bounds for the control</param>
	/// <param name="yb">Upper and lower bounds for the state</param>
	Solution solve(double t0, double t1, RungeKutta::ButcherTable bt, 
		Func Fc, MatrixT Fy, MatrixT Fu, 
		Func Gt, MatrixT Gy, MatrixT Gu,
		Func Ht, MatrixT Hy, MatrixT Hu,
		size_t steps, const Eigen::MatrixXd yPhi, double p = 0, 
		std::vector<Bound> ub = def_ub, std::vector<Bound> yb = def_yb
	);
}

//class LinearProblem
//{
//public:
//	void setDifferentialConstraints() {};
//	void setAlgebraicConstraints() {};
//	void setBoundsU(double max = 1, double min = 0) {};
//	void setBoundsY(double max = DBL_MAX, double min = 0) {};
//	void setAlgebraicInequalityConstraints() {};
//	void setBoundaryConditions() {}; // TODO: Unify interface for constraints and boundary conditions
//	void addBoundaryCondition() {};	// Usually only require a single boundary condition (initial value)
//
//	void solve() {}; // return control?
//	
//	void getControl() const {};
//	void getObjective() const {};
//
//	/// <summary>
//	/// n-dimensional, time-dependent
//	/// </summary>
//	/// <param name="t0"></param>
//	/// <param name="t1"></param>
//	/// <param name="Fc"></param>
//	/// <param name="Fy"></param>
//	/// <param name="Fu"></param>
//	LinearProblem(double t0, double t1, MatrixXd Fc, MatrixXd Fy, MatrixXd Fu) 
//	{
//		// Set up problem
//		assert(Fc.rows() == Fy.rows() && Fc.rows() == Fu.rows());
//		assert(Fc.cols() == Fu.cols() && Fc.cols() == Fu.cols());
//		
//		const size_t dim = Fc.cols();
//		const size_t n = Fc.rows();
//	};
//
//	/// <summary>
//	/// 1-dimensional problems, not time-dependent
//	/// </summary>
//	/// <param name="t0"></param>
//	/// <param name="t1"></param>
//	/// <param name="Fc"></param>
//	/// <param name="Fy"></param>
//	/// <param name="Fu"></param>
//	LinearProblem(double t0, double t1, MatrixXd Fc, MatrixXd Fy, MatrixXd Fu, size_t steps)
//		//: LinearProblem(t0, t1, MatrixXd::Constant(steps, 1, Fc), MatrixXd::Constant(steps, 1, Fy), MatrixXd::Constant(steps, 1, Fu))
//	{
//		// Create time-dependent matrix from constant matrix
//	};
//
//	/// <summary>
//	/// 1-dimensional problems, not time-dependent
//	/// </summary>
//	/// <param name="t0"></param>
//	/// <param name="t1"></param>
//	/// <param name="Fc"></param>
//	/// <param name="Fy"></param>
//	/// <param name="Fu"></param>
//	LinearProblem(double t0, double t1, double Fc, double Fy, double Fu, size_t steps = DEFAULT_STEPSIZE)
//		: LinearProblem(t0, t1, MatrixXd::Constant(1, 1, Fc), MatrixXd::Constant(1, 1, Fy), MatrixXd::Constant(1, 1, Fu), steps)
//	{};
//
//	/// <summary>
//	/// 1-dimensional problems, time-dependent
//	/// </summary>
//	/// <param name="t0"></param>
//	/// <param name="t1"></param>
//	/// <param name="Fc"></param>
//	/// <param name="Fy"></param>
//	/// <param name="Fu"></param>
//	LinearProblem(double t0, double t1, std::function<double(double)> Fc, std::function<double(double)> Fy, std::function<double(double)> Fu, size_t steps = DEFAULT_STEPSIZE)
//	{
//		MatrixXd fc(steps, 1);
//		MatrixXd fy(steps, 1);
//		MatrixXd fu(steps, 1);
//
//		for (auto i = 0; i < steps; i++)
//		{
//			const auto t = i * (t1 - t0) / steps;
//			fc(i) = Fc(t);
//			fy(i) = Fy(t);
//			fu(i) = Fu(t);
//		}
//
//		LinearProblem(t0, t1, fc, fy, fu);
//	};
//
//private:
//	std::vector<double> control;
//	std::vector<double> objective;
//};