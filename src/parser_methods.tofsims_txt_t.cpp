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
#include "parser_methods.tofsims_txt_t.hpp"


tofsims_txt_t::tofsims_txt_t() : parser_methods()
{
	/// overwrites parser_methods::tool_name
	tool_name = "tofsims";
	/// overwrites parser_methods::file_type_endings
	file_type_endings = {"TXT"};
}

/// Order is important
bool tofsims_txt_t::parse(filename_t* filename_p, string* contents)
{
	filename = filename_p;
	file_contents = contents;
	if (!check()) 
	{
		error=true;
		error_messages.push_back("tofsims_txt_t::parse:check(): "+ filename->filename()); // Warnings?
		return false;
	}
// 	if (!parse_filename()) 
// 	{
// 		error=true;
// 		error_messages.push_back("tofsims_txt_t::parse:parse_filename(): "+ *filename);
// 		return false;
// 	}
	tools::str::remove_substring_from_mainstring(contents,"#");
	
	if (!parse_cluster())
	{
		error=true;
		error_messages.push_back("tofsims_txt_t::parse:parse_cluster(): "+ filename->filename());
		return false;
	}
	add_global_sputter_time_in_sec_to_crater();
// 	copy_to_measurement_p();
	return true;
}

bool tofsims_txt_t::check() {
	string find_this="# Profile";
	if (file_contents->find(find_this)==string::npos) return false;
// 	if (!tools::mat::find_str_in_mat(file_contents,find_this)) return false;
	return true;
}

vector<isotope_t> tofsims_txt_t::parse_isotopes(string isotopes)
{
	vector<isotope_t> isotope_list;
	isotopes = tools::str::remove_chars_from_string(isotopes,"-+");
	smatch match;
	regex reg ("^\\^?([0-9]{0,3})([A-Z]{1}[a-z]{0,1})([0-9]{0,3})(.*?)$"); 
	while (regex_search(isotopes,match,reg)) 
	{
		// order is important, because it will be cleared on access
		string nucleons = match[1];
		string symbol = match[2];
		string atoms = match[3];
		isotopes = match[4];

		isotope_t isotope;		
		isotope.symbol=symbol;
		if (nucleons!="") isotope.nucleons = tools::str::str_to_int(nucleons);
		else isotope.nucleons = tools::str::extract_int_from_str(PSE.get_isotope_with_highest_abundance(symbol));
		if (atoms!="") isotope.atoms = tools::str::str_to_int(atoms);
		else isotope.atoms=1;
// 		cout << "nucleons="<<nucleons<<"\tsymbol="<<symbol<<"\tatoms="<<atoms<<"\tisotopes="<<isotopes<<endl;
// 		isotope.to_screen("");
		isotope_list.push_back(isotope);
	}
	return isotope_list;
}

bool tofsims_txt_t::parse_cluster()
{
	if (!parse_data_and_header_tensors(/*&raw_data_tensor,&raw_header_tensor*/)) return false;

	if (raw_header_tensor.size()<2) return false;
	if (raw_header_tensor[0].size()<2) return false;
	

	vector<string> cluster_names=raw_header_tensor[0][2]; // [0] should be empty (sputter_time or depth)
	vector<string> dimension_names = raw_header_tensor[1][0];
	tools::str::replace_chars(&raw_data_tensor[1],",",".");
	vector<vector<double>> data_mat = tools::mat::str_matrix_to_double_matrix(raw_data_tensor[1]);
// 	cout << "\ndata_mat.size()="<<data_mat.size()<<endl;
	data_mat = tools::mat::transpose_matrix(data_mat);
	
	if (cluster_names.size()!=dimension_names.size())
	{
		error_messages.push_back("tofsims_txt_t::parse_cluster() - cluster_names.size()!=dimension_names.size() - " + filename->filename());
		return false;
	}
	if (data_mat.size()!=dimension_names.size()-1)
	{
// 		cout << endl;
// 		print(data_mat);
// 		cout << "data_mat.size()="<<data_mat.size()<<endl;
// 		cout << "dimension_names.size()="<<dimension_names.size()<<endl;
		error_messages.push_back("tofsims_txt_t::parse_cluster() - data_mat.size()!=dimension_names.size()-1 - " + filename->filename());
		return false;
	}
	
	// 0th column
	/*
	 * 
	 * creating the crater directly here is special for this parser.method:
	 *		- prohibits interpolation with other measurements
	 * 		- is faster (saves CPU time)
	 * 		- (saves memory - just at parsing time)
	 * 
	 * to align with the other parsers, each parsed quantity_t needs X and Y axis
	 * 
	 * within this parser.method each quantity has only Y axis, because X axis is the same for all quantities
	 * 
	 */
	double max_sputter_time=-1, max_sputter_depth=-1;
	if (dimension_names[0].find("Sputter Time")!=string::npos)
	{
		measurement_p.crater.sputter_time_p.name="sputter_time";
		measurement_p.crater.sputter_time_p.dimension="time";
		measurement_p.crater.sputter_time_p.unit="s";
		measurement_p.crater.sputter_time_p.data=data_mat[0];
		max_sputter_time=measurement_p.crater.sputter_time_p.data.back();
	} 
	else // Depth
	{
		measurement_p.crater.sputter_depth_p.name="sputter_depth";
		measurement_p.crater.sputter_depth_p.dimension="length";
		measurement_p.crater.sputter_depth_p.unit="nm";
		measurement_p.crater.sputter_depth_p.data=data_mat[0];
		max_sputter_depth=measurement_p.crater.sputter_depth_p.data.back();
	}
// 	cluster_t::add_to_cluster_list_if_not_exist(&clusters,"crater",{crater_quantity});
	
	// other columns
	for (int i=1;i<cluster_names.size();i++)
	{
		string cluster_name=cluster_names[i];
		if (cluster_name.length()==0) continue;
		cluster_t cluster;
// 		cluster.name=cluster_name;
		cluster.isotopes=parse_isotopes(cluster_name);
		if (dimension_names[i].find("Intensity")!=string::npos)
		{
			cluster.intensity_p.name="intensity";
			cluster.intensity_p.unit="cnt/s";
			cluster.intensity_p.dimension="amount/time";
			cluster.intensity_p.data=data_mat[i];
// 			if (measurement_p.crater.sputter_depth_p.is_set()) cluster_depth_p=&measurement_p.crater.sputter_depth_p;
// 			if (measurement_p.crater.sputter_time_p.is_set()) cluster.sputter_time_p=&measurement_p.crater.sputter_depth_p;
		}
		else
		{
			cluster.concentration_p.name="concentration";
			cluster.concentration_p.unit="at/cm^3";
			cluster.concentration_p.dimension="amount*(length)^(-3)";
			cluster.concentration_p.data=data_mat[i];
// 			if (measurement_p.crater.sputter_depth_p.is_set()) cluster.sputter_depth_p=&measurement_p.crater.sputter_depth_p;
// 			if (measurement_p.crater.sputter_time_p.is_set()) cluster.sputter_time_p=&measurement_p.crater.sputter_time_p;
		}
		measurement_p.clusters[cluster.name()]=cluster;
	}
	if (max_sputter_time>0)
	{
		measurement_p.crater.total_sputter_time_p.name="total_sputter_time";
		measurement_p.crater.total_sputter_time_p.dimension="time";
		measurement_p.crater.total_sputter_time_p.unit="s";
		measurement_p.crater.total_sputter_time_p.data={max_sputter_time};
	}
// 	else
// 	{
// 		measurement_p.crater.total_sputter_depths.name="total_sputter_depth";
// 		measurement_p.crater.total_sputter_depths.dimension="length";
// 		measurement_p.crater.total_sputter_depths.unit="nm";
// 		measurement_p.crater.total_sputter_depths.data={max_sputter_depth};
// 	}
	return true;
}


