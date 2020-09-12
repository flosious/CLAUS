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
#ifndef EXPORT_T_HPP
#define EXPORT_T_HPP

#include <map>
#include <set>

#include "calc_models_t.hpp"
#include "quantity.hpp"
// #include "config.hpp"
#include "measurement.hpp"
#include "measurement_group_t.hpp"
#include "statistics.hpp"
#include "cluster_t.hpp"
#include "calc_models_t.hpp"
#include "calc_models_t.jiang_t.hpp"

/*this is a new version*/
class export2_t
{
private:
	friend class config_t;
protected:
	static string data_column_delimiter;
	static string filename_config;
	static string directory_config;
	static vector<string> export_column_names;
	static map<string, string> replacements;
	static double depth_resolution;
	static int smoothing_moving_window_mean_size;
	static string calc_location;
	static bool export_calc_history;
	static bool export_calc_results;
	static bool export_MG_parameters;
	
	measurement_t *measurement;
	void check_placeholders(string& check_this);
	static void check_replacements(string& check_this);
	string filename(string file_ending="_CLAUS.txt");
	string root_directory();
	string olcdb();
	string lot();
	string wafer();
	string group();
	string repetition();
	string lot_split();
	string chip();
	string monitor();
	string tool_name();
	string energy();
	/// prints out not parseable parts as string delimited by "_"
	string others();
public:
	export2_t(measurement_t* measurement_p);
	static void export_contents_to_file(vector<string> contents, string filename_p, measurement_group_t MG, string sub_directory="");
	static void export_contents_to_file(string contents, string filename_p, measurement_group_t MG, string sub_directory="");
};

class origin_t : export2_t
{
private:
	friend class config_t;
	class column_t
	{
	public:
		static string suffix_conf;
		static string prefix_conf;

		
		bool populated=false;
		string longname="";
		string unit="";
		string comment="";
		vector<string> data;
		/// creates the column
		vector<string> vec();
		column_t(cluster_t* cluster, quantity_t* quantity, string suffix="", string prefix="");
		column_t(cluster_t* cluster, quantity_t quantity, string suffix="", string prefix="");
		column_t(quantity_t* quantity, string suffix="", string prefix="");
		column_t(quantity_t quantity, string suffix="", string prefix="");
		column_t(string longname, quantity_t quantity, string suffix="", string prefix="");
		column_t(vector<string> data_p, string longname_p, string unit_p, string comment_p);
	};
	
	static void apply_origin_replacements_on_string(string& replace_this);
	vector<column_t> format_measurement_cluster_columns();
	vector<column_t> format_measurement_cluster_columns_from_config();
	vector<column_t> format_calc_result_columns();
	vector<column_t> format_jiang_parameters_columns();
	static vector<column_t> format_settings_mass_calibration(measurement_group_t& MG_p);
	string suffix();
	string prefix();
	static vector<vector<string>> cols_to_matrix(vector<column_t>& columns);
	static bool write_to_file(vector<column_t> columns, string directory_s, string filename_p);
public:
	origin_t(measurement_t* measurement);
	static set<string> root_directories(measurement_group_t& MG);
	static void export_settings_mass_calibration_to_file(measurement_group_t& MG_p);
	static void export_to_files(measurement_group_t& MG_p);
	static void export_MG_parameters_to_file(measurement_group_t& MG_p);
	static void export_MG_parameters_to_file_V2(measurement_group_t& MG_p);
	static void export_jiang_parameters_to_file(calc_models_t::jiang_t& jiang);
	/*set -1 in data to empty string ""*/
	static vector<string> quantity_data_to_string(quantity_t& q);
};

class plot_t : export2_t
{
private:
	friend class config_t;
	vector<string> files_to_delete;
public:
	static string plots_location;
	static void export_to_files(list<measurement_t>* measurements);
	static void export_to_files(measurement_group_t* MG);
	static void export_to_files(list<measurement_group_t>& MGs);
	plot_t(measurement_t* measurement);
	static void fast_plot(quantity_t X, quantity_t Y, string outputfile="", bool logscale=true);
	static void fast_plot(vector<quantity_t> X, vector<quantity_t> Y, string outputfile="", bool logscale=true);
	~plot_t();
	
	void XY(vector<double> X, vector<double> Y,string outputfile);
	void XY(map<double,double> XY,string outputfile);
};

#endif // EXPORT_T_HPP
