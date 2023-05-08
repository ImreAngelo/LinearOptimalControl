#pragma once

#include <Eigen/Core>
#include <ilcplex/ilocplex.h>

/// <summary>
/// Adds support for CPLEX types with Eigen (ignores optimizations)
/// </summary>
namespace MatrixUtil
{
    template <typename T>
    using Matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;

    // 
    Matrix<IloNumExprArg> mul(Matrix<IloNum> lhs, Matrix<IloNumVar> rhs)
    {
        assert(lhs.cols() == rhs.rows());
        Matrix<IloNumExprArg> expr(lhs.rows(), rhs.cols());

        for (auto i = 0; i < lhs.rows(); i++) {
            for (auto j = 0; j < rhs.cols(); j++) {
                IloNumExprArg sum = rhs(0, 0) - rhs(0, 0);
                for (auto r = 0; r < rhs.rows(); r++) {
                    sum = sum + lhs(i, r) * rhs(r, j);
                }

                expr(i, j) = sum;
            }
        }

        return expr;
    }

    //
    IloNumExprArg mulSum(Matrix<IloNum> lhs, Matrix<IloNumVar> rhs)
    {
        IloNumExprArg obj = rhs(0, 0) - rhs(0, 0);

        // TODO: Integrate y properly
        for (auto i = 0; i < lhs.rows(); i++) {
            for (auto j = 0; j < rhs.cols(); j++) {
                for (auto r = 0; r < rhs.rows(); r++) {
                    obj = obj + lhs(i, r) * rhs(r, j); // * exp(0)
                }
            }
        }

        return obj;
    }
}