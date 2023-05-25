#pragma once
#include <vector>


namespace Example
{
	typedef std::vector<double> vf;

	std::tuple<vf,vf> getSolution(size_t steps, double t0 = 0, double t1 = 1)
	{
		const double dt = (t1 - t0) / steps;

		std::vector<double> x(steps, 0);
		std::vector<double> u(steps, 0);

		for (auto i = 0; i < steps; i++)
		{
			const double t = t0 + i * dt;
			x[i] = (2 * exp(3 * t) + exp(3)) / (exp(3 * t / 2) * (2 + exp(3)));
			u[i] = (2 *(exp(3 * t) - exp(3)))/ (exp(3 * t / 2) * (2 + exp(3)));
		}


		return std::make_tuple<vf,vf>(std::move(u), std::move(x));
	}


}