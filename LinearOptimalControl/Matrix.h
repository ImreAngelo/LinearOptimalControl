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

    // Matrices A * B
    template <typename T>
    static inline Matrix<IloNumExprArg> mul(Matrix<IloNum> lhs, Matrix<T> rhs)
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

    // Add scalar to matrix
    template <typename T>
    static inline Matrix<IloNumExprArg> add(const Matrix<IloNumVar> lhs, const Matrix<T>& rhs)
    {
        Matrix<IloNumExprArg> r(lhs.rows(), lhs.cols());

        for (auto i = 0; i < lhs.rows(); i++) {
            for (auto j = 0; j < lhs.cols(); j++) {
                r(i, j) = lhs(i, j) + rhs(i, j);
            }
        }

        return r;
    };

    // Multiply scalar into matrix
    template <typename T>
    static inline Matrix<IloNumExprArg> scalarMul(const Matrix<T> lhs, double rhs)
    {
        Matrix<IloNumExprArg> r(lhs.rows(), lhs.cols());

        for (auto i = 0; i < lhs.rows(); i++) {
            for (auto j = 0; j < lhs.cols(); j++) {
                //std::cout << i << ", " << j << ": " << v << " * " << rhs << std::endl;
                r(i, j) = lhs(i, j) * rhs;
            }
        }

        return r;
    };

    // Multiply scalar into matrix
    template <typename T>
    static inline Matrix<IloNumExprArg> scalarAdd(const Matrix<T> lhs, double rhs)
    {
        Matrix<IloNumExprArg> r(lhs.rows(), lhs.cols());

        for (auto i = 0; i < lhs.rows(); i++) {
            for (auto j = 0; j < lhs.cols(); j++) {
                auto v = lhs(i, j);
                r(i, j) = v + rhs;
            }
        }

        return r;
    };

    // Vectors A * B
    static inline IloNumExprArg dot(Eigen::Matrix<IloNum, 1, Eigen::Dynamic> lhs, Eigen::Matrix<IloNumVar, Eigen::Dynamic, 1> rhs)
    {
        assert(lhs.cols() == rhs.rows());

        IloNumExprArg obj = lhs(0, 0) * rhs(0, 0);
        for (auto i = 1; i < lhs.cols(); i++)
            obj = obj + lhs(0, i) * rhs(i, 0);

        return obj;
    }

    // Sum elements of (A * B)
    static inline IloNumExprArg mulSum(const Matrix<IloNum> lhs, const Matrix<IloNumVar> rhs)
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

    // Eval a matrix of functions
    static inline Matrix<IloNum> eval(Matrix<std::function<double(double)>> m, double t)
    {
        Matrix<IloNum> o(m.rows(), m.cols());

        for (int i = 0; i < m.rows(); ++i) {
            for (int j = 0; j < m.cols(); ++j) {
                o(i,j) = m(i, j)(t);
            }
        }

        return o;
    }
}


//typedef Eigen::Matrix<IloNumExprArg, Eigen::Dynamic, Eigen::Dynamic> ArgMatrix;

//// Add scalar to matrix
//ArgMatrix& operator+(const ArgMatrix& lhs, IloNumExprArg rhs)
//{
//    ArgMatrix r;
//
//    for (auto i = 0; i < lhs.rows(); i++) {
//        for (auto j = 0; j < lhs.cols(); j++) {
//            r(i, j) = lhs(i, j) + rhs;
//        }
//    }
//
//    return r;
//};
//
//ArgMatrix& operator+(const ArgMatrix& lhs, double rhs)
//{
//    ArgMatrix r;
//
//    for (auto i = 0; i < lhs.rows(); i++) {
//        for (auto j = 0; j < lhs.cols(); j++) {
//            r(i, j) = lhs(i, j) * rhs;
//        }
//    }
//
//    return r;
//};
//
//ArgMatrix& operator*(const ArgMatrix& lhs, double rhs)
//{
//    ArgMatrix r(lhs.rows(), lhs.cols());
//
//    for (auto i = 0; i < lhs.rows(); i++) {
//        for (auto j = 0; j < lhs.cols(); j++) {
//            r(i, j) = lhs(i, j) * rhs;
//        }
//    }
//
//    return r;
//};