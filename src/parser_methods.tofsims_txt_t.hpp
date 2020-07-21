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
#ifndef PARSER_TOFSIMS_TXT_T_HPP
#define PARSER_TOFSIMS_TXT_T_HPP

#include <string>
#include <map>
#include "parser_methods.hpp"
#include "cluster_t.hpp"

using namespace std;

/*tofsims new file format since at least 2019*/
class tofsims_txt_t: public parser_methods
{
private:
	
	
// 	vector<vector<string>> header();
// 	vector<vector<double>> data();
	
	bool check();
	
	/// parses the raw_header_tensor into vectors of dimensions, isotopes and units
	bool parse_cluster();
	/// set the values of identifiers_t id
	void set_ids();
	/// adds the crater depth from filename or settings to the clusters quantities
	bool add_crater_to_clusters();
public:
	tofsims_txt_t();
	/// tries to parse the filename: and overwrites parser_t::parse()
	bool parse(filename_t* filename_p, string* contents);
	/// parses the isotopes within one cluster
	static vector<isotope_t>  parse_isotopes(string isotopes);
};

#endif // PARSER_TOFSIMS_TXT_T_HPP
