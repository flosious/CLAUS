
#ifndef FIT_FUCTIONS_HPP
#define FIT_FUCTIONS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
// #include <cmath>

#include "tools.hpp"
#include "statistics.hpp"


#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlinear.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_multilarge.h>
#include <gsl/gsl_statistics_double.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_min.h>
#include <gsl/gsl_movstat.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_sort_double.h>
#include <gsl/gsl_filter.h>
#include <gsl/gsl_bspline.h>

using namespace std;

class fit_functions
{
public:
	/// like origin -> peak functions -> asym2sig
	class asym2sig_t
	{
	private:
		double chisq_p=-1;
		bool fitted_p=false;
		static void callback(const size_t iter, void *params, const gsl_multifit_nlinear_workspace *w);
// 		static int function_1st_derivative(const gsl_vector * x, void *data, gsl_matrix * J);
		static int function(const gsl_vector * x, void *data, gsl_vector * f);
	public:
		//fitparameters
		/// offset
		double y0;
		/// anstieg
		double m;
		/// amplitude
		double A;
		/// crater centre
		double xc;
		/// crater width
		double w1;
		double w2,w3;
		/// y(x) values
		vector<double> fitted_y_data(vector<double> x={});
		/// calculation of fit parameters
		bool fit(map<double,double> data_XY);
		bool fitted();
		void print_parameters();
		double chisq();
	};
	
	class polynom_t
	{
	private:
		double chisq_p=-1;
		/// parameters
		bool fitted_p=false;
// 		static void callback(const size_t iter, void *params, const gsl_multifit_nlinear_workspace *w);
// 		static int function_1st_derivative(const gsl_vector * x, void *data, gsl_matrix * J);
// 		static int function(const gsl_vector * x, void *data, gsl_vector * f);
	public:
		double chisq();
		vector<double> fit_parameters;
		bool fit(map<double,double> data_XY, int degree);
		bool fitted();
		vector<double> fitted_y_data(vector<double> x={});
	};

};

#endif // FIT_FUCTIONS
