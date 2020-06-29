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

#ifndef database_HPP
#define database_HPP

#include <iostream>
#include <string>
#include <vector>
#include "tools.hpp"
#include <sqlite3.h> 
#include <regex>
#include "config.hpp"
#include <vector>
// #include "measurement.hpp"
// #include "sample_t.hpp"
// #include "measurement_group_t.hpp"




// Check out this link. The easiest way is to do the locking yourself, 
// and to avoid sharing the connection between threads. 
// Another good resource can be found here, and it concludes with:
// 
//     Make sure you're compiling SQLite with -DTHREADSAFE=1.
// 
//     Make sure that each thread opens the database file and keeps
//     its own sqlite structure.
// 
//     Make sure you handle the likely possibility that one or more 
//     threads collide when they access the db file at the same time: handle SQLITE_BUSY appropriately.
// 
//     Make sure you enclose within transactions the commands that 
//     modify the database file, like INSERT, UPDATE, DELETE, and others.
// 


using namespace std;

class database_t {
private:    
    sqlite3* DB;
    bool close();
    
	bool create_tables();
	
	bool create_table_everything();
	
	bool create_table_samples();
	bool create_table_sample_implants();
	bool create_table_sample_layers();
	
	bool create_table_measurements();
	bool create_table_measurement_settings();
	bool create_table_reference_measurement_isotopes();
	bool create_table_measurement_statistics();
	
	/// returns -1 if error, or the id of the last entry in the table
    int get_last_autoID_from_table(string table);
	///
	map<string, string> get_sample(int sample_id);
	map<string, string> get_measurement(int measurement_id);
	map<string, vector<string>> get_measurements(int sample_id);
	
	vector<string> error_messages;
	
    
	
    ///\brief the callback function for get_implanted_references
    static int callback_measurements(void *ptr, int argc, char **argv, char **azColName);
    ///\brief the callback function for get_implanted_references
    static int callback_implanted_reference_samples(void *ptr, int argc, char **argv, char **azColName);
public:
	bool open();
    
	bool execute_sql(std::__cxx11::string sql, int (*func_ptr)(void*,int,char**,char**)=NULL, void* func_arg=nullptr);
	database_t();
	~database_t();
	vector<string> get_error_messages();
	/*call backs*/
	///general callback function -> populates *ptr -> matrix(vector(vector(string)))
	static int callback_lines_cols(void *ptr, int argc, char **argv, char **azColName);
	///general callback function -> populates *ptr -> map(colname,vector(string) lines ))
	static int callback_lines_map(void *ptr, int argc, char **argv, char **azColName);
};

/// sqlite3
extern database_t db;
#endif // database_HPP
