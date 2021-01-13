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
#ifndef CALC_MODELS_JIANG_HPP
#define CALC_MODELS_JIANG_HPP

#include "calc_models_t.hpp"
#include "globalvars.hpp"
#include <math.h>
/*
 *	jiangs protocol for 2 element/isotope matrices/substrates
 * 	Jiang Z X et al 2006 Appl. Surf. Sci. 252 7262 
 * 	Bärwolf et al Semicond. Sci. Technol. 34 (2019) 014005 (6pp)
 * 	This class extends the original protocol by hihger polynomial grades
 */
class calc_models_t::jiang_t
{
	/// just for debugging, please remove !!
		friend class config_t;
private:
// 	static int SR_polynom_rank;
// 	static int Crel_polynom_rank ;
// 	static int RSF_polynom_rank ;
	
	int polynom_rank(pair<quantity_t,quantity_t> pairs);
	
	measurement_group_t* measurement_group_priv;
	
	bool error_p=true;
	
	///not in use yet
	bool apply_Crel_to_measurement(measurement_t& M);
	///not in use yet
	bool apply_SR_to_measurement(measurement_t& M);
	///not in use yet
	bool apply_RSF_to_measurement(measurement_t& M);
	
	
	vector<string> error_messages_p;
	/// C(matrix_isotope2.atoms) / C(matrix_isotope1.atoms)
	quantity_t Crel(sample_t::matrix_t& matrix);
	quantity_t Crel(measurement_t* measurement);
	/// cluster(matrix_isotope2).intensity() / cluster(matrix_isotope1).intensity()
	quantity_t Irel(measurement_t* measurement);
	
	set<isotope_t> single_element_matrix_isotopes_p;
	/// looks in all measurements in all matrix->isotopes
	isotope_t matrix_isotopes_or_elements_with_LEAST_relative_concentration_in_ref_sample();
	/// atoms=1; isotopes with 100at% concentration in MG
	set<isotope_t> single_element_matrix_isotopes();
	
	
	isotope_t counter_matrix_isotope_p;
	
	isotope_t denominator_matrix_isotope_p;
	
	pair<quantity_t,quantity_t> SRs_to_Crel_p;
	pair<quantity_t,quantity_t> Crel_to_Irel_p;
	map<string,pair<quantity_t,quantity_t>> clustername_to_RSFs_to_Crel_p;
public:
	fit_functions::polynom_t SR_to_Crel_polyfit;
	fit_functions::polynom_t CRel_to_Irel_polyfit;
	map<string, fit_functions::polynom_t> clustername_to_RSFs_to_Crel_polyfit;
	
	/// zähler
	isotope_t counter_matrix_isotope(); 
	/// nenner
	isotope_t denominator_matrix_isotope(); 
	bool is_error();
	pair<quantity_t,quantity_t> SRs_to_Crel();
	pair<quantity_t,quantity_t> Crel_to_Irel();
	map<string,pair<quantity_t,quantity_t>> clustername_to_RSFs_to_Crel();
	pair<quantity_t,quantity_t> RSFs_to_Crel(string cluster_name);
	vector<string> error_messages();
	/// the new measurement_group with populated jiang parameters on success
	measurement_group_t measurement_group();
	bool calc(/*measurement_group_t& measurement_group_p*/);
// 	/// overwrites all concentration and depth related parameters
// 	measurement_group_t calc(measurement_t* measurement_p);
// 	/// overwrites all concentration and depth related parameters
// 	measurement_group_t calc(cluster_t* cluster_p);
	
	jiang_t(measurement_group_t& measurement_group_p);
};

#endif // CALC_MODELS_JIANG_HPP
