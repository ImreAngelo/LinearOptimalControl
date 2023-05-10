#pragma once
#include <vector>
#include <ilcplex/ilocplex.h>
#include <Eigen/Core>

using Eigen::MatrixXd;

constexpr size_t DEFAULT_STEPSIZE = 500;

namespace Linear
{
	struct Solution {
		const std::vector<double> control;
		const std::vector<double> objective;
	};

	Solution solve(double t0, double t1, MatrixXd Fc, MatrixXd Fy, MatrixXd Fu);
}

class LinearProblem
{
public:
	void setDifferentialConstraints() {};
	void setAlgebraicConstraints() {};
	void setBoundsU(double min = 0, double max = 1) {};
	void setAlgebraicInequalityConstraints() {};
	void setBoundaryConditions() {};
	void addBoundaryCondition() {};	// Usually only require a single boundary condition (initial value)

	void solve() {}; // return control?
	
	void getControl() const {};
	void getObjective() const {};

	/// <summary>
	/// n-dimensional
	/// </summary>
	/// <param name="t0"></param>
	/// <param name="t1"></param>
	/// <param name="Fc"></param>
	/// <param name="Fy"></param>
	/// <param name="Fu"></param>
	LinearProblem(double t0, double t1, MatrixXd Fc, MatrixXd Fy, MatrixXd Fu) 
	{
		// Set up problem
		assert(Fc.rows() == Fy.rows() && Fc.rows() == Fu.rows());
		assert(Fc.cols() == Fu.cols() && Fc.cols() == Fu.cols());
		
		const size_t dim = Fc.cols();
		const size_t n = Fc.rows();
	};

	/// <summary>
	/// 1-dimensional problems, not time-dependent
	/// </summary>
	/// <param name="t0"></param>
	/// <param name="t1"></param>
	/// <param name="Fc"></param>
	/// <param name="Fy"></param>
	/// <param name="Fu"></param>
	LinearProblem(double t0, double t1, double Fc, double Fy, double Fu, size_t steps = DEFAULT_STEPSIZE)
		: LinearProblem(t0, t1, MatrixXd::Constant(steps, 1, Fc), MatrixXd::Constant(steps, 1, Fy), MatrixXd::Constant(steps, 1, Fu))
	{};

	/// <summary>
	/// 1-dimensional problems, time-dependent
	/// </summary>
	/// <param name="t0"></param>
	/// <param name="t1"></param>
	/// <param name="Fc"></param>
	/// <param name="Fy"></param>
	/// <param name="Fu"></param>
	LinearProblem(double t0, double t1, std::function<double(double)> Fc, std::function<double(double)> Fy, std::function<double(double)> Fu, size_t steps = DEFAULT_STEPSIZE)
	{
		MatrixXd fc(steps, 1);
		MatrixXd fy(steps, 1);
		MatrixXd fu(steps, 1);

		for (auto i = 0; i < steps; i++)
		{
			const auto t = i * (t1 - t0) / steps;
			fc(i) = Fc(t);
			fy(i) = Fy(t);
			fu(i) = Fu(t);
		}

		LinearProblem(t0, t1, fc, fy, fu);
	};

private:
	std::vector<double> control;
	std::vector<double> control;
};