//#pragma once
//#include "Matrix.h"
//
//namespace RungeKutta
//{
//    using namespace MatrixUtil;
//
//	void parameterize(IloModel& model, const Matrix<IloNumVar>& y, const Matrix<IloNumVar>& u, const std::function<double(double)>& Fc, const Matrix<double>& Fy, const Matrix<double>& Fu, double dt, double t0 = 0)
//	{
//        const size_t n = y.rows();  // dimensions
//        const size_t m = y.cols();  // steps
//
//        for (auto j = 0; j < m - 1; j++) {
//            auto t = j * dt + t0;
//
//            auto _c = Fc(t);
//            auto _y = MatrixUtil::mul(MatrixUtil::eval(Fy, t), y.col(j));
//            auto _u = MatrixUtil::mul(MatrixUtil::eval(Fu, t), u.col(j));
//
//            for (auto i = 0; i < n; i++)
//            {
//                model.add(y(i, j + 1) == y(i, j) + dt * (_c + _y(i, 0) + _u(i, 0)));
//            }
//        }
//	};
//};
//
