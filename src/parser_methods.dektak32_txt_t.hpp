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

#ifndef PARSER_METHODS_DEKTAK32_TXT_T_HPP
#define PARSER_METHODS_DEKTAK32_TXT_T_HPP

#include <set>
#include <string>
#include <map>
#include "parser_methods.hpp"

/*dektak32 profilometer export txts*/
class dektak32_txt_t: public parser_methods
{
private:	
	bool check();
	/// parses the raw_header_tensor into vectors of dimensions, isotopes and units
	bool parse_cluster();
	/// parses the raw_header_tensor into vectors of dimensions, isotopes and units
	bool parse_linescan();
	/// adds the crater depth from filename or settings to the clusters quantities
	bool add_crater_to_clusters();
public:
	dektak32_txt_t();
	/// tries to parse the filename: and overwrites parser_t::parse()
	bool parse(filename_t* filename_p, string* contents);
};


#endif // PARSER_METHODS_DEKTAK32_TXT_T_HPP
