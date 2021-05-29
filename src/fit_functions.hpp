
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
#include <gsl/gsl_minmax.h>
#include <gsl/gsl_fit.h>


using namespace std;

class fit_functions
{
private:
	class parent_t
	{
	protected:
		double chisq_p=-1;
		double chisq0_p=-1;
		double gof_p=-1;
		bool fitted_p=false;
// 		parent_t() {};
	public:
		virtual vector<double> fitted_y_data(vector<double> x);
		/// calculation of fit parameters
		bool fit(map<double, double> data_XY);
		bool fitted();
		double chisq();
		double chisq0();
		double gof();
	};
public:
	class exp_decay_t : public parent_t
	{
	private:
// 		static void callback(const size_t iter, void *params, const gsl_multifit_nlinear_workspace *w);
		static int function(const gsl_vector * x, void *data, gsl_vector * f);
	public:
// 		exp_decay_t() :  parent_t() {};
		double y0;
		double A1;
		double t1;
		vector<double> fitted_y_data(vector<double> x) override;
		/// calculation of fit parameters
		bool fit(map<double, double> data_XY, double y0_s=NAN, double A1_s=NAN, double t1_s=NAN);
		void print_parameters();
		string to_str(string prefix="");
	};
	/// like origin -> peak functions -> asym2sig
	class asym2sig_t
	{
	private:
		double chisq_p=-1;
		double chisq0_p=-1;
		double gof_p=-1;
		bool fitted_p=false;
		static void callback(const size_t iter, void *params, const gsl_multifit_nlinear_workspace *w);
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
		vector<double> fitted_y_data(vector<double> x);
		/// calculation of fit parameters
		bool fit(map<double, double> data_XY, double y0_s=NAN, double xc_s=NAN, double m_s=NAN, double A_s=NAN, double w1_s=NAN, double w2_s=NAN, double w3_s=NAN);
		bool fitted();
		void print_parameters();
		double chisq();
		double chisq0();
		double gof();
		string to_str(string prefix="");
	};
	
	class polynom_t
	{
	private:
		double chisq_p=-1;
		/// parameters
		bool fitted_p=false;
		int degree_p = 0;
	public:
		int degree();
		double chisq();
		vector<double> fit_parameters;
		bool fit(map<double,double> data_XY, int degree_s);
		bool fitted();
		string to_str(string prefix="");
		vector<double> fitted_y_data(vector<double> x={});
	};
	
	///y=m*x
	class linear_t
	{
	private:
		double chisq_p=-1;
		bool fitted_p=false;
	public:
		double cov;
		double slope;
		double chisq();
		bool fit(map<double,double> data_XY);
		bool fitted();
		vector<double> fitted_y_data(vector<double> x={});
	};
};

#endif // FIT_FUCTIONS
