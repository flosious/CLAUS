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
#ifndef ISOTOPE_T_HPP
#define ISOTOPE_T_HPP

#include <vector>
#include <regex>
#include "quantity.hpp"
#include <utility>
#include <regex>
#include <set>
#include "pse.hpp"

using namespace std;


class isotope_t 
{
	friend class parser_methods;
private:
public:
	/// gernates sth like "74Ge1 31P1"
	string name();
	/// parse
// 	static isotope_t parse(string isotope);
	/// Si in 30Si2
	string symbol="NULL";
	/// default=-1
	int nucleons=-1;
	/// 2 in 30Si2
	int atoms=1;
	quantity_t abundance();
	quantity_t mass();
// 	bool is_cluster=false;
	void to_screen(string prefix="");
	/// sort function for name()
	static bool sort_lower_mass(isotope_t i, isotope_t j);
	bool is_set();
	isotope_t();
	isotope_t(string name_to_parse);
	
	/// returns the not-common-isotopes from both vectors
	static vector<isotope_t>  not_common(const vector<isotope_t>& isotopes1, const vector<isotope_t>& isotopes2);
	static vector<isotope_t>  not_common_isotope_or_element(const vector<isotope_t>& isotopes1, const vector<isotope_t>& isotopes2);
	/// returns the common-isotopes from both vectors
	static vector<isotope_t> common(const vector<isotope_t>& isotopes1, const vector<isotope_t>& isotopes2);
	static vector<isotope_t> common_isotope_or_element(const vector<isotope_t>& isotopes1, const vector<isotope_t>& isotopes2);
	static vector<isotope_t>  not_common_elements(const vector<isotope_t>& elements1, const vector<isotope_t>& elements2);
	static vector<isotope_t>  common_elements(const vector<isotope_t>& elements1, const vector<isotope_t>& elements2);
	
	
	/*OPERATOR*/
	
	bool operator <(const isotope_t &isotope) const;
	bool operator >(const isotope_t &isotope) const;
	bool operator ==(const isotope_t &isotope) const;
	bool operator !=(const isotope_t &isotope) const;
};



#endif // ISOTOPE_T_HPP
