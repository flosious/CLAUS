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
#include "parser_methods.reference_t.hpp"

vector<string> reference_t::possible_dimensions={"sputter_depth","sputter_time", "sputter_current", "concentration", "intensity"};

reference_t::reference_t()
{
	/// overwrites parser_methods::tool_name
	tool_name = "dsims";
	/// overwrites parser_methods::file_type_endings
	file_type_endings = {"txt"};
}

bool reference_t::check() {
	string find_this="reference_dsims";
	if (file_contents->find(find_this)==string::npos) return false;
	return true;
}

vector<isotope_t> reference_t::parse_isotopes(string cluster_name)
{
	cluster_name = tools::str::remove_chars_from_string(cluster_name,"\\+()");
	vector<isotope_t> isotopes;
	vector<string> single_isotopes= tools::str::get_strings_between_delimiter(cluster_name," ");
	for (string single_isotope:single_isotopes)
	{
		isotopes.push_back(isotope_t(single_isotope));
	}
	return isotopes;
}

bool reference_t::parse(filename_t* filename_p, string* contents)
{
	filename = filename_p;
	file_contents = contents;
	if (!check()) return false;
	if (!parse_data_and_header_tensors()) return false;
	if (raw_data_tensor.size()!=1) return false;
	if (raw_header_tensor.size()!=1) return false;
	if (!parse_cluster()) return false;
	return true;
}

void reference_t::column_t::to_screen()
{
	cout << "cluster_name=\t" << cluster_name << endl;
	cout << "dimension=\t" << dimension << endl;
	cout << "unit=\t" << unit << endl;
	cout << "data.size()=\t" << data.size() << endl;
}


vector<reference_t::column_t> reference_t::columns()
{
	vector<vector<string>> data_cols_lines = tools::mat::transpose_matrix(raw_data_tensor[0]);
	vector<vector<string>> header_cols_lines = tools::mat::transpose_matrix(raw_header_tensor[0]);
	vector<reference_t::column_t> cols;
	for (int c=0;c<data_cols_lines.size();c++)
	{
		if (header_cols_lines[c].size()==4) cols.push_back(column_t(header_cols_lines[c],data_cols_lines[c]));
	}
	return cols;
}

/*
 *	reference_dsims
 *	sputter_depth
 *	nm
 *	crater_sample_name --> can be ignored
 *	data[]
 */
reference_t::column_t::column_t(vector<string>& header_lines, vector<string>& data_lines)
{
	for (auto& possible_dimension:possible_dimensions)
	{
		if (header_lines[1].find(possible_dimension)!=string::npos) 
		{
			dimension = possible_dimension;
			cluster_name = header_lines[1];
			tools::str::remove_substring_from_mainstring(&cluster_name,dimension);
			unit = header_lines[2];
			data = tools::mat::str_vec_to_double_vec(data_lines);
		}
	}
}

bool reference_t::parse_cluster()
{
	vector<reference_t::column_t> cols = columns();
	for (auto col:cols)
	{
		if (col.dimension == "sputter_depth")
		{
			measurement_p.crater.sputter_depth_p.unit=col.unit;
			measurement_p.crater.sputter_depth_p.dimension = "length";
			measurement_p.crater.sputter_depth_p.name = col.dimension;
			measurement_p.crater.sputter_depth_p.data = col.data;
		}
		else if (col.dimension == "sputter_time")
		{
			measurement_p.crater.sputter_depth_p.unit=col.unit;
			measurement_p.crater.sputter_depth_p.dimension = "time";
			measurement_p.crater.sputter_depth_p.name = col.dimension;
			measurement_p.crater.sputter_depth_p.data = col.data;
		} 
		else if (col.dimension=="sputter_current")
		{
			measurement_p.crater.sputter_current_p.unit = col.unit;
			measurement_p.crater.sputter_current_p.data = col.data;
			measurement_p.crater.sputter_current_p.dimension = "e_current";
			measurement_p.crater.sputter_current_p.name=col.dimension;
		}
		else
		{
			cluster_t cluster;
			cluster.isotopes = parse_isotopes(col.cluster_name);
			if (cluster.name()=="NULL") continue;
			if (measurement_p.clusters.find(cluster.name())!=measurement_p.clusters.end())
				cluster = measurement_p.clusters[cluster.name()];
			if (col.dimension=="intensity")
			{
				cluster.intensity_p.unit = col.unit;
				cluster.intensity_p.dimension = "amount/time";
				cluster.intensity_p.name = col.dimension;
				cluster.intensity_p.data = col.data;
			}
			else
			{
				cluster.concentration_p.unit = col.unit;
				cluster.concentration_p.dimension = "amount*(length)^(-3)";
				cluster.concentration_p.name = col.dimension;
				cluster.concentration_p.data = col.data;
			}
			measurement_p.clusters[cluster.name()]=cluster;
		}
	}
	return true;
}
