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
#include "filename_t.hpp"

filename_t::filename_t(string filename_p, string delimiter_p)
{
	filename_with_path_p = filename_p;
	delimiter = delimiter_p;
	parse_filename_parts();
}

std::__cxx11::string filename_t::type_ending(std::__cxx11::string delimiter)
{
	return tools::file::extract_filetype_ending(filename_with_path_p,delimiter);
}

bool filename_t::is_error()
{
	return error;
}

string filename_t::filename_with_path()
{
	return filename_with_path_p;
}

string filename_t::filename()
{
	return tools::file::extract_filename(filename_with_path_p);
}

string filename_t::directory()
{
	return tools::file::extract_directory_from_filename(filename_with_path_p);
}

string filename_t::filename_without_crater_depths()
{
	string filename_wo_crater_depths  = tools::file::extract_filename(filename_with_path_p);
	for (string crater_depth_string_part:crater_depth_string_parts)
	{
		tools::str::remove_substring_from_mainstring(&filename_wo_crater_depths ,crater_depth_string_part+delimiter);
		tools::str::remove_substring_from_mainstring(&filename_wo_crater_depths ,delimiter+crater_depth_string_part);
	}
	return filename_wo_crater_depths;
}


bool filename_t::parse_filename_parts()
{
	vector<string> filename_parts = tools::str::get_strings_between_delimiter(filename(),delimiter);
	for (auto filename_part:filename_parts)
	{
		// parse everything just once (except crater depths)
		// THE ORDER IS VERY IMPORTANT
		if (olcdb==-1) if(parse_olcdb(filename_part)) continue;
		if (wafer==-1) if(parse_wafer(filename_part)) continue;
		if (lot=="") if(parse_lot(filename_part)) continue;
		if (chip_x==-1)	if(parse_chip(filename_part)) continue;
		if (monitor=="") if(parse_monitor(filename_part)) continue;
		if (group==-1) if(parse_group(filename_part)) continue;
		if (!sputter_energy.is_set()) if (parse_sputter_energy(filename_part)) continue;
		if (parse_crater_depth(filename_part)) continue;
		// no parser worked
		not_parseable_filename_parts.push_back(filename_part);
	}
	return true;
}

bool filename_t::parse_monitor(string filename_part)
{
// 	string x,y;
	smatch match;
	regex reg1 ("^m-*([a-zA-Z0-9]+)$"); 
	regex reg2 ("^monitor-([a-zA-Z0-9]+)$"); 
	if (regex_search(filename_part,match,reg1) || regex_search(filename_part,match,reg2)) 
	{
		monitor = match[1];
		return true;
	}
	return false;
}

bool filename_t::parse_chip(string filename_part)
{
// 	string x,y;
	smatch match;
	regex reg1 ("^x([0-9]{1,2})y([0-9]{1,2})$"); 
	regex reg2 ("^chip-([0-9]{1,2})-([0-9]{1,2})$"); 
	regex reg3 ("^c([0-9]{1,2})-([0-9]{1,2})$"); 
	if (regex_search(filename_part,match,reg1) || regex_search(filename_part,match,reg2) || regex_search(filename_part,match,reg3)) 
	{
		chip_x = tools::str::str_to_int(match[1]);
		chip_y = tools::str::str_to_int(match[2]);
		return true;
	}
	return false;
}

bool filename_t::parse_olcdb(string filename_part)
{
	smatch match;
	regex reg ("^([0-9]{4,})$"); 
	if (regex_search(filename_part,match,reg)) 
	{
		olcdb = tools::str::str_to_int(filename_part);
		return true;
	}
	return false;
}

bool filename_t::parse_sputter_energy(string filename_part)
{
	smatch match;
	
	regex reg; 
	
	sputter_energy.name = "sputter_energy";
	sputter_energy.unit="eV";
	sputter_energy.dimension="energy";
	
	reg = ("^([0-9]{1,2})(kV|keV)(O|Cs)(\\+|-)$"); 
	if (regex_search(filename_part,match,reg)) 
	{
		sputter_energy.data.push_back(tools::str::str_to_double(match[1])*1000);
		sputter_element = match[3];
		polarity = match[4];
		return true;
	}
	
	reg = ("^([0-9]{2,5})(V|eV)(O|Cs)(\\+|-)$"); 
	if (regex_search(filename_part,match,reg)) 
	{
		sputter_energy.data.push_back(tools::str::str_to_double(match[1]));
		sputter_element = match[3];
		polarity = match[4];
		return true;
	}
	
	return false;
}

bool filename_t::parse_lot(string filename_part)
{
	smatch match;
	regex reg ("^([A-Z]{1,4}[0-9]{1,5})([#[0-9A-Za-z]*?]*?)$"); 
	if (regex_search(filename_part,match,reg)) 
	{
		lot = match[1];
		lot_split = match[2];
		return true;
	}
	return false;
}

bool filename_t::parse_wafer(string filename_part)
{
	smatch match;
	regex reg ("^[wW]?([0-9]{1,2})$"); 
	if (regex_search(filename_part,match,reg)) 
	{
		wafer = tools::str::str_to_int(match[1]);
		return true;
	}
	return false;
}

bool filename_t::parse_group(string filename_part)
{
	smatch match;
	regex reg ("^([0-9]{1,2})([a-z]*?)$"); 
	if (regex_search(filename_part,match,reg)) 
	{
		group = tools::str::str_to_int(match[1]);
		repition = match[2];
		return true;
	}
	regex reg1 ("^g-?*([0-9]+?)([a-z]*?)$"); 
	regex reg2 ("^group-*([0-9]+?)([a-z]*?)$"); 
// 	regex reg3 ("^g([0-9]+?)([a-z]*?)$"); 
	if (regex_search(filename_part,match,reg1) || regex_search(filename_part,match,reg2) /*|| regex_search(filename_part,match,reg3)*/) 
	{
		group = tools::str::str_to_int(match[1]);
		repition = match[2];
		return true;
	}
	return false;
}

bool filename_t::parse_crater_depth(string filename_part)
{
	total_sputter_depths.name="total_sputter_depth";
	total_sputter_depths.dimension="length";
	total_sputter_depths.unit="nm";
	
	smatch match;
	regex reg ("^([0-9]{2,})(nm|A)$"); 
	if (regex_search(filename_part,match,reg)) 
	{
		string value = match[1];
		string unit = match[2];
		crater_depth_string_parts.push_back(value+unit);
		if (unit=="nm") total_sputter_depths.data.push_back(tools::str::str_to_double(value));
		else if (unit=="A") total_sputter_depths.data.push_back(tools::str::str_to_double(value)/10); // A in nm
		return true;
	}
	return false;
}

void filename_t::to_screen(string prefix)
{
	
	cout<< prefix << "filename=\t"<< filename()<<endl;
	cout<< prefix << "filename_with_path=\t"<< filename_with_path()<<endl;
	cout << prefix<< "directory=\t"<< directory()<<endl;
	cout << prefix<< "filename_without_crater_depths=\t"<< filename_without_crater_depths()<<endl;
	
	cout<< prefix << "wafer=\t"<< wafer<<endl;
	cout << prefix<< "lot=\t"<< lot<<endl;
	cout << prefix<< "lot_split=\t"<< lot_split<<endl;
	cout<< prefix << "chip_x=\t"<< chip_x<<endl;
	cout << prefix<< "chip_y=\t"<< chip_y<<endl;
	cout << prefix<< "monitor=\t"<< monitor<<endl;

	cout << prefix<< "olcdb=\t"<< olcdb<<endl;
	cout << prefix<< "group=\t"<< group<<endl;
	cout << prefix<< "repition=\t"<< repition<<endl;
	
	cout << prefix<< "polarity=\t"<< polarity<<endl;
	cout << prefix<< "sputter_element=\t"<< sputter_element<<endl;
	
	if (sputter_energy.is_set()) sputter_energy.to_screen(prefix);
	if (total_sputter_depths.is_set()) total_sputter_depths.to_screen(prefix);
	
	cout << prefix<< "not_parseable_filename_parts:" << endl;
	print(not_parseable_filename_parts);
}

std::__cxx11::list< filename_t > filename_t::parse(std::vector< std::__cxx11::string > filenames_with_path, std::__cxx11::string delimiter)
{
	list<filename_t> filenames;
	for (auto& fname:filenames_with_path)
		filenames.push_back(filename_t(fname,delimiter));
	return filenames;
}

