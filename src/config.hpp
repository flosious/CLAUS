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

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <string>
#include <iostream>
#include "tools.hpp"
#include "print.hpp"

using namespace std;

class config_t {
private:
		/*FRIENDS*/
//     friend int main( int    argc, char **argv );
//     friend class processor;
//     friend class threadp;
//     friend class measurement_t;
//     friend class files_t;
//     friend class fprint;
// 	friend class pse_t;
public:

	/*CALCULATION METHOD STUFF*/
	string standard_reference_intensity_calculation_method = "pbp"; // "median"
    
    /*DEFAULTS*/
    ///default config file location
    string default_file = "config.conf";
    ///config file endings: *.conf & *.cfg
    vector<string> file_endings {".conf", ".cfg", ".config"};
    
    
    /*VARIABLES*/
    ///config file list
    vector<string> files;
    vector<string> files_loaded;
	vector<string> pse_file_locations;
	bool print_errors = false;
	
    /*FUNCTIONS*/   
    ///parse the lines to config
    int parse(vector<string> config_lines);
	/// saves locations of possible PSE files
	void save_pse_file_locations(string value);
    ///loads the actual file to RAM
    int load_file(std::string filename_with_path);
    /*save_functions*/
    ///sets the outformat columns (time depth intensity concentration)
//     void save_output_format(vector<string> values);
    ///sets value to test
    void save_test(string value);
    ///sets value to threads
    void save_threads(string value);
    ///sets value to filename_format
    void save_filename_format(string value);
    ///sets value to references_location
    void save_references_location(string value);
    ///sets value to plots_location
    void save_plots_location(string value);
    ///sets value to results_location
    void save_results_location(string value);
    ///sets value to calc_location
    void save_calc_location(string value);
    ///sets value to export_location
    void save_export_location(string value);
    ///sets value to matrix_isotopes
    void save_matrix_isotopes(string value);
    ///adds entry to replacements
    void save_replacements(vector<string> values);
    ///set types to measurement_file_types
    void save_measurement_file_types(string value);
    ///set the measurement tool (dsims, tofsims, xps,...)
//     void save_measurement_tool(string value);
	/// set ignore_filename_substrings
	void save_ignore_filename_substrings(string value);
	/// defines a measurement_group (groupid + olcdbid + settings + tool)
	void save_measurement_group_definition(string value);
	/// defines a measurement 
	void save_measurement_definition(string value);
	/// what should be exported? intensity, concentration, sputter_depth ....
	void save_export_types(string value);
    /*************/
    
    /*PARAMETERs*/
    /*processor stuff*/
    ///number of processor threads
    int threads_max=4;
//     string measurement_tool;
	bool input_files_are_one_measurement_group=false;
    // scan all filenames for wildcards and apply them (useful when using windows, that means shell not available)
	bool use_wildcards_in_filenames = false;
    /// scan all files in the directory of each measurement files
    bool use_directory_files_list = false;
	/// if true, programm will try to parse the file regardless of file ending, e.g. ".txt"
    bool ignore_file_type_endings = false;
	/// filenames (or parts of it) to ignore
    vector<string> ignore_filename_substrings;
	
    /*formatting*/
    ///filename
	string export_filename="";
	string export_types="";
    ///replace "this" with "with" in all strings (filenames, pathnames, headers, ...)
    map<string, string> replacements; 
    ///file endings of the measurement files
    vector<string> measurement_file_types {".asc",".txt",".asc_rpc"};
    ///data column delimiter
    string data_column_delimiter="\t";
    ///file name delimiter
    string file_name_delimiter="_";
    ///what to put in output files
//     vector<string> output_format {"time","depth","intensity","concentration"};
	
    /*directories*/
    ///path and name of the database
    string db_location="database.sqlite3";
    ///path of reference measurements
    string references_location="references";
    ///directory name of plots; no output when empty
    string plots_location="figures";
    ///directory name of results; no output when empty
    string results_location="results";
    ///directory name of calculation; no output when empty
    string calc_location="";
    ///directory name of exports; no output when empty
    string export_location="";
	
	/*definitions*/
    bool measurement_group_definition_olcdbid=true;
	bool measurement_group_definition_groupid=true;
	bool measurement_group_definition_settings=true;
	bool measurement_group_definition_tool=true;
	
	bool measurement_definition_olcdbid=true;
	bool measurement_definition_lot=true;
	bool measurement_definition_lot_split=false;
	bool measurement_definition_wafer=true;
	bool measurement_definition_monitor=true;
	bool measurement_definition_chip=true;
	bool measurement_definition_groupid=true;
	bool measurement_definition_repition=true;
	
	// könnte man als schnittstelle für eine Skriptsprache in der config nehmen: "median(concentration) + 
	bool export_type_sputter_time	=false;
	bool export_type_sputter_rate	=false;
	bool export_type_sputter_depth	=true;
	bool export_type_intensity		=false;
	bool export_type_RSF			=false;
	bool export_type_concentration	=true;
    
    /*calculation*/
    ///not used at the moment; these isotopes/cluster (or any combination of them) describe the matrix
    vector<string> matrix_isotopes {"70Ge","72Ge","73Ge","74Ge","28Si","29Si","30Si"};
    
    double sputter_time_resolution=0;
	double depth_resolution=0;
    

    ///constructor
    config_t();
    
    ///destructor
    //~config();
    
    ///load just default config file; returns the number of succesfully loaded files
    int load();
    ///loads one external config file; returns the number of succesfully loaded files
    int load(std::string filename_with_path);
    ///loads multiple external config file; returns the number of succesfully loaded files
    int load(std::vector<std::string> filenames_with_path);
    ///returns all config files in the file list
    vector<string> filter_files(vector<string> *file_list);
    
};

// singleton
extern config_t conf;
#endif // CONFIG.HPP
