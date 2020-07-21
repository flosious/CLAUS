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
#include "fit_functions.hpp"

/*************
 * asym2sig_t*
 *************/

double fit_functions::asym2sig_t::chisq()
{
	return chisq_p;
}


int fit_functions::asym2sig_t::function(const gsl_vector * x, void *data, gsl_vector * f)
{
  map<double, double> *data_XY = ((map<double, double> *)data);
	
//   size_t n = ((struct data *)data)->n;
//   double *t = ((struct data *)data)->t;
//   double *y = ((struct data *)data)->y;

  // y0,m,A,xc,w1,w2,w3
	double y0 = gsl_vector_get (x, 0);
	double m = gsl_vector_get (x, 1);
	double A = gsl_vector_get (x, 2);
	double xc = gsl_vector_get (x, 3);
	double w1 = gsl_vector_get (x, 4);
	double w2 = gsl_vector_get (x, 5);
	double w3 = gsl_vector_get (x, 6);

  size_t i=0;

//   for (i = 0; i < data_XY->size(); i++)
	for (map<double,double>::iterator XY=data_XY->begin();XY!=data_XY->end();++XY)
    {
      /* Model Yi = A * exp(-lambda * t_i) + b */
//       double Yi = A * exp (-lambda * t[i]) + b;
		double Yi = y0 + m*XY->first + A/(1+exp(-((XY->first-xc+w1/2)/w2)))*( 1-1/(1+exp(-((XY->first-xc-w1/2)/w3))) );
      gsl_vector_set (f, i, Yi - XY->second);
	  i++;
    }

  return GSL_SUCCESS;
}

void fit_functions::asym2sig_t::callback(const size_t iter, void *params, const gsl_multifit_nlinear_workspace *w)
{
  gsl_vector *f = gsl_multifit_nlinear_residual(w);
  gsl_vector *x = gsl_multifit_nlinear_position(w);
  double rcond;

  /* compute reciprocal condition number of J(x) */
  gsl_multifit_nlinear_rcond(&rcond, w);

  fprintf(stderr, "iter %2zu: A = %.4f, lambda = %.4f, b = %.4f, cond(J) = %8.4f, |f(x)| = %.4f\n",
          iter,
          gsl_vector_get(x, 0),
          gsl_vector_get(x, 1),
          gsl_vector_get(x, 2),
          1.0 / rcond,
          gsl_blas_dnrm2(f));
}


bool fit_functions::asym2sig_t::fit(map<double, double> data_XY)
{
	// maximum time for calculation?
// 	double TMAX=60;
	
	const gsl_multifit_nlinear_type *T = gsl_multifit_nlinear_trust;
	gsl_multifit_nlinear_workspace *w;
	gsl_multifit_nlinear_fdf fdf;
	gsl_multifit_nlinear_parameters fdf_params = gsl_multifit_nlinear_default_parameters();
	// number of data points to fit
	int N = data_XY.size();
	const size_t n = N;
	// number of fit parameters
	const size_t p = 7;

	gsl_vector *f;
	gsl_matrix *J;
	gsl_matrix *covar = gsl_matrix_alloc (p, p);
	double /* t[N], y[N],*/ weights[N];
// 	struct data d = { n, t, y };
	
	vector<double> x_,y_;
	tools::vec::split_map_to_vecs(&data_XY,&x_,&y_);
	/* starting values */
	
	// y0,m,A,xc,w1,w2,w3
	xc = data_XY.rbegin()->first/2;
// 	w1=-10;
	y0 = y_[0];
	A = y0+y_[y_.size()/2];
	m = (y_.back()-y_[0])/(x_.back()-x_[0]);
	
	double x_init[p] = { y0, m, A, xc, 300,10,10}; 
	/******************/
	
	
	gsl_vector_view x = gsl_vector_view_array (x_init, p);
	gsl_vector_view wts = gsl_vector_view_array(weights, n);
	gsl_rng * r;
	double chisq0;
	int status, info;
	size_t i;

	const double xtol = 1e-8;
	const double gtol = 1e-8;
	const double ftol = 0.0;

	gsl_rng_env_setup();
	r = gsl_rng_alloc(gsl_rng_default);

	/* define the function to be minimized */
	fdf.f = function;
	fdf.df = NULL;   /* set to NULL for finite-difference Jacobian */
	fdf.fvv = NULL;     /* not using geodesic acceleration */
	fdf.n = n;
	fdf.p = p;
// 	fdf.params = &d;
	fdf.params = &data_XY;
// 	vector<double> X,Y;
// 	tools::vec::split_map_to_vecs(&data_XY,&X,&Y);
// 	statistics::std_vec_to_gsl_vec(&Y,y);
	
	/* this is the data to be fitted */
	for (i = 0; i < n; i++)
	{
// 		double ti = i * TMAX / (n - 1.0);
// 		double yi = 1.0 + 5 * exp (-0.1 * ti);
// 		double si = 0.1 * yi;
// 		double dy = gsl_ran_gaussian(r, si);
// 
// 		t[i] = ti;
// 		y[i] = yi + dy;
// 		weights[i] = 1.0 / (si * si);
		weights[i] = 1.0;
// 		printf ("data: %g %g %g\n", ti, y[i], si);
    };

	/* allocate workspace with default parameters */
	w = gsl_multifit_nlinear_alloc (T, &fdf_params, n, p);

	/* initialize solver with starting point and weights */
	gsl_multifit_nlinear_winit (&x.vector, &wts.vector, &fdf, w);

	/* compute initial cost function */
	f = gsl_multifit_nlinear_residual(w);
	gsl_blas_ddot(f, f, &chisq0);

	/* solve the system with a maximum of 100 iterations */
// 	status = gsl_multifit_nlinear_driver(100, xtol, gtol, ftol, callback, NULL, &info, w);
	status = gsl_multifit_nlinear_driver(100, xtol, gtol, ftol, NULL, NULL, &info, w);

	/* compute covariance of best fit parameters */
	J = gsl_multifit_nlinear_jac(w);
	gsl_multifit_nlinear_covar (J, 0.0, covar);

	/* compute final cost */
	gsl_blas_ddot(f, f, &chisq_p);

	#define FIT(i) gsl_vector_get(w->x, i)
	#define ERR(i) sqrt(gsl_matrix_get(covar,i,i))
	
	/* save fit parameters*/
	y0 = FIT(0);
	m = FIT(1);
	A = FIT(2);
	xc = FIT(3);
	w1 = FIT(4);
	w2 = FIT(5);
	w3 = FIT(6);
	fitted_p = true;
	
// 	fprintf(stderr, "summary from method '%s/%s'\n", gsl_multifit_nlinear_name(w), gsl_multifit_nlinear_trs_name(w));
// 	fprintf(stderr, "number of iterations: %zu\n",	gsl_multifit_nlinear_niter(w));
// 	fprintf(stderr, "function evaluations: %zu\n", fdf.nevalf);
// 	fprintf(stderr, "Jacobian evaluations: %zu\n", fdf.nevaldf);
// 	fprintf(stderr, "reason for stopping: %s\n", (info == 1) ? "small step size" : "small gradient");
// 	fprintf(stderr, "initial |f(x)| = %f\n", sqrt(chisq0));
// 	fprintf(stderr, "final   |f(x)| = %f\n", sqrt(chisq));
// 
// 	{
// 		double dof = n - p;
// 		double c = GSL_MAX_DBL(1, sqrt(chisq / dof));
// 
// 		fprintf(stderr, "chisq/dof = %g\n", chisq / dof);
// 
// 		fprintf (stderr, "y0      = %.5f +/- %.5f\n", FIT(0), c*ERR(0));
// 		fprintf (stderr, "m       = %.5f +/- %.5f\n", FIT(1), c*ERR(1));
// 		fprintf (stderr, "A       = %.5f +/- %.5f\n", FIT(2), c*ERR(2));
// 		fprintf (stderr, "xc      = %.5f +/- %.5f\n", FIT(3), c*ERR(3));
// 		fprintf (stderr, "w1      = %.5f +/- %.5f\n", FIT(4), c*ERR(4));
// 		fprintf (stderr, "w2      = %.5f +/- %.5f\n", FIT(5), c*ERR(5));
// 		fprintf (stderr, "w3      = %.5f +/- %.5f\n", FIT(6), c*ERR(6));
// 	}
// 
// 	fprintf (stderr, "status = %s\n", gsl_strerror (status));

	gsl_multifit_nlinear_free (w);
	gsl_matrix_free (covar);
	gsl_rng_free (r);

	return true;
}

void fit_functions::asym2sig_t::print_parameters()
{
	cout << "y0 =" << y0 << endl;
	cout << "m =" << m << endl;
	cout << "A =" << A << endl;
	cout << "xc =" << xc << endl;
	cout << "w1 =" << w1 << endl;
	cout << "w2 =" << w2 << endl;
	cout << "w3 =" << w3 << endl;
}

vector<double> fit_functions::asym2sig_t::fitted_y_data(vector<double> x)
{
	if (!fitted_p) return {};
	vector<double> y_data(x.size());
	for (int i=0;i<x.size();i++)
		y_data[i] = y0 + m*x[i] + A / (1+exp(-((x[i]-xc+w1/2)/w2)))*( 1-1/(1+exp(-((x[i]-xc-w1/2)/w3))) );
	return y_data;
}

bool fit_functions::asym2sig_t::fitted()
{
	return fitted_p;
}

/************
 **POLYNOM***
 ************/

vector<double> fit_functions::polynom_t::fitted_y_data(vector<double> x)
{
	if (!fitted_p) return {};
	vector<double> Y(x.size());
	for (int i=0;i<x.size();i++)
	{
		Y[i]=fit_parameters[0];
		for (int p=1;p<fit_parameters.size();p++)
			Y[i]+=fit_parameters[p]*pow(x[i],p);
	}
	return Y;
}

double fit_functions::polynom_t::chisq()
{
	return chisq_p;
}

bool fit_functions::polynom_t::fitted()
{
	return fitted_p;
}


bool fit_functions::polynom_t::fit(map<double, double> data_XY, int grade)
{
	if (grade<0) return false;
	if (grade>=data_XY.size()) return false;
	fitted_p = false;
	int i, n;
	gsl_matrix *X, *cov;
	gsl_vector *y, *w, *c;
	const int p = grade+1;
	fit_parameters.resize(p);
    
	n = data_XY.size();
    
	X = gsl_matrix_alloc (n, p);
	y = gsl_vector_alloc (n);
	w = gsl_vector_alloc (n);

	c = gsl_vector_alloc (p);
	cov = gsl_matrix_alloc (p, p);
    
	vector<double> Xdata,Ydata;
	tools::vec::split_map_to_vecs(&data_XY,&Xdata,&Ydata);
	chisq_p=-1;
	
	for (i = 0; i < n; i++)
    {
		gsl_vector_set (y, i, Ydata[i]);
		for (int ii=0;ii<p;ii++) {
		gsl_matrix_set (X, i, ii, pow(Xdata[i],ii));
      }
    }
//     cout << endl << "data_XY.size()=" << data_XY.size() << endl;
  {
    gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc (n, p);
    gsl_multifit_linear (X, y, c, cov, &chisq_p, work);
    gsl_multifit_linear_free (work);
  }

#define C(i) (gsl_vector_get(c,(i)))
#define COV(i,j) (gsl_matrix_get(cov,(i),(j)))
    for (int ii=0;ii<p;ii++) {
        fit_parameters.at(ii)=C(ii);
    }
    fitted_p = true;
    gsl_matrix_free (X);
    gsl_vector_free (y);
    gsl_vector_free (w);
    gsl_vector_free (c);
    gsl_matrix_free (cov);
	return true;
}


