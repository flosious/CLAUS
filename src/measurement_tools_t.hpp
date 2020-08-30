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
#ifndef measurement_tools_t_HPP
#define measurement_tools_t_HPP

#include <unordered_set>
#include <string>
#include <vector>
#include "filename_t.hpp"
#include <list>
#include <algorithm>
// #include "measurement.hpp"
// #include "sample_t.hpp"

#include "parser_methods.dsims_asc_t.hpp"
#include "parser_methods.tofsims_txt_t.hpp"
#include "parser_methods.images_t.hpp"
#include "parser_methods.xps_csv_t.hpp"
#include "parser_methods.dektak32_txt_t.hpp"

class measurement_t;
class sample_t;

using namespace std;

class measurement_tools_t
{
	friend class config_t;
private:
	static bool ignore_file_type_endings;
	bool load_file();

	vector<string> error_messages;
	int parsed_measurements_counter=0;
	
	bool is_correct_file_type(filename_t* filename_p,unordered_set<string> file_type_endings_list_p={});
	/// lists all file_type_endings defined by all methode/parsers
	unordered_set<string> file_types_endings_list;
	bool get_file_types_endings_list();
	bool add_secondary_measurement();
	
	void add_measurement_to_cluster(measurement_t* measurement);
	void add_sample(std::__cxx11::list< sample_t >* samples, measurement_t* measurement);
public:

	/// returns the error_messages
	vector<string> get_error_messages();
	
	/// returns all measurements from all tools and populates samples list
	list<measurement_t> measurements(list<filename_t>* filenames_p, list<sample_t>* samples_p=nullptr);
	/// tries to parse the filename; if failed: measurement_t.error = true
	measurement_t measurement(filename_t* filename_p);
	
	
	
	measurement_tools_t();
};

#endif // measurement_tools_t

