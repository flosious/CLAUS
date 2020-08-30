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
#include "pse.hpp"


/// global variable
pse_t PSE;

vector<string> pse_t::file_locations;

quantity_t pse_t::get_abundance_from_isotope(string isotope)
{
	quantity_t result;
// 	result.is_data=false;
	auto it = isotopes_abundance.find(isotope);
	if (it==isotopes_abundance.end()) return result;
	return isotopes_abundance[isotope];
}

quantity_t pse_t::get_mass_from_isotope(string isotope)
{
	quantity_t result;
// 	result.is_data=false;
	auto it = isotopes_mass.find(isotope);
	if (it==isotopes_mass.end()) return result;
	return isotopes_mass[isotope];
}

vector<string> pse_t::get_isotopes_from_element(string element) 
{
	vector<string> result;
	auto it = elements_isotopes.find(element);
	if (it==elements_isotopes.end()) return result;
	return elements_isotopes[element];
}

vector<string> pse_t::get_all_elements()
{
	return list_elements;
}

vector<string> pse_t::get_all_isotopes()
{
	if (list_isotopes.size()==0) 
	{
		vector<string> list_isotopes_p;
		for (int i=0;i<get_all_elements().size();i++)
		{
			list_isotopes_p = get_isotopes_from_element(get_all_elements()[i]);
			list_isotopes.insert(list_isotopes.end(),list_isotopes_p.begin(),list_isotopes_p.end());
		}
	}
	return list_isotopes;
}

string pse_t::get_isotope_with_highest_abundance(string element)
{
	double highest_abundance=0;
	string isotope_with_highest_abundance="";
	vector<string> isotopes = get_isotopes_from_element(element);
	for (auto const& isotope:isotopes)
	{
		if (get_abundance_from_isotope(isotope).is_data() && get_abundance_from_isotope(isotope).data[0]>highest_abundance) 
		{
			highest_abundance = get_abundance_from_isotope(isotope).data[0];
			isotope_with_highest_abundance=isotope;
		}
	}
	return isotope_with_highest_abundance;
}

vector<string> pse_t::get_all_isotopes_with_highest_abundance()
{
	vector<string> isotopes_with_highest_abundance;
	for (auto const& element:get_all_elements())
	{
		isotopes_with_highest_abundance.push_back(get_isotope_with_highest_abundance(element));
	}
	return isotopes_with_highest_abundance;
}

int pse_t::load()
{
	int loaded=0;
	if (file_locations.size()==0) return load_file(default_filename);
	for (auto const& file: file_locations) 
	{
		loaded=loaded+load_file(file);
	}
	return loaded;
}

int pse_t::load_file(string filename) 
{
	vector<vector<string>> contents = tools::file::load(filename,delimiter);
	if (contents.size()==0) return 0;
	tools::mat::remove_empty_cols_from_matrix(&contents);
	tools::mat::remove_empty_lines_from_matrix(&contents);
	tools::str::remove_spaces(&contents);
	tools::file::write_mat_to_file(filename,&contents,false,delimiter,"\n");
	string symbol, nuclide, mass, abundance, symbol_alternative;
	for (int i=0;i<contents.size();i++) 
	{
		
		if (contents[i].size()>4)
		{
			symbol_alternative="";
			if (contents[i][0]!="") 
			{
				symbol=contents[i][1];
				tools::str::remove_spaces(&symbol);
				list_elements.push_back(symbol);
			}
			if (contents[i][0]=="" && contents[i][1]!="") 	symbol_alternative=contents[i][1];
			nuclide = contents[i][2];
			mass = contents[i][3];
			abundance = contents[i][4];
			abundance = tools::str::remove_chars_from_string(abundance,"()");
			
			/*abundance*/
			
			quantity_t isotope_a;
			isotope_a.name="abundance";
// 			isotope_a.is_data=true;
// 			isotope_a.is_skalar_data=true;
			isotope_a.dimension="";
			isotope_a.unit="";
			if (abundance.length()==0) isotope_a.data.push_back(0);
			else isotope_a.data.push_back(tools::str::str_to_double(abundance));

			isotopes_abundance[nuclide+symbol] = isotope_a;
			if (symbol_alternative!="") isotopes_abundance[symbol_alternative] = isotope_a; // D->2H and T->3H
			
			
			/*mass*/
			mass =tools::str::remove_chars_from_string(mass,"()");
			isotope_a.name="mass";
// 			isotope_a.is_data=true;
// 			isotope_a.is_skalar_data=true;
			isotope_a.dimension="mass";
			isotope_a.unit="u";
			isotope_a.data[0]=tools::str::str_to_double(mass);
			isotopes_mass[nuclide+symbol] = isotope_a;
			
			elements_isotopes[symbol].push_back(nuclide+symbol);
// 			list_isotopes.push_back(nuclide+symbol);
		}
	}
	add_isotopes_with_highest_abundance_to_maps();
	if (get_all_elements().size()==0) return 0;
	return 1;
}


void pse_t::add_isotopes_with_highest_abundance_to_maps() 
{
	/*iterate all elements and get their isotopes: e.g. 28Si,29Si,30Si for Si*/
	for (auto const& it:elements_isotopes)
	{
		string element = it.first;
		vector<string> isotopes = it.second;
		string isotope_with_highest_abundance = get_isotope_with_highest_abundance(element);
		isotopes_abundance[element] = get_abundance_from_isotope(isotope_with_highest_abundance);
		isotopes_mass[element] = get_mass_from_isotope(isotope_with_highest_abundance);
	}
	return;
}

void pse_t::print_t() 
{
	get_all_isotopes();
	for (int i=0;i<list_isotopes.size();i++)
	{
		print_t(list_isotopes[i]);
	}
	return;
}

void pse_t::print_t(string isotope) 
{
	quantity_t abundance = get_abundance_from_isotope(isotope);
	quantity_t mass = get_mass_from_isotope(isotope);
	if (!abundance.is_data() || !mass.is_data()) return;
		
	cout << isotope << ":\t" << "abundance=" << abundance.data[0] << "[" << abundance.unit << "]";
	cout << "\t" << "mass=" << mass.data[0] << "[" << mass.unit << "]" << endl;;
	
	return;
}

// vector<isotope_t> get_all()
// {
// 	vector<isotope_t> isotopes;
// 	
// 	
// 	
// 	return isotopes;
// }



