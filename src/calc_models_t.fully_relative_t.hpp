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
#ifndef CALC_MODELS_FULLY_RELATIVE_HPP
#define CALC_MODELS_FULLY_RELATIVE_HPP

#include <vector>
#include "calc_models_t.hpp"
#include "globalvars.hpp"

class calc_models_t::fully_relative_t
{
private:
	measurement_group_t MG;
	bool error_p=true;
	vector<string> error_messages_p;
public:
	vector<string> error_messages();
	bool is_error();
	/// returns the calculated MG
	measurement_group_t measurement_group();
	/// tries to calculate
	bool calc();
	fully_relative_t(measurement_group_t& MG_p);
	map<string,quantity_t> clustername_to_Crels();
	map<string,quantity_t> clustername_to_Irels();
};

#endif // CALC_MODELS_FULLY_RELATIVE_HPP
