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
#ifndef CRATER_T_HPP
#define CRATER_T_HPP

#include <vector>
#include "quantity.hpp"
#include "fit_functions.hpp"
#include <map>
#include "filename_t.hpp"
// #include "cluster_t.hpp"


class cluster_t;
class linescan_t;
class measurement_t;

class crater_t 
{
	friend class measurement_tools_t;
	friend class parser_methods;
	friend class xps_csv_t;
	friend class dsims_asc_t;
	friend class tofsims_txt_t;
	friend class cluster_t;
	friend class calc_models_t;
private:
	double sputter_depth_resolution=0;
	double sputter_time_resolution=0;
	vector<double> sputter_depth_old;
	bool already_calculated_sputter_depth=false;
	bool already_calculated_sputter_time=false;
	bool already_calculated_sputter_rate=false;
	
	map<string,cluster_t>* clusters;
// 	map<string, cluster_t>* clusters;
// 	measurement_t* measurement;
	filename_t* filename;
	/// additional values
	quantity_t total_sputter_depths_p;
	quantity_t total_sputter_time_p;
	quantity_t sputter_energy_p;
	quantity_t sputter_time_p;
	quantity_t sputter_depth_p;
	quantity_t sputter_rate_p;
	quantity_t sputter_current_p;
	quantity_t global_sputter_time_p;
public:
	/// crater profiles measured by profilometer (line scans)
	vector<linescan_t> linescans;
	/// depths of crater: parsed from filenames
	quantity_t total_sputter_depths();
	quantity_t total_sputter_depths_from_linescans();
	quantity_t total_sputter_depths_from_filename();
	
	quantity_t total_sputter_time();
	quantity_t sputter_energy();
	quantity_t sputter_time();
	quantity_t sputter_depth(double resolution=0);
	quantity_t sputter_rate();
	quantity_t sputter_current();
	quantity_t global_sputter_time();

// 	quantity_t unequilibrium_sputter_depth();
// 	quantity_t unequilibrium_sputter_time();
	
	void to_screen(string prefix="");
};

class linescan_t
{
private:
	vector <double> fit_parameters;
	fit_functions::asym2sig_t asym2sig;
	bool fitted=false;
public:
	/// the depth or height (z)
	quantity_t z;
	/// the surface (xy)
	quantity_t xy;
	/// calculates the depth from profile and returns it mean
	quantity_t depth();
	/// calculates roughness
	quantity_t roughness();
	/// origin -> peak-functions -> Asym2Sig
	quantity_t fit();
};

#endif // #define CRATER_T_HPP
