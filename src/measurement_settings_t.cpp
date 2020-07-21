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
#include "measurement_settings_t.hpp"

std::__cxx11::string measurement_settings_t::polarity()
{
	if (polarity_p!="") return polarity_p;
	if (filename!=nullptr && filename->polarity!="") return filename->polarity;
	return "";
}

std::__cxx11::string measurement_settings_t::sputter_element()
{
	if (sputter_element_p!="") return sputter_element_p;
	if (filename!=nullptr && filename->sputter_element!="") return filename->sputter_element;
	return "";
}

quantity_t measurement_settings_t::sputter_energy()
{
	if (sputter_energy_p.is_set()) return sputter_energy_p;
	if (filename!=nullptr && filename->sputter_energy.is_set()) return filename->sputter_energy;
	return quantity_t();
}

void measurement_settings_t::to_screen(std::__cxx11::string prefix)
{
	cout << prefix << "polarity = " << polarity() << endl;
	cout << prefix << "sputter_element = " << sputter_element() << endl;
	if (sputter_energy().is_set()) sputter_energy().to_screen(prefix);
}

bool measurement_settings_t::egun()
{
	return egun_p;
}



/*OPERATORS*/

bool measurement_settings_t::operator==(measurement_settings_t& settings)
{
	if (polarity() != settings.polarity()) return false;
	if (sputter_element() != settings.sputter_element()) return false;
	if (sputter_energy() != settings.sputter_energy()) return false;
	return true;
}

bool measurement_settings_t::operator!=(measurement_settings_t& settings)
{
	return !operator==(settings);
}
