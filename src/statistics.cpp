/*
	Copyright (C) 2020 Florian Bärwolf
	floribaer@gmx.de

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "statistics.hpp"

vector<int> statistics::chunk_data_into_equal_bins_relative(const vector<double>& data1, const vector<double>& data2, const int bins_count)
{
	if (bins_count==0 || data1.size() == 0 || data2.size()==0 || data1.size()!=data2.size())
		return {};
	
	vector<double> distances(data1.size());
	
	for (int i=0;i<data1.size();i++)
		distances.at(i)=sqrt(pow(data1.at(i),2)+pow(data2.at(i),2));
	
	return chunk_data_into_equal_bins_relative(distances,bins_count);
}

vector<int> statistics::chunk_data_into_equal_bins_relative(const vector<double>& data, const int bins_count)
{
	if (data.size()==0 || bins_count == 0 || bins_count > data.size())
		return {};
	auto MIN = data.at(get_min_index_from_Y(data));
	auto MAX = data.at(get_max_index_from_Y(data))*1.01; // make it a little bit bigger, so every data fits in
	vector<double> bins(bins_count);
	double bins_size = (MAX-MIN)/bins_count;
	for (int i=0;i<bins_count;i++)
	{
		bins.at(i*bins_size+MIN);
	}
	return chunk_data_into_bins(data,bins);
}

vector<int> statistics::chunk_data_into_bins(const vector<double>& data1, const vector<double>& data2, vector<double> bins)
{
	if (bins.size() == 0 || data1.size() == 0 || data2.size()==0 || data1.size()!=data2.size())
		return {};
	
	vector<double> distances(data1.size());
	
	for (int i=0;i<data1.size();i++)
		distances.at(i)=sqrt(pow(data1.at(i),2)+pow(data2.at(i),2));
	
	return chunk_data_into_bins(distances,bins);
}

vector<int> statistics::chunk_data_into_bins(const vector<double>& data, vector<double> bins)
{
	vector<int> count_number_in_bins(0,bins.size()-1);
	for (auto d:data)
	{
		for (int i=0;i<bins.size()-1;i++)
		{
			if (bins[i] <= d && d < bins[i+1])
				count_number_in_bins.at(i)++;
		}
	}
	return count_number_in_bins;
}

bool statistics::get_extrema_indices(vector<int>& maxIdx, vector<int>& minIdx, vector<double> Y, double treshold)
{
	vector<int> local_extrema;
	
// 	vector<float> data(Y.size());
// 	for (int i=0;i<Y.size();i++)
// 		Y[i]=Y[i];
	
	//Run persistence on data - this is the main call.
    p1d::Persistence1D p;
    p.RunPersistence(Y);
	
	//Get all extrema with a persistence larger than 10.
    vector< p1d::TPairedExtrema > Extrema;
    if (treshold < 0) treshold =  get_mad_from_Y(Y);
	if (!p.GetPairedExtrema(Extrema, treshold)) return false;
	
	maxIdx.clear();
	minIdx.clear();
	maxIdx.resize(Extrema.size()+1);
	minIdx.resize(Extrema.size()+1);
	
	maxIdx[0]=get_max_index_from_Y(Y);
	minIdx[0]=p.GetGlobalMinimumIndex();
	
	int i=0;
	int s = minIdx.size();
	for(vector< p1d::TPairedExtrema >::iterator it = Extrema.begin(); it != Extrema.end(); it++)
	{
		i++;
		minIdx[s-i]=(*it).MinIndex;
		maxIdx[s-i]=(*it).MaxIndex;
	}
	
	 //Print all found pairs - pairs are sorted ascending wrt. persistence.
//     for(vector< p1d::TPairedExtrema >::iterator it = Extrema.begin(); it != Extrema.end(); it++)
//     {
//         cout << "Persistence: " << (*it).Persistence
// 			 << endl
//              << " minimum index: " << (*it).MinIndex << "\tvalue = " << Y[(*it).MinIndex]
//              << endl 
//              << " maximum index: " << (*it).MaxIndex << "\tvalue = " << Y[(*it).MaxIndex]
//              << std::endl;
//     }
	
    //Also, print the global minimum.
//     cout << "Global minimum index: " << p.GetGlobalMinimumIndex()
//          << " Global minimum value: " << p.GetGlobalMinimumValue() << endl;
		 
	return true;
}

int statistics::get_index_for_next_value_within_treshold(vector<double> Y, double treshold_bottom, double treshold_top, int start, int ende)
{
	if (ende==0) ende=Y.size();
	int index=0;

	for (int i=start;i<ende;i++)
	{
		index=i;
		if (Y[i]<treshold_top && Y[i]>=treshold_bottom) break;
	}
	return index;
}


int statistics::get_index_for_next_value_lower_than_treshold(vector<double> Y,double treshold, int start, int ende)
{
	if (ende==0) ende=Y.size();
	int index=0;
	for (int i=start;i<ende;i++)
	{
		index=i;
		if (Y[i]<treshold) break;
	}
	return index;
}

int statistics::get_index_for_next_value_higher_than_treshold(vector<double> Y,double treshold, int start, int ende)
{
	if (ende==0) ende=Y.size();
	int index=0;
	for (int i=start;i<ende;i++)
	{
		index=i;
		if (Y[i]>treshold) break;
	}
	return index;
}

vector<int> statistics::get_local_maxima_indices_by_triangulation(vector<double> Y, double treshold, int stepsize)
{
	if (Y.size()<3*stepsize) return {};
	vector<int> local_max_indices;
    vector<double> Y_max;
	for (int x=2*stepsize;x<Y.size();x=x+stepsize)
	{
		if (Y[x-stepsize]>Y[x-2*stepsize]+treshold && Y[x-stepsize]>Y[x]+treshold) local_max_indices.push_back(x-stepsize);
	}
	return local_max_indices;
}

vector<int> statistics::get_local_minima_indices_by_triangulation(vector<double> Y, double treshold, int stepsize)
{
	if (Y.size()<3*stepsize) return {};
	vector<int> local_min_indices;
    vector<double> Y_max;
	for (int x=3*stepsize;x<Y.size();x=x+stepsize)
	{
		if (Y[x-stepsize]<Y[x-2*stepsize]-treshold && Y[x-stepsize]<Y[x]-treshold) local_min_indices.push_back(x-stepsize);
	}
	return local_min_indices;
}

vector<int> statistics::get_local_maxima_indices(vector<double> Y, double treshold, int stepsize) {
    vector<int> local_max_indices;
    vector<double> Y_max;
    int Y_max_start_i;
    
    if (treshold==-1) treshold=statistics::get_median_from_Y(Y)+1.5*statistics::get_mad_from_Y(Y);

    for (int i=0;i<Y.size();i+=stepsize) { 
        if (Y[i]>=treshold ) { /*record entries*/
            if (Y_max.size()==0) Y_max_start_i=i;
            Y_max.push_back(Y[i]);
        } else { // outside of treshold
            if (Y_max.size()>0) { // record and reset
                int local_max_index = statistics::get_max_index_from_Y(Y_max) + Y_max_start_i;
                local_max_indices.push_back(local_max_index);
                Y_max.clear();
            } 
        }
    }
    return local_max_indices;
}

vector<int> statistics::get_local_minima_indices_new(vector<double> Y, double Y_tolerance, int x_stepsize)
{
	vector<int> local_minima_indices;
	if (Y.size()==0) return local_minima_indices;
	
// 	bool last_max=false;
	bool last_min=false;
	int last_min_pos=0;
	for (int x=x_stepsize;x<Y.size();x=x+x_stepsize)
	{
		if (Y[x]<Y[x-x_stepsize]) 
		{
			last_min=false;
		}
		else if (Y[x]>+Y_tolerance+Y[x-x_stepsize] && !last_min) 
		{
			last_min=true;
			local_minima_indices.push_back(x-x_stepsize/2);
		}
		
	}
	return local_minima_indices;
}

vector<int> statistics::get_local_minima_indices(vector<double> Y, double treshold, int stepsize) {
    vector<int> local_min_indices;
    vector<double> Y_min;
    int Y_min_start_i;
    
    if (treshold==-1) treshold=statistics::get_median_from_Y(Y)-1.5*statistics::get_mad_from_Y(Y);
    
    for (int i=0;i<Y.size();i+=stepsize) { 
        if (Y[i]<=treshold ) { /*record entries*/
            if (Y_min.size()==0) Y_min_start_i=i;
            Y_min.push_back(Y[i]);
        } else { // outside of treshold
            if (Y_min.size()>0) { // record and reset
                int local_min_index = statistics::get_min_index_from_Y(Y_min) + Y_min_start_i;
                local_min_indices.push_back(local_min_index);
                Y_min.clear();
            } 
        }
    }
    return local_min_indices;
}

vector<double> statistics::get_elements_higher_than_threshold(vector<double> Y, double treshold) {
    vector<double> Y_res;
    for (int i=0;i<Y.size();i++) { 
        if (Y[i]>treshold) Y_res.push_back(Y[i]);
    }
    return Y_res;
}

vector<int> statistics::get_elements_higher_than_threshold(vector<int> Y, int treshold) {
    vector<int> Y_res;
    for (int i=0;i<Y.size();i++) { 
        if (Y[i]>treshold) Y_res.push_back(Y[i]);
    }
    return Y_res;
}


vector<double> statistics::get_elements_lower_than_threshold(vector<double> Y, double treshold) {
    vector<double> Y_res;
    for (int i=0;i<Y.size();i++) {
        if (Y[i]<treshold) Y_res.push_back(Y[i]);
    }
    return Y_res;
}
vector<int> statistics::get_elements_lower_than_threshold(vector<int> Y, int treshold) {
    vector<int> Y_res;
    for (int i=0;i<Y.size();i++) {
        if (Y[i]<treshold) Y_res.push_back(Y[i]);
    }
    return Y_res;
}

vector<double> statistics::normalize_to_max(vector<double> Y) {
    double max_index = get_max_index_from_Y(Y);
    for (int i=0;i<Y.size();i++) Y[i]=Y[i]/Y[max_index];
    return Y;
}

vector<double> statistics::normalize_to_median(vector<double> Y) {
    double median = get_median_from_Y(Y);
    for (int i=0;i<Y.size();i++) Y[i]=Y[i]/median;
    return Y;
}

vector<double> statistics::normalize_to_mean(vector<double> Y) {
    double mean = get_mean_from_Y(Y);
    for (int i=0;i<Y.size();i++) Y[i]=Y[i]/mean;
    return Y;
}

vector<double> statistics::recursive_median_filter(vector<double> Y, int window_size) {
    vector<double> Y_filtered;
    
    const size_t N = Y.size();                        /* length of time series */
    const size_t K = window_size;                         /* window size */
    gsl_vector *x = gsl_vector_alloc(N);         /* input vector */
    gsl_vector *y = gsl_vector_alloc(N);        /* filtered output vector for alpha1 */
    gsl_filter_rmedian_workspace  *w = gsl_filter_rmedian_alloc(K);

//     statistics::std_vec_to_gsl_vec(&Y,&x);
    for (size_t i=0;i<Y.size();i++)gsl_vector_set (x, i, Y.at(i));
    /* apply filters */
    gsl_filter_rmedian(GSL_FILTER_END_TRUNCATE, x, y, w);
//     Y_filtered = statistics::gsl_vec_to_std_vec(&y);
    for (int i=0;i<y->size;i++) Y_filtered.push_back(y->data[i]);
    
    gsl_vector_free(x);
    gsl_vector_free(y);
    gsl_filter_rmedian_free(w);

    return Y_filtered;
}

vector<double> statistics::impulse_filter(vector<double> Y, int window_size, float scale_factor) {
    vector<double> Y_filtered;                                   /* result vector */
    const size_t N = Y.size();                               /* length of time series */
    const size_t K = window_size;                                 /* window size */
    const double t = scale_factor;                                /* number of scale factors for outlier detection */
    gsl_vector *x = gsl_vector_alloc(N);                 /* input vector */
    gsl_vector *y = gsl_vector_alloc(N);                 /* output (filtered) vector */
    gsl_vector *xmedian = gsl_vector_alloc(N);           /* window medians */
    gsl_vector *xsigma = gsl_vector_alloc(N);            /* window scale estimates */
    gsl_vector_int *ioutlier = gsl_vector_int_alloc(N);  /* outlier detected? */
    gsl_filter_impulse_workspace * w = gsl_filter_impulse_alloc(K);
    gsl_rng *r = gsl_rng_alloc(gsl_rng_default);
    size_t noutlier;
//     statistics::std_vec_to_gsl_vec(&Y,&x);
    for (size_t i=0;i<Y.size();i++) gsl_vector_set (x, i, Y.at(i));
    /* apply impulse detection filter */
	/// THIS IS NOT WORKING IN WINDOWS 32BIT FOR SOME INPUT VALUES
//     gsl_filter_impulse(GSL_FILTER_END_TRUNCATE, GSL_FILTER_SCALE_QN, t, x, y,
//                      xmedian, xsigma, &noutlier, ioutlier, w);
	gsl_filter_impulse(GSL_FILTER_END_PADVALUE, GSL_FILTER_SCALE_MAD, t, x, y, xmedian, xsigma, &noutlier, ioutlier, w);
// 	gsl_filter_impulse(GSL_FILTER_END_TRUNCATE, GSL_FILTER_SCALE_QN, t, x, y, xmedian, xsigma, &noutlier, ioutlier, w);
    for (int i=0;i<y->size;i++) 
		Y_filtered.push_back(y->data[i]);
    gsl_vector_free(x);
    gsl_vector_free(y);
    gsl_vector_free(xmedian);
    gsl_vector_free(xsigma);
    gsl_vector_int_free(ioutlier);
    gsl_filter_impulse_free(w);
    gsl_rng_free(r);
    return Y_filtered;
}

vector<double> statistics::gaussian_filter(vector<double> Y, int window_size, double alpha) {
    vector<double> Y_filtered;
    
    const size_t N = Y.size();                        /* length of time series */
    const size_t K = window_size;                         /* window size */
    gsl_vector *x = gsl_vector_alloc(N);         /* input vector */
    gsl_vector *y = gsl_vector_alloc(N);        /* filtered output vector for alpha1 */
    gsl_vector *k1 = gsl_vector_alloc(K);        /* Gaussian kernel for alpha1 */
    gsl_rng *r = gsl_rng_alloc(gsl_rng_default);
    gsl_filter_gaussian_workspace *gauss_p = gsl_filter_gaussian_alloc(K);
    /* compute kernels without normalization */
    gsl_filter_gaussian_kernel(alpha, 0, 0, k1);

//     statistics::std_vec_to_gsl_vec(&Y,&x);
    for (size_t i=0;i<Y.size();i++)gsl_vector_set (x, i, Y.at(i));
    /* apply filters */
    gsl_filter_gaussian(GSL_FILTER_END_PADVALUE, alpha, 0, x, y, gauss_p);
//     Y_filtered = statistics::gsl_vec_to_std_vec(&y1);
    for (int i=0;i<y->size;i++) Y_filtered.push_back(y->data[i]);
    
    gsl_vector_free(x);
    gsl_vector_free(y);
    gsl_vector_free(k1);
    gsl_rng_free(r);
    gsl_filter_gaussian_free(gauss_p);

    return Y_filtered;
}

int statistics::get_next_min_index(vector<double> Y, int skip_points) {
    int index=0;
    double min=Y[index];
    for (int i=skip_points;i<Y.size();i=i+1+skip_points) {
        index = i;
        if (Y[i] > min) break;
        min = Y[i];
    }
    return index;
}

int statistics::get_next_max_index(vector<double> Y, int skip_points) {
    int index=0;
    double max=Y[index];
    for (int i=skip_points;i<Y.size();i=i+1+skip_points) {
        index = i;
        if (Y[i] < max) break;
        max = Y[i];
    }
    return index;
}

double statistics::get_next_min(vector<double> Y, int skip_points) {
    double min=Y[0];
    for (int i=1;i<Y.size();i=i+1+skip_points) {
        if (Y[i] < min) min=Y[i];
        else break;
    }
    return min;
}

double statistics::get_next_max(vector<double> Y,int skip_points) {
    double max=Y[0];
    for (int i=1;i<Y.size();i=i+1+skip_points) {
        if (Y[i] > max) max=Y[i];
        else break;
    }
    return max;
}

/*
vector<vector<double>> statistics::get_local_minima_XY(vector<double> X, vector<double> Y) {
    
    vector<vector<double>> minima;
	return minima;
}
*/

bool statistics::std_mat_to_gsl_vec(vector<vector<double>> *std_mat, gsl_vector ** vec, int col) {
    *vec = gsl_vector_alloc(std_mat->size());
    for (size_t i=0;i<std_mat->size();i++) {
        if (col>=std_mat->at(i).size()) {
            return false;
        }
        gsl_vector_set (*vec, i, std_mat->at(i)[col]);
    }
    return true;
}

bool statistics::std_vec_to_gsl_vec(vector<double> *std_vec, gsl_vector ** vec) {
    *vec = gsl_vector_alloc(std_vec->size());
    for (size_t i=0;i<std_vec->size();i++)gsl_vector_set (*vec, i, std_vec->at(i));
    return true;
}

vector<double> statistics::gsl_vec_to_std_vec(gsl_vector ** vec) {
    vector<double> Y;
    for (int i=0;i<(*vec)->size;i++) Y.push_back((*vec)->data[i]);
//     gsl_vector_free(vec);
    return Y;
}


vector<double> statistics::interpolate_dataXY_to_X(vector<vector<double>> data_XY, vector<double> X) {
    vector<double> Y;
    int N = data_XY.size();
    gsl_interp_accel *acc = gsl_interp_accel_alloc ();
    const gsl_interp_type *t = gsl_interp_cspline_periodic;
    gsl_spline *spline = gsl_spline_alloc (t, N);
    gsl_vector *x, *y;
    statistics::std_mat_to_gsl_vec(&data_XY,&x,0);
    statistics::std_mat_to_gsl_vec(&data_XY,&y,1);
    gsl_spline_init (spline, x->data, y->data, N);
    double *Xi= &X[0];
    for (int i=0;i<X.size();i++) {
        Y.push_back(gsl_spline_eval (spline, Xi[i], acc));
    }
    gsl_spline_free (spline);
    gsl_interp_accel_free (acc);
    gsl_vector_free(x);
    gsl_vector_free(y);
    return Y;
}

/*
 * 1st derivative (numerical)
 */
vector<double> statistics::derive_vec(vector<double> X, vector<double> Y) {
    vector<double> d_Y(Y.size());
    for (int i=1;i<X.size();i++) {
        d_Y[i]=(Y[i]-Y[i-1])/(X[i]-X[i-1]);
    }
    return d_Y;
}

int statistics::get_index_from_upper_limit_in_vec(vector<double> vec, double upper_limit) {
    int erg=0;
    for (int i=0;i<vec.size();i++) {
        if (vec[i]>upper_limit) {
            erg=i;
            break;
        }
    }
    return erg;
}

vector<vector<double>> statistics::truncate_mat_by_limit_in_col(vector<vector<double>> *data_XY, double limit, int col) {
    vector<vector<double>> result_XY;
    //1st sort matrix by col?
    for (int i=0;i<data_XY->size();i++) {
        if (data_XY->at(i)[col] > limit) result_XY.push_back(data_XY->at(i));
    }
    cout << "limit = " << limit << "\tresult_XY.size()" << result_XY.size() << endl;
    return result_XY;
}

vector<vector<double>> statistics::get_moving_window_statistics_on_dataX_YY(vector<vector<double>> *data_X_YY, int window_size, int column) {
    vector<vector<double>> mat_results;
    if (window_size<2) window_size=10;
    if (column>=data_X_YY->at(0).size()) return mat_results;
    
    int N=data_X_YY->size();
    gsl_vector *x = gsl_vector_alloc(N);
    statistics::std_mat_to_gsl_vec(data_X_YY,&x,column);
    gsl_vector *xmedian = gsl_vector_alloc(N);
    gsl_vector *xmad = gsl_vector_alloc(N);
    gsl_vector *xiqr = gsl_vector_alloc(N);
    gsl_vector *xSn = gsl_vector_alloc(N);
    gsl_vector *xQn = gsl_vector_alloc(N);
    gsl_vector *xsd = gsl_vector_alloc(N);
    gsl_vector *xsd2 = gsl_vector_alloc(N);
    gsl_vector *xsd3 = gsl_vector_alloc(N);
    gsl_vector *xmean = gsl_vector_alloc(N);
    gsl_vector *xmin = gsl_vector_alloc(N);
    gsl_vector *xmax = gsl_vector_alloc(N);
    
    gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
//     gsl_rng *r = gsl_rng_alloc(gsl_rng_default);
    gsl_movstat_mad(GSL_MOVSTAT_END_TRUNCATE, x, xmedian, xmad, w);
    gsl_movstat_qqr(GSL_MOVSTAT_END_TRUNCATE, x, 0.25, xiqr, w);
    gsl_movstat_Sn(GSL_MOVSTAT_END_TRUNCATE, x, xSn, w);
    gsl_movstat_Qn(GSL_MOVSTAT_END_TRUNCATE, x, xQn, w);  
    gsl_movstat_sd(GSL_MOVSTAT_END_TRUNCATE, x, xsd, w);
    gsl_movstat_sd(GSL_MOVSTAT_END_TRUNCATE, xsd, xsd2, w);
    gsl_movstat_sd(GSL_MOVSTAT_END_TRUNCATE, xsd2, xsd3, w);
    gsl_movstat_mean(GSL_MOVSTAT_END_PADVALUE, x, xmean, w);
    gsl_movstat_minmax(GSL_MOVSTAT_END_PADVALUE, x, xmin, xmax, w);
    for (int j=0;j<N;j++) {
//         data_XY->at(j)={xmedian->data[j],xmad->data[j],xiqr->data[j],xSn->data[j]};
        mat_results.at(j).push_back(xmedian->data[j]); //3
        mat_results.at(j).push_back(xmad->data[j]);    //4
        mat_results.at(j).push_back(xiqr->data[j]);    //5
        mat_results.at(j).push_back(xSn->data[j]);     //6 bk
        mat_results.at(j).push_back(xQn->data[j]);     //7 b
        mat_results.at(j).push_back(xsd->data[j]);     //8 c
        mat_results.at(j).push_back(xmean->data[j]);   //9
        mat_results.at(j).push_back(xmax->data[j]);    //10
        mat_results.at(j).push_back(xmin->data[j]);    //11
        mat_results.at(j).push_back(xsd2->data[j]);     //12 
        mat_results.at(j).push_back(xsd3->data[j]);     //13 
    }
    gsl_vector_free(x);
    gsl_vector_free(xmedian);
    gsl_vector_free(xmad);
    gsl_vector_free(xiqr);
    gsl_vector_free(xSn);
    gsl_vector_free(xQn);
    gsl_vector_free(xsd);
    gsl_vector_free(xsd3);
    gsl_vector_free(xsd2);
    gsl_vector_free(xmean);
    gsl_vector_free(xmin);
    gsl_vector_free(xmax);
//     gsl_rng_free(r);
    gsl_movstat_free(w);
    return mat_results;
}

vector<vector<double>> statistics::get_moving_window_statistics_from_Y(vector<double> *Y, int window_size) {
    
    vector<vector<double>> mat_results;
    if (window_size<2) window_size=10;
    mat_results=tools::mat::transpose_matrix_double({*Y});
    int N=Y->size();
    gsl_vector *x = gsl_vector_alloc(N);
//     statistics::std_vec_to_gsl_vec(Y,&x);

    for (size_t i=0;i<Y->size();i++) gsl_vector_set (x, i, Y->at(i));
    gsl_vector *xmedian = gsl_vector_alloc(N);
    gsl_vector *xmad = gsl_vector_alloc(N);
    gsl_vector *xiqr = gsl_vector_alloc(N);
    gsl_vector *xSn = gsl_vector_alloc(N);
    gsl_vector *xQn = gsl_vector_alloc(N);
    gsl_vector *xsd = gsl_vector_alloc(N);
    gsl_vector *xsd2 = gsl_vector_alloc(N);
    gsl_vector *xsd3 = gsl_vector_alloc(N);
    gsl_vector *xmean = gsl_vector_alloc(N);
    gsl_vector *xmin = gsl_vector_alloc(N);
    gsl_vector *xmax = gsl_vector_alloc(N);
    
    gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
//     gsl_rng *r = gsl_rng_alloc(gsl_rng_default);
    gsl_movstat_mad(GSL_MOVSTAT_END_TRUNCATE, x, xmedian, xmad, w);
    gsl_movstat_qqr(GSL_MOVSTAT_END_TRUNCATE, x, 0.25, xiqr, w);
    gsl_movstat_Sn(GSL_MOVSTAT_END_TRUNCATE, x, xSn, w);
    gsl_movstat_Qn(GSL_MOVSTAT_END_TRUNCATE, x, xQn, w);  
    gsl_movstat_sd(GSL_MOVSTAT_END_TRUNCATE, x, xsd, w);
    gsl_movstat_sd(GSL_MOVSTAT_END_TRUNCATE, xsd, xsd2, w);
    gsl_movstat_sd(GSL_MOVSTAT_END_TRUNCATE, xsd2, xsd3, w);
    gsl_movstat_mean(GSL_MOVSTAT_END_PADVALUE, x, xmean, w);
    gsl_movstat_minmax(GSL_MOVSTAT_END_PADVALUE, x, xmin, xmax, w);
    for (int j=0;j<N;j++) {
//         data_XY->at(j)={xmedian->data[j],xmad->data[j],xiqr->data[j],xSn->data[j]};
        mat_results.at(j).push_back(xmedian->data[j]); //1
        mat_results.at(j).push_back(xmad->data[j]);    //2
        mat_results.at(j).push_back(xiqr->data[j]);    //3
        mat_results.at(j).push_back(xSn->data[j]);     //4 bk
        mat_results.at(j).push_back(xQn->data[j]);     //5 b
        mat_results.at(j).push_back(xsd->data[j]);     //6 c
        mat_results.at(j).push_back(xmean->data[j]);   //7
        mat_results.at(j).push_back(xmax->data[j]);    //8
        mat_results.at(j).push_back(xmin->data[j]);    //9
        mat_results.at(j).push_back(xsd2->data[j]);     //10 
        mat_results.at(j).push_back(xsd3->data[j]);     //11 
    }
    gsl_vector_free(x);
    gsl_vector_free(xmedian);
    gsl_vector_free(xmad);
    gsl_vector_free(xiqr);
    gsl_vector_free(xSn);
    gsl_vector_free(xQn);
    gsl_vector_free(xsd);
    gsl_vector_free(xsd3);
    gsl_vector_free(xsd2);
    gsl_vector_free(xmean);
    gsl_vector_free(xmin);
    gsl_vector_free(xmax);
//     gsl_rng_free(r);
    gsl_movstat_free(w);
    return mat_results;
}

vector<double> statistics::get_moving_window_mean_from_Y(vector<double> Y, int window_size) {
    if (window_size<2) window_size=10;
    vector<double> sd;
    int N=Y.size();
    gsl_vector *x = gsl_vector_alloc(N);
    for (size_t i=0;i<Y.size();i++) gsl_vector_set (x, i, Y.at(i));
    gsl_vector *xsd = gsl_vector_alloc(N);
    gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
    gsl_movstat_mean(GSL_MOVSTAT_END_TRUNCATE, x, xsd, w);
    for (int j=0;j<N;j++) {
        sd.push_back(xsd->data[j]);     //8 c
    }
    gsl_vector_free(x);
    gsl_vector_free(xsd);
    gsl_movstat_free(w);
    return sd;
}

vector<double> statistics::get_moving_window_median_from_Y(vector<double> Y, int window_size) {
    if (window_size<2) window_size=10;
    vector<double> sd;
    int N=Y.size();
    gsl_vector *x = gsl_vector_alloc(N);
    for (size_t i=0;i<Y.size();i++) gsl_vector_set (x, i, Y.at(i));
    gsl_vector *xsd = gsl_vector_alloc(N);
    gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
    gsl_movstat_median(GSL_MOVSTAT_END_TRUNCATE, x, xsd, w);
    for (int j=0;j<N;j++) {
        sd.push_back(xsd->data[j]);     //8 c
    }
    gsl_vector_free(x);
    gsl_vector_free(xsd);
    gsl_movstat_free(w);
    return sd;
}

vector<double> statistics::get_moving_window_MAD_from_Y(vector<double> Y, int window_size) {
    if (window_size<2) window_size=10;
    vector<double> sd;
    int N=Y.size();
    gsl_vector *x = gsl_vector_alloc(N);
    for (size_t i=0;i<Y.size();i++) gsl_vector_set (x, i, Y.at(i));
    gsl_vector *xmad = gsl_vector_alloc(N);
	gsl_vector *xmedian = gsl_vector_alloc(N);
    gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
    gsl_movstat_mad(GSL_MOVSTAT_END_TRUNCATE, x, xmedian, xmad, w);
    for (int j=0;j<N;j++) {
        sd.push_back(xmad->data[j]);     //8 c
    }
    gsl_vector_free(x);
    gsl_vector_free(xmad);
	gsl_vector_free(xmedian);
    gsl_movstat_free(w);
    return sd;
}

vector<double> statistics::get_moving_window_sd_from_Y(vector<double> Y, int window_size) {
    if (window_size<2) window_size=10;
    vector<double> sd;
    int N=Y.size();
    gsl_vector *x = gsl_vector_alloc(N);
//     statistics::std_vec_to_gsl_vec(&Y,&x);
    for (size_t i=0;i<Y.size();i++)gsl_vector_set (x, i, Y.at(i));
    gsl_vector *xsd = gsl_vector_alloc(N);
    gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
    gsl_movstat_sd(GSL_MOVSTAT_END_TRUNCATE, x, xsd, w);
    for (int j=0;j<N;j++) {
        sd.push_back(xsd->data[j]);     //8 c
    }
    gsl_vector_free(x);
    gsl_vector_free(xsd);
    gsl_movstat_free(w);
    return sd;
}

int statistics::get_moving_window_max_index_from_Y(vector<double> Y, int window_size) {
    int max_pos;
    int N=Y.size();
    gsl_vector *x = gsl_vector_alloc(N);
//     statistics::std_vec_to_gsl_vec(&Y,&x);
    for (size_t i=0;i<Y.size();i++)gsl_vector_set (x, i, Y.at(i));
    gsl_movstat_workspace * w = gsl_movstat_alloc(window_size);
    max_pos = gsl_stats_max_index(x->data,1,Y.size());
    gsl_vector_free(x);
    gsl_movstat_free(w);
    return max_pos;
}

double statistics::get_correlation_from_dataXY(vector<vector<double>> *data_XY1, vector<vector<double>> *data_XY2) {
    gsl_vector *vec1, *vec2;
    statistics::std_mat_to_gsl_vec(data_XY1,&vec1,1);
    statistics::std_mat_to_gsl_vec(data_XY2,&vec2,1);    
    double corr = gsl_stats_correlation(vec1->data,1,vec2->data,1,data_XY2->size());
    gsl_vector_free(vec1);
    gsl_vector_free(vec2);
    return corr;
}

vector<double> statistics::get_minmax_from_dataXY_tensor(vector<vector<vector<double>>> dataXY_tensor) {
    vector<double> max_values_of_all_colls;
    vector<vector<double>> mat, mat_transposed;
    mat=dataXY_tensor.at(0);
    for (int i=1;i<dataXY_tensor.size();i++) {
        mat.insert(mat.end(),dataXY_tensor.at(i).begin(),dataXY_tensor.at(i).end());
    }
//     mat_transposed=Tools::transpose_matrix_double(mat);
    gsl_vector * vec;
    for (int i=0;i<2;i++) { // X + Y column
        statistics::std_mat_to_gsl_vec(&mat,&vec,i);
        double max = gsl_stats_max(vec->data,1,mat.size());
        double min = gsl_stats_min(vec->data,1,mat.size());
        max_values_of_all_colls.push_back(min);
        max_values_of_all_colls.push_back(max);
    }
    gsl_vector_free(vec);
    return max_values_of_all_colls;
}

int statistics::get_max_index_from_Y(vector<double> Y) {
    gsl_vector * vec = gsl_vector_alloc(Y.size());
//     statistics::std_vec_to_gsl_vec(&Y,&vec);
    for (size_t i=0;i<Y.size();i++)gsl_vector_set (vec, i, Y.at(i));
    double max = gsl_stats_max_index(vec->data,1,Y.size());
    gsl_vector_free(vec);
    return max;
}

double statistics::get_gastwirth_estimator_from_Y ( vector<double> Y )
{
	gsl_vector * vec = gsl_vector_alloc(Y.size());
    for (size_t i=0;i<Y.size();i++)
		gsl_vector_set (vec, i, Y.at(i));
	gsl_sort(vec->data,1,Y.size());
	double gastwirth = gsl_stats_gastwirth_from_sorted_data(vec->data,1,Y.size());
    gsl_vector_free(vec);
    return gastwirth;
}


double statistics::get_trimmed_mean_from_Y(vector<double> Y, float alpha)
{
	gsl_vector * vec = gsl_vector_alloc(Y.size());
    for (size_t i=0;i<Y.size();i++)
		gsl_vector_set (vec, i, Y.at(i));
	gsl_sort(vec->data,1,Y.size());
	double tr_mean = gsl_stats_trmean_from_sorted_data(alpha,vec->data,1,Y.size());
    gsl_vector_free(vec);
    return tr_mean;
}


double statistics::get_mean_from_Y(vector<double> Y) {
    gsl_vector * vec = gsl_vector_alloc(Y.size());
//     statistics::std_vec_to_gsl_vec(&Y,&vec);
    for (size_t i=0;i<Y.size();i++) gsl_vector_set (vec, i, Y.at(i));
    double max = gsl_stats_mean(vec->data,1,Y.size());
    gsl_vector_free(vec);
    return max;
}

//     stats.quantile = gsl_stats_quantile_from_sorted_data(vec->data,1,c,0.75);
double statistics::get_quantile_from_Y(vector<double> Y, double percentile) 
{
    gsl_vector * vec = gsl_vector_alloc(Y.size());
    for (size_t i=0;i<Y.size();i++)gsl_vector_set (vec, i, Y.at(i));
//     double median = gsl_stats_median(vec->data,1,Y.size());
	gsl_sort(vec->data,1,Y.size());
	double quantile = gsl_stats_quantile_from_sorted_data(vec->data,1,Y.size(),percentile);
    gsl_vector_free(vec);
    return quantile;
}

double statistics::get_median_from_Y(vector<double> Y) {
    gsl_vector * vec = gsl_vector_alloc(Y.size());
    for (size_t i=0;i<Y.size();i++)gsl_vector_set (vec, i, Y.at(i));
    double median = gsl_stats_median(vec->data,1,Y.size());
    gsl_vector_free(vec);
    return median;
}

double statistics::get_mad_from_Y(vector<double> Y) {
    gsl_vector * vec = gsl_vector_alloc(Y.size());
    for (size_t i=0;i<Y.size();i++)gsl_vector_set (vec, i, Y.at(i));
    int c = Y.size();
    double w[c];
    double mad = gsl_stats_mad(vec->data,1,c,w);
    gsl_vector_free(vec);
    return mad;
}

double statistics::get_sd_from_Y(vector<double> Y) {
    gsl_vector * vec = gsl_vector_alloc(Y.size());
    for (size_t i=0;i<Y.size();i++)gsl_vector_set (vec, i, Y.at(i));
    int c = Y.size();
    double w[c];
    double sd = gsl_stats_sd(vec->data,1,c);
    gsl_vector_free(vec);
    return sd;
}

int statistics::get_min_index_from_Y(vector<double> Y) {
    gsl_vector * vec = gsl_vector_alloc(Y.size());
//     statistics::std_vec_to_gsl_vec(&Y,&vec);
    for (size_t i=0;i<Y.size();i++)gsl_vector_set (vec, i, Y.at(i));
    double min = gsl_stats_min_index(vec->data,1,Y.size());
    gsl_vector_free(vec);
    return min;
}

// statis statistics::get_statistics_from_dataXY_transposed(vector<vector<double>> *data_XY_transposed) {
//     int c=data_XY_transposed->at(0).size(); // anzahl der zeilen
//     statis stats;
//     if (c<3) return stats;
//     double w[c];
//     
//     gsl_vector * vec = gsl_vector_alloc(c);
// //     statistics::std_vec_to_gsl_vec(&data_XY_transposed->at(1),&vec);
//     for (size_t i=0;i<c;i++) gsl_vector_set (vec, i, data_XY_transposed->at(1)[i]);
//     
//     stats.mean = gsl_stats_mean(vec->data,1,c);
//     stats.sd = gsl_stats_sd(vec->data,1,c);
//     
//     stats.max_index = gsl_stats_max_index(vec->data,1,c);
// //     stats.max = Y[stats.max_index];
//     stats.min_index = gsl_stats_min_index(vec->data,1,c);
// //     stats.min = Y[stats.min_index];
//     stats.abs_dev = gsl_stats_absdev(vec->data,1,c);
//     
//     stats.skew = gsl_stats_skew(vec->data,1,c);
//     stats.kurtosis = gsl_stats_kurtosis(vec->data,1,c);
//     stats.auto_corr = gsl_stats_lag1_autocorrelation(vec->data,1,c);
//     stats.median = gsl_stats_median(vec->data,1,c);
//     gsl_sort(vec->data,1,c);
//     stats.quantile = gsl_stats_quantile_from_sorted_data(vec->data,1,c,0.75);
//     stats.mean_trimmed = gsl_stats_trmean_from_sorted_data(0.25,vec->data,1,c);
//     stats.mad = gsl_stats_mad(vec->data,1,c,w);
//     stats.sn = gsl_stats_Sn_from_sorted_data(vec->data,1,c,w);
// //     stats.qn = gsl_stats_Qn_from_sorted_data(vec->data,1,c,w,wi);
//     
//     stats.area=statistics::get_area_under_dataXY_transposed(*data_XY_transposed,(*data_XY_transposed)[0][0],(*data_XY_transposed)[0].back());
//     stats.max=data_XY_transposed->at(1)[stats.max_index];
//     stats.min=data_XY_transposed->at(1)[stats.min_index];
//     stats.max_x=data_XY_transposed->at(0)[stats.max_index];
//     stats.max_x=data_XY_transposed->at(0)[stats.min_index];
// 
//     gsl_vector_free(vec);
//     return stats;
// }

// double statistics::get_mad(vector<double> Y) {
//     double mad;
//     int c=Y.size();
//     double w[c];
//     gsl_vector * vec;
//     statistics::std_vec_to_gsl_vec(&Y,&vec);
//     
// }

// statis statistics::get_statistics(vector<double> Y) {
//     statis stats;
//     int c=Y.size();
//     double w[c];
//     gsl_vector * vec = gsl_vector_alloc(Y.size());
// //     statistics::std_vec_to_gsl_vec(&Y,&vec);
//     for (size_t i=0;i<Y.size();i++) gsl_vector_set (vec, i, Y.at(i));
//     
//     stats.mean = gsl_stats_mean(vec->data,1,c);
//     stats.sd = gsl_stats_sd(vec->data,1,c);
//     
//     stats.min_index = gsl_stats_min_index(vec->data,1,c);
//     stats.max_index = gsl_stats_max_index(vec->data,1,c);
//     stats.max = Y[stats.max_index];
//     stats.min = Y[stats.min_index];
// //     stats.min = Y[stats.min_index];
//     stats.abs_dev = gsl_stats_absdev(vec->data,1,c);
//     
//     stats.skew = gsl_stats_skew(vec->data,1,c);
//     stats.kurtosis = gsl_stats_kurtosis(vec->data,1,c);
//     stats.auto_corr = gsl_stats_lag1_autocorrelation(vec->data,1,c);
//     stats.median = gsl_stats_median(vec->data,1,c);
//     gsl_sort(vec->data,1,c);
//     stats.quantile = gsl_stats_quantile_from_sorted_data(vec->data,1,c,0.75);
//     stats.mean_trimmed = gsl_stats_trmean_from_sorted_data(0.25,vec->data,1,c);
//     stats.mad = gsl_stats_mad(vec->data,1,c,w);
//     stats.sn = gsl_stats_Sn_from_sorted_data(vec->data,1,c,w);
// //     stats.qn = gsl_stats_Qn_from_sorted_data(vec->data,1,c,w,wi);
//     gsl_vector_free(vec);
//     return stats;
// }


double statistics::polynom_integrator (double x, void * params) {
    double sum=0;
    vector<double> *poly_params=(vector<double>*) params;
    for (int i=0;i<poly_params->size();i++) {
        sum=sum+poly_params->at(i)*pow(x,i);
    }
    return sum;
}


/* numerical integration*/
double statistics::get_area_under_dataXY_transposed(vector<vector<double>> data_XY_transposed,double lower_X, double upper_X) {
    vector<vector<double>> data_XY = tools::mat::transpose_matrix_double(data_XY_transposed);
    return statistics::get_area_under_dataXY(data_XY,lower_X,upper_X);
}



/* numerical integration*/
double statistics::get_area_under_dataXY(vector<vector<double>> data_XY,double lower_X, double upper_X) {
    if (upper_X<0 || lower_X <0) return -1;
    if (upper_X==lower_X) return 0;
    if (upper_X<lower_X) {
        double temp;
        temp=upper_X;
        upper_X=lower_X;
        lower_X=temp;
    }
    double sum=0;
    for (int i=1;i<data_XY.size();i++) {
        double X=data_XY.at(i)[0];
        double Y=abs(data_XY.at(i)[1]);
        double dX=abs(data_XY.at(i-1)[0]-X);
        if (X>=lower_X && X<=upper_X) sum=sum+dX*Y;
    }
    return sum;
}

/*analytical integration using polynoms*/
double statistics::get_area_under_polynom(vector<double> polynom_parameters, double lower_X, double upper_X) {
    if (upper_X<0 || lower_X <0) return -1;
    if (upper_X==lower_X) return 0;
    if (upper_X<lower_X) {
        double temp;
        temp=upper_X;
        upper_X=lower_X;
        lower_X=temp;
    }
    double sum=0;
    for (int i=1;i<polynom_parameters.size()+1;i++) {
        sum=sum+polynom_parameters[i-1]*(pow(upper_X,i)-pow(lower_X,i))/i;
    }
    return sum;
}

// bool statistics::gsl_vec_to_std_vec(gsl_vector * vec, vector<double> *std_vec) {
//     std_vec->clear();
//     for (size_t i=0;i<vec->size;i++) {
//         std_vec->push_back(gsl_vector_get (vec, i));
//     }
//     return true;
// }

vector<int> statistics::get_positions_of_zero_values(vector<vector<double>> matrix, double treshold) {
    vector<int> positions;
    for (int i=0;i<matrix.size();i++) {
        if (matrix[i].size()<2) return positions;
        if (abs(matrix[i][1]<treshold)) positions.push_back(i);
    }
    return positions;
}

vector<double> statistics::fit_poly_XY_transposed(vector<vector<double>> XY, int poly_grad) {
    vector<double> Y_fit;
    int i, n;
  double chisq;
  gsl_matrix *X, *cov;
  gsl_vector *y, *w, *c;
  const int p = poly_grad;
  vector<double> fit_parameters(p);
//     if (p>9) return false;
    
  n = XY[0].size();
    
  X = gsl_matrix_alloc (n, p);
  y = gsl_vector_alloc (n);
  w = gsl_vector_alloc (n);

  c = gsl_vector_alloc (p);
  cov = gsl_matrix_alloc (p, p);
    
  chisq=-1;
  for (i = 0; i < n; i++)
    {
      double xi=XY[0][i];
      double yi=XY[1][i];
      gsl_vector_set (y, i, yi);
      for (int ii=0;ii<p;ii++) {
          gsl_matrix_set (X, i, ii, pow(xi,ii));
      }
    }
  {
    gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc (n, p);
//     gsl_multifit_wlinear (X, w, y, c, cov, &chisq, work);
    gsl_multifit_linear (X, y, c, cov, &chisq, work);
    gsl_multifit_linear_free (work);
  }

#define C(i) (gsl_vector_get(c,(i)))
#define COV(i,j) (gsl_matrix_get(cov,(i),(j)))
    for (int ii=0;ii<p;ii++) {
        fit_parameters.at(ii)=C(ii);
    }
    
    gsl_matrix_free (X);
    gsl_vector_free (y);
    gsl_vector_free (w);
    gsl_vector_free (c);
    gsl_matrix_free (cov);
    return statistics::get_poly_Y(fit_parameters,XY[0]);

}

vector<double> statistics::get_poly_Y(vector<double> parameters, vector<double> X) {
    int p = parameters.size();
    vector<double> data_Y(X.size());
    double yi,xi;
    for (int x=0;x<X.size();x++) {
        yi=0;
        for (int i=0;i<p;i++) {
            xi=X.at(x);
            yi=pow(xi,i)*parameters.at(i)+yi;
        }
        data_Y[x]=(yi);
    }
    return data_Y;
}

// vector<double> statistics::get_polynom_Y(vector<double> parameters, vector<double> *data_X) {
//     int p = parameters.size();
//     vector<double> data_Y(data_X->size());
//     double yi,xi;
//     for (int x=0;x<data_X->size();x++) {
//         yi=0;
//         for (int i=0;i<p;i++) {
//             xi=data_X->at(x);
//             yi=pow(xi,i)*parameters.at(i)+yi;
//         }
//         data_Y[x]=(yi);
//     }
//     return data_Y;
// }

vector<double> statistics::derive_poly(vector<double> parameters) {
    vector<double> derived_parameters;
    int p = parameters.size();
    for (int i=1;i<p;i++) {
        derived_parameters.push_back(i*parameters[i]);
    }
    return derived_parameters;
}

vector<double> statistics::integrate_poly(vector<double> parameters) {
    vector<double> integrated_parameters;
    int p = parameters.size();
    integrated_parameters.push_back(0); // assuming integration with boarders 
    for (int i=1;i<p;i++) {
        integrated_parameters.push_back(parameters[i-1]/i);
    }
    return integrated_parameters;
}

bool statistics::first_derivative_poly(vector<double> *parameters, vector<vector<double>> *data_XY_dY) {
    int p = parameters->size();
    double xi;
    double yi;
    for (int x=0;x<data_XY_dY->size();x++) {
        yi=0;
        for (int i=1;i<p;i++) {
            xi=data_XY_dY->at(x)[0];
            yi=pow(xi,i-1)*i*parameters->at(i)+yi;
        }
        data_XY_dY->at(x).push_back(yi);
    }
    return false;
}

bool statistics::second_derivative_poly(vector<double> *parameters, vector<vector<double>> *data_XY_ddY) {
    int p = parameters->size();
    double xi;
    for (int x=0;x<data_XY_ddY->size();x++) {
        for (int i=2;i<p;i++) {
            xi=data_XY_ddY->at(x)[0];
            (data_XY_ddY->at(x)).push_back(pow(xi,i-2)*(i-1)*parameters->at(i));
        }
    }
    return false;
}



vector<double> statistics::get_polynom_Y(vector<double> parameters, vector<double> *data_X) {
    int p = parameters.size();
    vector<double> data_Y(data_X->size());
    double yi,xi;
    for (int x=0;x<data_X->size();x++) {
        yi=0;
        for (int i=0;i<p;i++) {
            xi=data_X->at(x);
            yi=pow(xi,i)*parameters.at(i)+yi;
        }
        data_Y[x]=(yi);
    }
    return data_Y;
}

double statistics::fit_poly (vector<vector<double>> *data_matrix, vector<double> *fit_parameters)  {
  int i, n;
  double chisq;
  gsl_matrix *X, *cov;
  gsl_vector *y, *w, *c;
  const int p = fit_parameters->size();
//     if (p>9) return false;
    
  n = data_matrix->size();
    
  X = gsl_matrix_alloc (n, p);
  y = gsl_vector_alloc (n);
  w = gsl_vector_alloc (n);

  c = gsl_vector_alloc (p);
  cov = gsl_matrix_alloc (p, p);
    
  chisq=-1;
  for (i = 0; i < n; i++)
    {
      double xi=data_matrix->at(i)[0];
      double yi=data_matrix->at(i)[1];
      gsl_vector_set (y, i, yi);
      for (int ii=0;ii<p;ii++) {
          gsl_matrix_set (X, i, ii, pow(xi,ii));
      }
    }
  {
    gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc (n, p);
//     gsl_multifit_wlinear (X, w, y, c, cov, &chisq, work);
    gsl_multifit_linear (X, y, c, cov, &chisq, work);
    gsl_multifit_linear_free (work);
  }

#define C(i) (gsl_vector_get(c,(i)))
#define COV(i,j) (gsl_matrix_get(cov,(i),(j)))
    for (int ii=0;ii<p;ii++) {
        fit_parameters->at(ii)=C(ii);
    }
    
    gsl_matrix_free (X);
    gsl_vector_free (y);
    gsl_vector_free (w);
    gsl_vector_free (c);
    gsl_matrix_free (cov);
  return chisq;
}

/*
bool statistics::polynomfit_XY(vector<vector<double>> data_XY,data *fitted_data, int polynom_grad, double interp_res) {
    if (polynom_grad<1) return false;
    // data_XY_XY contains 2 columns from ONE implanted element Y(X)
    double chisq_best,chisq;
    vector<double> fit_parameters(polynom_grad);
    vector<double> fit_parameters_best(polynom_grad);
    chisq_best=statistics::fit_poly(&data_XY,&fit_parameters);
//     if (chisq_best==-1) cout << "error\n";
    fit_parameters_best=fit_parameters;
    if (polynom_grad==1) {
        for (int i=1;i<15;i++) {
            fit_parameters.resize(i);
            chisq=statistics::fit_poly(&data_XY,&fit_parameters);
//             if (chisq==-1) cout << "error\n";
            if (chisq<chisq_best)  { 
                fit_parameters_best=fit_parameters;
                chisq_best=chisq;
            }
        }
    }
    fit_parameters=fit_parameters_best;
    fitted_data->fit_info.chisq_abs = round(chisq_best);
    fitted_data->fit_info.fit_parameters=fit_parameters;
    vector<double> X, Y;
    if (interp_res>0) {
        int points = data_XY.back()[0]/interp_res;
        X.resize(points);
        for (int i=0;i<points;i++) {
            X[i]=i*interp_res;
        }
        Y=statistics::get_polynom_Y(fit_parameters,&X);
    } else {
        vector<vector<double>> temp = Tools::transpose_matrix_double(data_XY);
        X=temp[0];
        Y=statistics::get_polynom_Y(fit_parameters,&X);
    }
    fitted_data->data_XY_transposed.push_back(X);
    fitted_data->data_XY_transposed.push_back(Y);
    vector<double> xx = Tools::transpose_matrix_double(data_XY).at(0);
    Y=statistics::get_polynom_Y(fit_parameters,&xx);
    double mean = statistics::get_mean_from_Y(Y);
    fitted_data->fit_info.chisq_rel = chisq_best/mean;
    
    return true;
}
*/


vector<double> statistics::interpolate_data_XY_transposed(vector<vector<double>> data_XY_transposed, vector<double> X) {
    vector<double> Y,xv,yv;
    double yi;
    double *x,*y;
//     vector<vector<double>> data_XY_XY_transposed=data_XY_transposed;
    
    const size_t c=data_XY_transposed[0].size();
    gsl_interp_accel *acc = gsl_interp_accel_alloc ();
//     gsl_spline *spline = gsl_spline_alloc (gsl_interp_cspline, c);
    gsl_spline *spline_akima = gsl_spline_alloc(gsl_interp_akima, c);
    xv=data_XY_transposed[0];
    yv=data_XY_transposed[1];
	print(xv);
    x=&xv[0];
    y=&yv[0];
    gsl_spline_init (spline_akima, x, y, c);
	cout << "init done\n";
//     for (size_t i = 0; i < c; ++i)
//     printf("%g %g\n", x[i], y[i]);
        
    for (int i=0; i < X.size(); i++) {
//         cout << X->at(i) << endl;
        yi = gsl_spline_eval (spline_akima, X.at(i), acc);
		cout << "A";
        
        Y.push_back(yi);
      }
      cout << endl;
    gsl_spline_free (spline_akima);
    gsl_interp_accel_free (acc);
    return Y;
}







/********* NEW *************/


vector<double> statistics::interpolate_bspline(map<double, double>& data_XY,vector<double> X_new, int bspline_degree)
{
	const size_t k = bspline_degree +1;
	const size_t n = data_XY.size();
// 	const size_t nbreak = data_XY.size();
	/* nbreak = ncoeffs + 2 - k */
// 	const size_t ncoeffs = nbreak - 2 + k;
	const size_t ncoeffs = 20;
	const size_t nbreak = ncoeffs + 2 - k;
	size_t i, j;
	gsl_bspline_workspace *bw;
	gsl_vector *B;
	double dy;
		gsl_rng *r;
	gsl_vector *c, *w;
	gsl_vector *x, *y;
	gsl_matrix *X, *cov;
	gsl_multifit_linear_workspace *mw;
	double chisq, Rsq, dof, tss;

	gsl_rng_env_setup();
	r = gsl_rng_alloc(gsl_rng_default);

	/* allocate a cubic bspline workspace (k = 4) */
	bw = gsl_bspline_alloc(k, nbreak);
	B = gsl_vector_alloc(ncoeffs);

	x = gsl_vector_alloc(n);
	y = gsl_vector_alloc(n);
	X = gsl_matrix_alloc(n, ncoeffs);
	c = gsl_vector_alloc(ncoeffs);
	w = gsl_vector_alloc(n);
	cov = gsl_matrix_alloc(ncoeffs, ncoeffs);
	mw = gsl_multifit_linear_alloc(n, ncoeffs);

	/* this is the data to be fitted */  
	vector<double> Xdata,Ydata;
	tools::vec::split_map_to_vecs(&data_XY,&Xdata,&Ydata);
	for (i = 0; i < n; ++i)
    {
		gsl_vector_set (y, i, Ydata[i]);
		gsl_vector_set (x, i, Xdata[i]);
//       double sigma;
//       double xi = (15.0 / (N - 1)) * i;
//       double yi = cos(xi) * exp(-0.1 * xi);
// 
//       sigma = 0.1 * yi;
//       dy = gsl_ran_gaussian(r, sigma);
//       yi += dy;
// 
//       gsl_vector_set(x, i, xi);
//       gsl_vector_set(y, i, yi);
//       gsl_vector_set(w, i, 1.0 / (sigma * sigma));
// 
//       printf("%f %f\n", xi, yi);
    }

  /* use uniform breakpoints on the intervall */
  gsl_bspline_knots_uniform(Xdata.front(), Xdata.back(), bw);
// 	gsl_bspline_knots(x,bw);
  /* construct the fit matrix X */
  for (i = 0; i < n; ++i)
    {
      double xi = gsl_vector_get(x, i);

      /* compute B_j(xi) for all j */
      gsl_bspline_eval(xi, B, bw);

      /* fill in row i of X */
      for (j = 0; j < ncoeffs; ++j)
        {
          double Bj = gsl_vector_get(B, j);
          gsl_matrix_set(X, i, j, Bj);
        }
    }
	
  /* do the fit */
  ///ATTENTION hier ist irgend ne matrix nicht quadratisch
  gsl_multifit_wlinear(X, w, y, c, cov, &chisq, mw);

  dof = n - ncoeffs;
  tss = gsl_stats_wtss(w->data, 1, y->data, 1, y->size);
  Rsq = 1.0 - chisq / tss;

  fprintf(stderr, "chisq/dof = %e, Rsq = %f\n", chisq / dof, Rsq);

  printf("\n\n");

	
  /* output the smoothed curve */
	if (X_new.size()==0) X_new = Xdata;
	vector<double> Y_new(X_new.size());
    vector<double> Y_err(X_new.size());
	
	for (int i = 0; i<X_new.size(); i++)
	{
		gsl_bspline_eval(X_new[i], B, bw);
		gsl_multifit_linear_est(B, c, cov, &Y_new[i], &Y_err[i]);
	}


	gsl_rng_free(r);
	gsl_bspline_free(bw);
	gsl_vector_free(B);
	gsl_vector_free(x);
	gsl_vector_free(y);
	gsl_matrix_free(X);
	gsl_vector_free(c);
	gsl_vector_free(w);
	gsl_matrix_free(cov);
	gsl_multifit_linear_free(mw);

	return Y_new;
}



vector<double> statistics::interpolate_data_XY(map<double,double> *data_XY, vector<double> *X) 
{

    vector<double> Y,xv,yv;
    double yi;
    double *x,*y;
    const size_t c=data_XY->size();

	if (c<10) return {};
    gsl_interp_accel *acc = gsl_interp_accel_alloc ();
    gsl_spline *spline_akima = gsl_spline_alloc(gsl_interp_akima, c);
	tools::vec::split_map_to_vecs(data_XY,&xv,&yv);

    x=&xv[0];
    y=&yv[0];
    gsl_spline_init (spline_akima, x, y, c);
	
	
    for (int i=0; i < X->size(); i++) {

        yi = gsl_spline_eval (spline_akima, X->at(i), acc);
        Y.push_back(yi);
      }
    gsl_spline_free (spline_akima);
    gsl_interp_accel_free (acc);
    return Y;
}


double statistics::integrate(map<double,double> *data_XY, double lower_limit_X, double upper_limit_X) 
{
	if (lower_limit_X<0 || upper_limit_X <0) return -1;
    if (upper_limit_X<lower_limit_X) {
        double temp;
        temp=upper_limit_X;
        upper_limit_X=lower_limit_X;
        lower_limit_X=temp;
    }
    double sum=0;
	map<double,double>::iterator it,it_prev;
	double dY,dX;
	it = data_XY->begin();
	int i=1;
    for (++it;it!=data_XY->end();++it)
	{
		i++;
		it_prev=it;
		--it_prev;
        dY=abs(it->second+it_prev->second)/2;
        dX=abs(it->first-it_prev->first);
        if (lower_limit_X==upper_limit_X || (it->first>=lower_limit_X && it->first<=upper_limit_X)) sum=sum+dX*dY;
    }
    return sum;
}



