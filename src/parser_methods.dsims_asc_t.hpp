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
#ifndef PARSER_DSIMS_ASC_T_HPP
#define PARSER_DSIMS_ASC_T_HPP

#include <set>
#include <string>
#include <map>
#include "parser_methods.hpp"
// #include "tools.hpp"
#include "print.hpp"

using namespace std;

/*handles the specific parsing (dsims_asc_t file format) of one file */
class dsims_asc_t: public parser_methods 
{
private:
	class column_t
	{
	public:
		string unit;
		string cluster_name;
		string dimension;
		vector<double> data;
		void to_screen();
	};
	/// fals if cols have different sizes
	bool check_cols_size(vector<column_t>& cols);
	/// returns the corrrupted lines, which should be removes
	set<int> corrupted_lines();
	/// cluster_name -> column_t
	vector<column_t> columns();
	/// populates crater.sputer_depth or crater.sputter_time
	vector<double> get_common_X_dimension_vector();
	string common_X_dimension;
	/// needed for interpolation " clustername -> X -> Y data"
	map<string, map<double, double>> clusters_; 
	/// needed for interpolation
	map<double, double> Ipr_XY;
	
	map<string,string> settings;
	void add_data_time_to_measurement_p();
	double total_sputter_X=-1;
	double total_sputter_depth_p=-1;
	double total_sputter_time_p=-1;
	bool add_crater_total_sputter_time();
	/// return true, if one file_type_endings was found
// 	bool check_file_type();
	/// removes the lines, where some columns are empty
	void remove_corrupted_lines();
	bool parse_settings(vector<vector<vector<string>>> *headers);
// 	bool parse_file_name();
	bool parse_file_contents();
	/// populated by parse_headers
	vector<string> cluster_names, dimensions,units;
	/// parses the raw_header_tensor into vectors of dimensions, isotopes and units
	bool parse_headers(vector<vector<vector<string>>> *headers);
	/// parses the raw_header_tensor into vectors of dimensions, isotopes and units
	bool parse_cluster();

	
	/// shifts the Ipr column in the raw_data_tensor[0] matrix to correct values
	bool ipr_shift_correction(vector<vector<vector<string>>>* raw_data_tensor);
	/// the column position of Ipr X axis within the header-data matrix
	int Ipr_col;
	/// return true if raw_data_mat contains Ipr (primary current)
	bool check_Ipr();

	/// true, if Ipr was detected; false if no Ipr
	bool is_Ipr;
	///checks wheter this file is a dsims_asc compatible data format (true) or not (false)
	bool check();
	/// detected masses and their settings (bits, times, etc)
	map<string, vector<string>> calibrated_masses;
public:
	dsims_asc_t();
	/// tries to parse the filename: and overwrites parser_t::parse()
	bool parse(filename_t* filename_p, string* contents);
	/// parses the isotopes within one cluster
	static vector<isotope_t>  parse_isotopes(string cluster_name);
};


#endif // PARSER_DSIMS_ASC_T_HPP
