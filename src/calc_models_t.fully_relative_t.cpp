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
#include "calc_models_t.fully_relative_t.hpp"
#include "globalvars.hpp"

vector<std::__cxx11::string> calc_models_t::fully_relative_t::error_messages()
{
	return error_messages_p;
}

bool calc_models_t::fully_relative_t::is_error()
{
	return error_p;
}

measurement_group_t calc_models_t::fully_relative_t::measurement_group()
{
	return MG;
}

calc_models_t::fully_relative_t::fully_relative_t(measurement_group_t& MG_p)
{
	// local copy to play with
	MG=MG_p;
}

bool calc_models_t::fully_relative_t::calc()
{
	if (!error_p) return true;
	calc_history.push_back("fully_relative_t\t" + MG.name());
	
	return true;
}

