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
#ifndef EXPORT_T_HPP
#define EXPORT_T_HPP

#include <map>
#include <set>

#include "calc_models_t.hpp"
#include "quantity.hpp"
#include "config.hpp"
#include "measurement.hpp"
#include "measurement_group_t.hpp"
#include "statistics.hpp"
#include "cluster_t.hpp"
#include "calc_models_t.jiang_t.hpp"

/*this is a new version*/
class export2_t
{
private:
	
protected:
	measurement_t *measurement;
	void check_placeholders(string& check_this);
	static void check_replacements(string& check_this);
	string filename(string filename=conf.export_filename, string file_ending="_CLAUS.txt");
	string root_directory(string directory=conf.export_location);
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
public:
	export2_t(measurement_t* measurement_p);
	static void export_contents_to_file(vector<string> contents, string filename_p, measurement_group_t MG, string sub_directory="");
};

class export_t
{
protected:
	string olcdb,lot,wafer,group,chip,monitor,tool,lot_split;
	bool format_id();
	
	/// parses the export directory name given in the config: like "C:\{oldbid}\{lotid}\..." and replaces them
	bool check_replacements(string& check_this);
	string directory;
	string filename;
	string output;
	measurement_t *measurement;
	/// writes output to file
	bool write_to_file();
	bool get_filename(string file_ending = "_EXPORT.txt");
	bool get_directory();
	static string get_directory(measurement_t *measurement);
	
public:
	export_t(measurement_t *measurement);
// 	export_t(measurement_group_t *MG);
	/// sets output with origin formatted string
	static bool samples_overview(list<measurement_t> *measurements);
	static bool Ipr_global(list<measurement_t> *measurement);
};

class origin_t : export2_t
{
private:
	class column_t
	{
	public:
		string measurement_name="";
		bool populated=false;
		string longname="";
		string unit="";
		string comment="";
		vector<string> data;
		/// creates the column
		vector<string> vec();
		column_t(cluster_t* cluster, quantity_t* quantity, string suffix="");
		column_t(cluster_t* cluster, quantity_t quantity, string suffix="");
		column_t(quantity_t* quantity, string suffix="");
		column_t(quantity_t quantity, string suffix="");
		column_t(string longname, quantity_t quantity, string suffix="");
		column_t(vector<string> data_p, string longname_p, string unit_p, string comment_p);
	};
	static void apply_origin_replacements_on_string(string& replace_this);
	vector<column_t> format_measurement_cluster_columns();
	vector<column_t> format_measurement_cluster_columns_from_config();
	vector<column_t> format_calc_result_columns();
	vector<column_t> format_jiang_parameters_columns();
	string suffix();
	static vector<vector<string>> cols_to_matrix(vector<column_t>& columns);
	static bool write_to_file(vector<column_t> columns, string directory_p, string filename_p);
public:
	origin_t(measurement_t* measurement);
	
	static void export_settings_mass_calibration_to_file(measurement_group_t MG_p);
	static void export_to_files(measurement_group_t MG_p);
	static void export_MG_parameters_to_file(measurement_group_t& MG_p);
	static void export_jiang_parameters_to_file(calc_models_t::jiang_t& jiang);
};

// class origin_t : export_t
// {
// private:
// 	bool error=false;
// protected:
// 	string suffix();
// 	class column_t
// 	{
// 	public:
// 		string measurement_name="";
// 		bool populated=false;
// 		string longname="";
// 		string unit="";
// 		string comment="";
// 		vector<string> data;
// 		/// creates the column
// 		vector<string> vec();
// 		column_t(cluster_t* cluster, quantity_t* quantity, string suffix="");
// 		column_t(cluster_t* cluster, quantity_t quantity, string suffix="");
// 		column_t(quantity_t* quantity, string suffix="");
// 		column_t(quantity_t quantity, string suffix="");
// 	};
// 	/// origin compatible columns for the whole measurement
// 	vector<column_t> columns;
// 	vector<vector<string>> matrix();
// public:
// 	origin_t(measurement_t *measurement);
// 	bool export_to_file();
// 	static bool write(vector<column_t>& cols, string dir,string fname);
// 	static void export_to_files(list< measurement_t >* measurements);
// 	static void export_to_files(list< measurement_t* > measurements);
// 	static void export_to_files(list< measurement_group_t> MGs);
// 	static void export_to_files(measurement_group_t MG);
// 	static void export_to_files(calc_models_t::jiang_t& jiang);
// 	static void export_overview_to_files(list< measurement_t >* measurements);	
// };

class plot_t : export_t
{
private:
	vector<string> files_to_delete;
public:
	static void export_to_files(list<measurement_t>* measurements);
	plot_t(measurement_t* measurement);
	static void fast_plot(quantity_t X, quantity_t Y, string outputfile);
	~plot_t();
	
	void XY(vector<double> X, vector<double> Y,string outputfile);
	void XY(map<double,double> XY,string outputfile);
};

#endif // EXPORT_T_HPP
