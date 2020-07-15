/*
	Copyright (C) 2020 Florian Bärwolf
	baerwolf@ihp-microelectronics.com

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
#ifndef QUANTITY_HPP
#define QUANTITY_HPP

#include <string>
#include <map>
#include <vector>
#include "tools.hpp"
#include "statistics.hpp"
#include "fit_functions.hpp"
// #include "print.hpp"

using namespace std;

class unit_t;

class quantity_t {
private:
	
// 	double unequlibrium_data_end_point_index_p=-1;
// 	double unequlibrium_data_end_point_index();
	/// returns all data points within the measurement equilibrium // moved to cluster_t.hpp because of multi dimensional quantities
// 	quantity_t equilibrium(quantity_t& quantity);
	
	double max_s=-1;
public:
// 	int equlibrium_starting_pos=0;
	
	vector<double> data;
	string unit; // nm, sec,s g, at/ccm, c/s, cts
// 	unit_t unit;
	string dimension; // length, time, intensity, concentration, amount
	string name; // depth, sputter_depth, sputter_time, total_sputter_time, total_sputter_depth
	/// returns a string vector: 0:dimension,1:unit,n:data[0..end]
// 	vector<string> to_vec();
    /// false if data is vector
    bool is_skalar_data();
	/// false if data is empty
    bool is_data();
	
	static void reduce_string(string reduce_this);

	bool is_set();
	
	
	/// transform the quantity into a single line string
	string to_str();
	/// prints the quantity to cout
	void to_screen(string prefix="");
	
	/*statistics*/
	
	// moving windows
	quantity_t moving_window_median(int window_size=0);
	quantity_t moving_window_mad(int window_size=0);
	quantity_t moving_window_mean(int window_size=0);
	quantity_t moving_window_sd(int window_size=0);
	
	
	//skalars
	quantity_t quantile(double percentile=0.75);
	quantity_t median();
	quantity_t mean();
	quantity_t trimmed_mean(float alpha=0.25);
	quantity_t gastwirth();
	quantity_t sd();
	quantity_t mad();
	quantity_t max();
	/// returns x value at max(Y)
	quantity_t max_at_x(quantity_t X,double lower_X_limit, double upper_X_limit);
	quantity_t max_at_x(quantity_t X);
	
	quantity_t min();
	/// returns x value at min(Y)
	quantity_t min_at_x(quantity_t X,double lower_X_limit=0, double upper_X_limit=0);
	quantity_t integrate(quantity_t* x_data,double lower_X_limit=0, double upper_X_limit=0);
	quantity_t integrate(quantity_t x_data, double lower_X_limit=0, double upper_X_limit=0);
	/// point by point integration
	quantity_t integrate_pbp(quantity_t x_data);
	
	
	
	quantity_t fit_polynom_by_x_data(quantity_t x_data, quantity_t new_x_data, int polynom_grade=-1 );
	quantity_t polyfit(int polynom_grade=-1);
	

	
	
	
	
	quantity_t filter_recursive_median(int window_size=0);
	quantity_t filter_impulse(int window_size=0, float factor=4);
	quantity_t filter_gaussian(int window_size=0, double alpha=1);
	
	
	
	/// check whether resignation dim1 and dim2 belong to the same dimension
	static bool are_same_dimensions(string dim1, string dim2);
	bool is_same_dimension(quantity_t *quantity_p);
	bool is_same_dimension(string dimension_p);
	
	static bool are_same_units(string unit1, string unit2);
	bool is_same_unit(quantity_t *quantity_p);
	bool is_same_unit(string unit_p);
	
	quantity_t add_to_data(quantity_t quantity_p);
	quantity_t remove_from_data(quantity_t& quantity);
// 	quantity_t addition(quantity_t quantity_p);
// 	quantity_t add(double number);
	bool pow(int power);
	
// 	quantity_t multiply(quantity_t quantity_p);
// 	quantity_t divide_by(quantity_t quantity_p);
	
	
	/*OPERATORS*/
	quantity_t operator + (const quantity_t& quantity);
	quantity_t operator + (const double summand);
	quantity_t operator - (quantity_t& quantity);
	quantity_t operator * (const quantity_t& quantity_p);
	quantity_t operator/(const quantity_t& quantity_p);
	quantity_t operator * (const double factor);
	quantity_t operator/(const double divisor);
	
	
	bool operator == (const quantity_t& quantity);
	bool operator < (const quantity_t& quantity) const;
	bool operator != (const quantity_t& quantity);
};

/// SI units: s,m ,kg, A, K, mol, cd
class unit_t 
{
private:
	string _prefix;
	string _suffix;
	string _base;
	
	vector<string> base_units = {"m","g","s","t"};
	
public:
	unit_t(string unit_name);
	/// "nm", "s", "t", "kg", "Mg", "%", ....
	string name;
	/// returns prefix: "nm" --> "n";
	string prefix();
	/// "%" , "at%" 
	string suffix();
	/// "nm" --> "m", "kg"-->"kg"
	string si();
	/// "nm" --> "m", "kg"-->"g"
	string base();
	/// factor to become SI: "nm" --> "1E-9", "kg"->"1E0"
	double factor();
	/// factor to become unit: 0 if not possible
	double factor_to(unit_t become);
	/// factor to become unit: 0 if not possible
	double factor_to(string become);
	
	/// exponents for SI units: "SI unit" --> exponen: "nm" -> "1E-9"
	static map<string,double> exponent_SI;
	///exponent seconds
	int s=0;
	///exponent meter
	int m=0;
	///exponent kilo gramm
	int kg=0;
	///exponent Ampere
	int A=0;
	///exponent Kelvin
	int K=0;
	///exponent Mol
	int mol=0;
	///exponent Candela
	int cd=0;
	
	/// multiplier prefix
// 	double prefix;
	
	static map<string, double> prefixes;
};

// class dimension_t
// {
// public:
// 	string length = "length";
// 	string mass = "mass";
// 	string time = "time";
// 	/// electrical current
// 	string e_current = "e_current";
// 	string temperature = "temperature";
// 	string amount = "amount";
// 	/// lichstrom
// 	string luminous_intensity = "luminous_intensity";
// };

#endif // QUANTITY_HPP
