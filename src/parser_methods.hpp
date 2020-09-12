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
#ifndef PARSER_METHODS_T_HPP
#define PARSER_METHODS_T_HPP

#include "quantity.hpp"
#include <map>
#include <vector>
#include "measurement.hpp"
#include <regex>
#include "isotope.hpp"
#include <map>
#include "filename_t.hpp"
#include <unordered_set>
#include "cluster_t.hpp"

// #include "parser.hpp"
// class parser_t;
// class dsims_asc_t : public parser_t;

using namespace std;


/*handles ONE parser on one file*/
class parser_methods
{
	friend class config_t;
// 	friend class filename_t;
// 	friend class dsims_asc_t;
protected:
	static bool use_impulse_filter_on_data;
	/// INPUT
	filename_t* filename;
	/// INPUT
	string* file_contents;
	
	/// this should bep parsed by the childs
	measurement_t measurement_p;
	unordered_set<string> file_type_endings={"NULL"};
	string tool_name;
	bool error=false;
	vector<string> error_messages;
	///////////////////////////////
	
	bool check_file_type();
	
	
	bool global_sputter_time_in_sec_from_STOPtime();
	bool global_sputter_time_in_sec_from_STARTtime();
	bool add_global_sputter_time_in_sec_to_crater();
	///parses the filename and populates id
// 	bool parse_id_and_crater_depths();
    
	void set_secondary_measurement();
// 	bool parse_filename();
	
	/// populates raw_data_tensor, raw_header_tensor
	bool parse_data_and_header_tensors(string line_delimiter="\t");
	vector<vector<vector<string>>> raw_data_tensor, raw_header_tensor;
public:
	static void copy_secondaries_and_delete_from_list(list<measurement_t>* measurements);
	static bool add_secondary_to_primary(measurement_t* primary, measurement_t* secondary);
	/// returns the parsed measurement_p
	measurement_t measurement();
	///
	bool is_error();
	/// populates measurement_p
// 	bool parse(string* filename_p, string* file_contents_p);

	/// returns the name of the measurement tool
	string get_tool_name();
	/// returns the error_messages
	vector<string> get_error_messages();
	/// returns all file_type_endings
	unordered_set<string> get_file_type_endings();
	~parser_methods();
	parser_methods();
	/// called at the end of successfull parse
// 	void copy_to_measurement_p();
};



#endif // PARSER_METHODS_T_HPP

