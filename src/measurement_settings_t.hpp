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
#ifndef MEASUREMENT_SETTINGS_T_HPP
#define MEASUREMENT_SETTINGS_T_HPP

#include <vector>
#include <string>
#include "quantity.hpp"
#include "filename_t.hpp"

using namespace std;



class measurement_settings_t
{
	friend class parser_methods;
	friend class dsims_asc_t;
private:
	
	quantity_t sputter_energy_p;
	string polarity_p="";
	string sputter_element_p="";
	bool egun_p=false;
	filename_t* filename;
public:
	/// eV
	quantity_t sputter_energy();
	//// +/-
	string polarity();
	/// Cs, O,
	string sputter_element();
	/// calibration parameters for masses
	map<string,vector<string>> clustername_to_mass_calib;
	void to_screen(string prefix="");
	///true if egun was on
	bool egun();
	bool operator == (measurement_settings_t& settings);
	bool operator != (measurement_settings_t& settings);
};

#endif // MEASUREMENT_SETTINGS
