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

#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#ifdef __unix__
#define PATH_DELIMITER string("/")
#define LINE_DELIMITER string("\n")

#include <pthread.h>
#else
#define PATH_DELIMITER string("\\")
#define LINE_DELIMITER string("\r\n")
#include "windows.h"
#endif


// #include <thread>
// #include <mutex>

#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <map>


#include "tools.hpp"
// #include "measurement.hpp"
#include "files_t.hpp"
// #include "fprint.hpp"
#include "pse.hpp"
#include "measurement_tools_t.hpp"
#include "export.hpp"
// #include "database_t.hpp"
#include "fit_functions.hpp"
#include "measurement_group_t.hpp"
#include "filename_t.hpp"
#include "sample_t.hpp"
#include <list>
#include "calc_manager_t.hpp"
#include "calc_models_t.jiang_t.hpp"
#include "globalvars.hpp"
#include "mass_interference_filter_t.hpp"
//tests//

//

using namespace std;

class processor 
{
	friend class config_t;
private: 
    static bool print_errors;
	static bool use_jiang;
	
	static bool use_mass_interference_filter;
	static bool force_RSF_to_foreign_matrix;

    ///arguments for executable "./dsims arg1 arg2 ..."
    vector<string> args;
    /// prefilter the filenames and get names of all measurement files;
    files_t files;
	/// parsed filenames
	list<filename_t> filenames;
	/// applying different parsers for the collection of tools on each file 
	measurement_tools_t measurement_tools;
	/// the measurement data structure
	list<measurement_t> measurements;
	/// the measurement groups
	list<measurement_group_t> measurement_groups;
	/// the samples
	list<sample_t> samples;
	
	/// the order of calculated measurement files
	vector<string> calculation_order_log;
	
	/// calculations
// 	string calc_results_to_string(measurement_group_t& MG, string prefix="");
// 	string calc_results_to_string(measurement_t* M, string prefix="");
// 	void calc_results_to_screen(measurement_t* M, string prefix="");
// 	void calc_results_to_screen(measurement_group_t& MG, string prefix="");
    
#ifdef __unix__
//     static void * thread_runner(void *data_p);
    ///functions that runs in one thread
#else
    ///functions that runs in one thread
//     DWORD WINAPI thread_runner(void *data_p);
#endif
    
    
    
    
    // the threads;
//     vector<thread> processor_threads;
    
public: // processor
    
    processor(vector<string> arg_list);
    ~processor();
    bool start();
};


#endif // PROCESSOR_HPP
