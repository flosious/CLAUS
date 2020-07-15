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
#ifndef SAMPLE_T_HPP
#define SAMPLE_T_HPP

#include <vector>
#include "filename_t.hpp"
#include "quantity.hpp"
#include "isotope.hpp"
#include <list>
#include "database_t.hpp"
#include <map>
#include "isotope.hpp"

// #include "measurement_group_t.hpp"
// #include "measurement.hpp"



class sample_t
{
private:
	int wafer_p;
	string lot_p;
	string lot_split_p;
	string monitor_p;
	int chip_x_p;
	int chip_y_p;
	
protected:
	
public:
	
	class matrix_t
	{
	protected:
		string name_p;
		vector<isotope_t> isotopes_p;
	public:
		/// returns isotope / sum(isotopes)
		quantity_t relative_concentration(const isotope_t& isotope) const;
		vector<isotope_t> isotopes() const;
		/// Si40 Ge60 no points!
		bool set_isotopes_from_name(string name);
		int get_atoms_from_isotope(isotope_t isotope);
// 		vector<isotope_t> isotopes();
		string name();
		bool operator==(const matrix_t& matrix);
		bool operator!=(const matrix_t& matrix);
		bool operator<(const matrix_t& matrix) const;
	};
	
	matrix_t matrix;
	
	bool load_from_database();
	bool save_to_database();
	
	/// merges all information to one string
	string name();
	
	int wafer();
	string lot();
	string lot_split();
	string monitor();
	int chip_x();
	int chip_y();
	
// 	vector<string> substrat_element_symbols;
	/// implantation dose
	map<isotope_t,quantity_t> doses_p;
	
	void to_screen(string prefix="");
	
	sample_t(filename_t* filename_p);
	static list<sample_t> samples(list<filename_t> *filenames);
	
	/*OPERATORS*/
	bool operator == (sample_t& sample);
	bool operator != (sample_t& sample);
	bool operator < (sample_t& sample);
	
};

#endif // SAMPLE_T_HPP
