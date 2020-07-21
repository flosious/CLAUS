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

#ifndef FILENAME_T_HPP
#define FILENAME_T_HPP

#include <string>
#include "quantity.hpp"
#include <vector>
#include <regex>
#include "config.hpp"
#include <map>
#include <list>


using namespace std;

class filename_t
{
private:
	vector<string> crater_depth_string_parts;
	string delimiter="_";
	string filename_with_path_p;
	bool parse_filename_parts();
	bool parse_olcdb(string filename_part);
	bool parse_sputter_energy(string filename_part);
	bool parse_lot(string filename_part);
	bool parse_wafer(string filename_part);
	bool parse_chip(string filename_part);
	bool parse_monitor(string filename_part);
	bool parse_group(string filename_part);
	bool parse_crater_depth(string filename_part);
	
	bool error=false;
	
public:
	
	bool is_error();
	
	vector<string> not_parseable_filename_parts;
	filename_t(string filename_p, string delimiter_p="_");
	
	string filename_with_path();
	string type_ending(string delimiter=".");
	string filename();
	string directory();
	string filename_without_crater_depths();
	
	string lot="";
	string lot_split="";
	string monitor="";
	string repetition="";
	
	int wafer=-1;
	int group=-1;
	int chip_x=-1;
	int chip_y=-1;
	int olcdb=-1;
	
	string polarity="";
	string sputter_element="";
	quantity_t sputter_energy;
	quantity_t total_sputter_depths;
	
	void to_screen(string prefix);
	
	static list<filename_t> parse(vector<string> filenames_with_path, string delimiter);
};

#endif // FILENAME_T_HPP
