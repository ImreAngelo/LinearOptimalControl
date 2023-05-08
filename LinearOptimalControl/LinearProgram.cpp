#include "LinearProgram.h"
#include <ilcplex/ilocplex.h>

void LinearProgram::solve(double t0, double t1, size_t n)
{
	float dt = ((t1 - t0) / n);
    
    dynamics = std::vector<double>(n);
    control = std::vector<double>(n);
	
	IloEnv env;
	IloModel model(env);

	IloNumVarArray u(env, n, 0.0, 1.0);
	IloNumVarArray y(env, n, DBL_MIN, DBL_MAX);

    // Constraint on each point u_i (Eulers method)
    for (auto i = 0; i < n - 1; i++) {
        auto t = i * dt + t0;
        model.add(y[i + 1] == y[i] + dt * (-0.8f * u[i]));
    }

    // Objective = Integral of y
    IloNumExprArg obj = u[n - 1].asNumExpr();
    for (auto i = 0; i < n; i++)
        obj = obj + (y[i] / 2);

    model.add(IloMinimize(env, obj));

    // Initial value
    model.add(y[0] == 1);

	try {   
        // Solve
        IloCplex cplex(model);
        cplex.solve();

        for (auto i = 0; i < n; i++) {
             control[i] = cplex.getValue(u[i]);
            dynamics[i] = cplex.getValue(y[i]);
        }
    }
    catch (IloException& e) {
        std::cerr << "Concert exception caught: " << e << std::endl;
    }
    catch(...) {
        std::cerr << "An unknown error occured.";
    }
}
