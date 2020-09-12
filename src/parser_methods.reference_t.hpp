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
#ifndef PARSER_REFERENCE_T_HPP
#define PARSER_REFERENCE_T_HPP


#include "parser_methods.hpp"

class reference_t: public parser_methods
{
public:
	class column_t
	{
	public:
		string unit;
		string cluster_name;
		string dimension;
		vector<double> data;
		void to_screen();
		column_t(vector<string>& header_lines, vector<string>& data_lines);
	};
	static vector<string> possible_dimensions;
	vector<column_t> columns();
	bool parse_cluster();
	bool check();
	reference_t();
	
	/// tries to parse the filename: and overwrites parser_t::parse()
	bool parse(filename_t* filename_p, string* contents);
	/// parses the isotopes within one cluster
	static vector<isotope_t>  parse_isotopes(string cluster_name);
	
};

#endif // PARSER_REFERENCE_T_HPP
