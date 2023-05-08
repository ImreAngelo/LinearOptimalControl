#pragma once
#include <vector>

class LinearProgram
{
public:
	void solve(double t0, double t1, size_t n);

	void solve_mat(double t0, double t1, size_t steps, size_t dimensions = 1);

	std::vector<double> getControl() { return control; };
	std::vector<double> getDynamics() { return dynamics; };

private:
	std::vector<double> control, dynamics;
};
