#pragma once
#include <ilcplex/ilocplex.h>
#include <vector>

class LinearProgram
{
public:
	bool solve(double t0, double t1, size_t n, size_t dimensions = 1);

	std::vector<double> getControl() { return control; };
	std::vector<double> getDynamics() { return dynamics; };

private:
	std::vector<double> control, dynamics;
};
