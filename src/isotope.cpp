/*
	Copyright (C) 2020 Florian Bärwolf
	baerwolf@ihp-microelectronics.com

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
#include "isotope.hpp"

quantity_t isotope_t::mass()
{
	if (nucleons==-1) return PSE.get_mass_from_isotope(symbol);
	string isotope_name= to_string(nucleons) + symbol;
	return PSE.get_mass_from_isotope(isotope_name);
}

quantity_t isotope_t::abundance()
{
	if (nucleons==-1) return PSE.get_abundance_from_isotope(symbol);
	string isotope_name= to_string(nucleons) + symbol;
	return PSE.get_abundance_from_isotope(isotope_name);
}


void isotope_t::to_screen(string prefix)
{
	cout << prefix << name() << endl;
	mass().to_screen(prefix+"\t");
	abundance().to_screen(prefix+"\t");
}

bool isotope_t::sort_lower_mass(isotope_t i, isotope_t j)
{
	return i.mass().data[0]<j.mass().data[0];
}

std::__cxx11::string isotope_t::name()
{
	if (atoms==1 && nucleons==-1) return symbol;
	if (atoms==1) return to_string(nucleons) + symbol;
	if (nucleons==-1) return symbol + to_string(atoms);
	return to_string(nucleons) + symbol + to_string(atoms);
}

isotope_t::isotope_t()
{
	return;
}

isotope_t::isotope_t ( std::__cxx11::string name_to_parse )
{
// 	cout << "name_to_parse=" << name_to_parse << endl;
	smatch match;; 
	if (regex_search(name_to_parse,match,regex("^([0-9]{0,3})([a-zA-Z]{1,3})([0-9]*)"))) 
	{
		if (match[1]!="") nucleons = tools::str::str_to_int(match[1]);
		if (match[2]!="") symbol = match[2];
		if (match[3]!="") atoms = tools::str::str_to_int(match[3]);
		else atoms = 1;
	}
	else if (regex_search(name_to_parse,match,regex("^([a-zA-Z]{1,3})([0-9]*)"))) 
	{
		if (match[1]!="") nucleons = -1;
		if (match[2]!="") symbol = match[1];
		if (match[3]!="") atoms = tools::str::str_to_int(match[2]);
		else atoms = 1;
	}
	else if (regex_search(name_to_parse,match,regex("^([a-zA-Z]{1,3})"))) 
	{
		if (match[1]!="") nucleons = -1;
		if (match[2]!="") symbol = match[1];
		if (match[3]!="") atoms = 1;
	}
}

bool isotope_t::is_set()
{
	if (symbol=="NULL") return false;
// 	if (nucleons==-1) return false;
	return true;
}

vector<isotope_t>  isotope_t::not_common_elements(const vector<isotope_t>& elements1, const vector<isotope_t>& elements2)
{
	bool found=false;
	vector<isotope_t> results;
// 	isotope_t result_temp;
	for (auto& element1:elements1)
	{
		found = false;
// 		result_temp.symbol=element1.symbol;
		for (auto& element2:elements2)
		{
			if (element1.symbol==element2.symbol)
			{
				found = true;
				break;
			}
		}
		if (!found) results.push_back(element1);
	}
	return results;
}
vector<isotope_t>  isotope_t::common_elements(const vector<isotope_t>& elements1, const vector<isotope_t>& elements2)
{
	bool found=false;
	vector<isotope_t> results;
// 	isotope_t result_temp;
	for (auto& element1:elements1)
	{
		found = false;
// 		result_temp.symbol=element1.symbol;
		for (auto& element2:elements2)
		{
			if (element1.symbol==element2.symbol)
			{
				found = true;
				break;
			}
		}
		if (found) results.push_back(element1);
	}
	return results;
}

vector<isotope_t>  isotope_t::not_common(const vector<isotope_t>& isotopes1, const vector<isotope_t>& isotopes2)
{
	bool found=false;
	vector<isotope_t> results;
	for (auto& isotope1:isotopes1)
	{
		found = false;
		for (auto& isotope2:isotopes2)
		{
			if (isotope1==isotope2)
			{
				found = true;
				break;
			}
		}
		if (!found) results.push_back(isotope1);
	}
	return results;
}
vector<isotope_t>  isotope_t::common(const vector<isotope_t>& isotopes1, const vector<isotope_t>& isotopes2)
{
	bool found=false;
	vector<isotope_t> results;
	for (auto& isotope1:isotopes1)
	{
		found = false;
		for (auto& isotope2:isotopes2)
		{
			if (isotope1==isotope2)
			{
				found = true;
				break;
			}
		}
		if (found) results.push_back(isotope1);
	}
	return results;
}

vector<isotope_t>  isotope_t::common_isotope_or_element(const vector<isotope_t>& isotopes1, const vector<isotope_t>& isotopes2)
{
	bool found=false;
	vector<isotope_t> results;
	for (auto& isotope1:isotopes1)
	{
		found = false;
		for (auto& isotope2:isotopes2)
		{
			if (isotope1.nucleons>0 && isotope2.nucleons>0 &&isotope1==isotope2 )
			{
				found = true;
				break;
			}
			else if ((isotope1.nucleons<1 || isotope2.nucleons<1) && isotope1.symbol==isotope2.symbol )
			{
				found = true;
				break;
			}
		}
		if (found) results.push_back(isotope1);
	}
	return results;
}

vector<isotope_t>  isotope_t::not_common_isotope_or_element(const vector<isotope_t>& isotopes1, const vector<isotope_t>& isotopes2)
{
	bool found=false;
	vector<isotope_t> results;
	for (auto& isotope1:isotopes1)
	{
		found = false;
		for (auto& isotope2:isotopes2)
		{
			if (isotope1.nucleons>0 && isotope2.nucleons>0 &&isotope1==isotope2 )
			{
				found = true;
				break;
			}
			else if ((isotope1.nucleons<1 || isotope2.nucleons<1) && isotope1.symbol==isotope2.symbol )
			{
				found = true;
				break;
			}
		}
		if (!found) results.push_back(isotope1);
	}
	return results;
}

/****** OPERATORS **********/
bool isotope_t::operator< (const isotope_t& isotope) const
{
	if (nucleons<isotope.nucleons) return true;
	else if (nucleons==isotope.nucleons && symbol<isotope.symbol) return true;
	return false;
}

bool isotope_t::operator > (const isotope_t& isotope) const
{
	if (nucleons>isotope.nucleons) return true;
	else if (nucleons==isotope.nucleons && symbol>isotope.symbol) return true;
	return false;
}

bool isotope_t::operator == (const isotope_t& isotope) const
{
	if (symbol!=isotope.symbol) return false;
	if (nucleons!=isotope.nucleons) return false;
// 	if (atoms!=isotope.atoms) return false;
	return true;
}

bool isotope_t::operator != (const isotope_t& isotope) const
{
	return (!operator==(isotope));
}
