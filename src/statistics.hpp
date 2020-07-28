
#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "tools.hpp"

#include <gsl/gsl_bspline.h>
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

#include "persistence1d.hpp" // from https://www.csc.kth.se/~weinkauf/notes/persistence1d.html

using namespace std;

class statistics {
private:  
public:
	/*moving windows*/
	static vector<vector<double>> get_moving_window_statistics_on_dataX_YY(vector<vector<double>> *data_X_YY, int window_size, int column);
	static vector<vector<double>> get_moving_window_statistics_from_Y(vector<double> *Y, int window_size);
	static vector<double> get_moving_window_median_from_Y(vector<double> Y, int window_size);
	static vector<double> get_moving_window_mean_from_Y(vector<double> Y, int window_size);
	static vector<double> get_moving_window_sd_from_Y(vector<double> Y, int window_size);
	static vector<double> get_moving_window_MAD_from_Y(vector<double> Y, int window_size);
	
	static int get_moving_window_max_index_from_Y(vector<double> Y, int window_size);
	
	static vector<int> get_local_minima_indices_new(vector<double> Y, double Y_tolerance=0, int x_stepsize=1);
	
	/*!
		populates vectors with indices for maxima and minima.
		starting with 0 index as global maximum resp. minimum
		and going on with 1 .... n in decreasing treshold order
	*/
	static bool get_extrema_indices(vector<int>& maxIdx, vector<int>& minIdx, vector<double> Y, double treshold=-1);
    ///\brief returns indices of all local maxima for Y, where Y>treshold
    static vector<int> get_local_maxima_indices(vector<double> Y, double treshold=-1, int stepsize=1);
	/// not tested!
	static vector<int> get_local_maxima_indices_by_triangulation(vector<double> Y, double treshold=0, int stepsize=1);
    /// not tested!
    static vector<int> get_local_minima_indices_by_triangulation(vector<double> Y, double treshold=0, int stepsize=1);
	static vector<int> get_local_minima_indices(vector<double> Y, double treshold=-1, int stepsize=1);
    ///\brief returns vectors {minima,maxima} of all local minima and maxima 
//     static vector<vector<int>> get_local_maxima_minima(vector<double> Y, double treshold);
    static vector<double> get_elements_lower_than_threshold(vector<double> Y, double treshold);
    static vector<int> get_elements_lower_than_threshold(vector<int> Y, int treshold);
    static vector<double> get_elements_higher_than_threshold(vector<double> Y, double treshold);
    static vector<int> get_elements_higher_than_threshold(vector<int> Y, int treshold);
    static vector<double> normalize_to_max(vector<double> Y);
    static vector<double> normalize_to_median(vector<double> Y);
    static vector<double> normalize_to_mean(vector<double> Y);
	/// percentile value is between 1 and 0 
	static double get_quantile_from_Y(vector<double> Y,double percentile=0.75);
    static double get_median_from_Y(vector<double> Y);
    static double get_mad_from_Y(vector<double> Y);
	static double get_sd_from_Y(vector<double> Y);
    static double get_mean_from_Y(vector<double> Y);
	
	/*!
	 * The trimmed mean, or truncated mean, discards a certain number of smallest and largest samples from the input vector
	 * before computing the mean of the remaining samples. The amount of trimming is specified by a factor alpha [0,0.5]. 
	 * Then the number of samples discarded from both ends of the input vector is floor(\alpha * n), 
	 * where n is the length of the input. So to discard 25% of the samples from each end, one would set alpha = 0.25.
	 */
	static double get_trimmed_mean_from_Y(vector<double> Y,float alpha=0.25);
	/*!
	 * something like "get_trimmed_mean_from_Y" 
	 * https://www.gnu.org/software/gsl/doc/html/statistics.html#robust-location-estimates
	 */
	static double get_gastwirth_estimator_from_Y(vector<double> Y);
    static int get_next_max_index(vector<double> Y, int skip_points=0);
    static int get_next_min_index(vector<double> Y, int skip_points=0);
    static double get_next_min(vector<double> Y, int skip_points=0);
    static double get_next_max(vector<double> Y, int skip_points=0);
    static vector<double> recursive_median_filter(vector<double> Y, int window_size);
    static vector<double> gaussian_filter(vector<double> Y, int window_size, double alpha);
    static vector<vector<double>> get_local_minima_XY(vector<double> X, vector<double> Y);
    static vector<double> impulse_filter(vector<double> Y, int window_size, float scale_factor);
    static vector<double> gsl_vec_to_std_vec(gsl_vector ** vec);
    static vector<double> fit_poly_XY_transposed(vector<vector<double>> XY, int poly_grad);
    static vector<double> get_poly_Y(vector<double> parameters, vector<double> X);
    static vector<double> derive_vec(vector<double> X, vector<double> Y);
    static int get_max_index_from_Y(vector<double> Y);
    static int get_min_index_from_Y(vector<double> Y);
    
//     static statis get_statistics(vector<double> Y);
    static bool std_mat_to_gsl_vec(vector<vector<double>> *std_mat, gsl_vector ** vec, int col);
    static bool std_vec_to_gsl_vec(vector<double> *std_vec, gsl_vector ** vec);
    ///\brief returns {X_min,X_max,Y_min,Y_max}
    static vector<double> get_minmax_from_dataXY_tensor(vector<vector<vector<double>>> dataXY_tensor);
    static int get_index_from_upper_limit_in_vec(vector<double> vec, double upper_limit);
    static vector<double> interpolate_dataXY_to_X(vector<vector<double>> data_XY, vector<double> X);
    static vector<vector<double>> truncate_mat_by_limit_in_col(vector<vector<double>> *data_XY, double limit, int col);
    
    static int get_index_for_next_value_lower_than_treshold(vector<double> Y,double treshold, int start=0, int ende=0);
	static int get_index_for_next_value_higher_than_treshold(vector<double> Y,double treshold, int start=0, int ende=0);
    static int get_index_for_next_value_within_treshold(vector<double> Y,double treshold_bottom, double treshold_top, int start=0, int ende=0);
    
    static double get_correlation_from_dataXY(vector<vector<double>> *data_XY1, vector<vector<double>> *data_XY2);
//     static statis get_statistics_from_dataXY_transposed(vector<vector<double>> *data_XY_transposed);
    static double polynom_integrator (double x, void * params);
    static double get_area_under_dataXY_transposed(vector<vector<double>> data_XY_transposed,double lower_X, double upper_X);
    static double get_area_under_dataXY(vector<vector<double>> data_XY,double lower_X, double upper_X);
    static double get_area_under_polynom(vector<double> polynom_parameters, double lower_X, double upper_X);
    static vector<int> get_positions_of_zero_values(vector<vector<double>> matrix, double treshold);
    static vector<double> derive_poly(vector<double> parameters);
    static vector<double> integrate_poly(vector<double> parameters);
    static bool first_derivative_poly(vector<double> *parameters, vector<vector<double>> *data_XY_dY);
    static bool second_derivative_poly(vector<double> *parameters, vector<vector<double>> *data_XY_ddY);
    static vector<double> get_polynom_Y(vector<double> parameters, vector<double> *data_X);
    static double fit_poly (vector<vector<double>> *data_matrix, vector<double> *fit_parameters);
//     static bool polynomfit_XY(vector<vector<double>> data_XY,data *fitted_data, int polynom_grad=1, double interp_res=0);
	static vector<double> interpolate_data_XY_transposed(vector<vector<double>> data_XY, vector<double> X);
	
	
	/****************/
	static vector<double> interpolate_bspline(map<double, double>& data_XY,vector<double> X_new={}, int bspline_degree=3);
	static vector<double> interpolate_data_XY(map<double,double> *data_XY, vector<double> *X);
	static double integrate(map<double,double> *data_XY, double lower_limit_X=0, double upper_limit_X=0) ;
	
};

#endif // STATISTICS_HPP

