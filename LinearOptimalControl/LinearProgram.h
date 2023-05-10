#pragma once
#include <vector>
#include <ilcplex/ilocplex.h>
#include <Eigen/Core>

using Eigen::MatrixXd;

namespace Linear
{
	struct Solution {
		const std::vector<double> control;
		const std::vector<double> objective;
	};

	Solution solve(double t0, double t1, MatrixXd Fc, MatrixXd Fy, MatrixXd Fu);
}