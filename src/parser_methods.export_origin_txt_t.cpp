/*
	Copyright (C) 2020 Florian Bärwolf

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
// #include "parser_methods.hpp"
// 
// export_origin_txt_t::export_origin_txt_t()
// {
// 	file_type_endings_parent=file_type_endings;
// }
// 
// bool export_origin_txt_t::check() {
// 	//rechteckige matrizen
// // 	print(raw_data_tensor);
// // 	print(raw_header_tensor);
// 	if (raw_data_tensor.size()!=1 || raw_header_tensor.size()!=1 || raw_data_tensor[0][0].size()!=raw_header_tensor[0][0].size()) return false;
// 	return true;
// }
// 
// vector<isotope_t> export_origin_txt_t::parse_isotopes(string isotopes)
// {
// 	vector<isotope_t> isotope_list;
// 	isotopes = tools::str::remove_chars_from_string(isotopes,"-+");
// 	smatch match;
// 	regex reg ("^\\\\+\\(?([0-9]{0,3})\\)([A-Z]{1}[a-z]{0,1})([0-9]{0,3})(.*?)$"); 
// 	while (regex_search(isotopes,match,reg)) 
// 	{
// 		// order is important, because it will be cleared on access
// 		string nucleons = match[1];
// 		string symbol = match[2];
// 		string atoms = match[3];
// 		isotopes = match[4];
// 	
// 		isotope_t isotope;		
// 		isotope.symbol=symbol;
// 		if (nucleons!="") isotope.nucleons = tools::str::str_to_int(nucleons);
// 		else isotope.nucleons = tools::str::extract_int_from_str(PSE.get_isotope_with_highest_abundance(symbol));
// 		if (atoms!="") isotope.atoms = tools::str::str_to_int(atoms);
// 		else isotope.atoms=1;
// // 		cout << "nucleons="<<nucleons<<"\tsymbol="<<symbol<<"\tatoms="<<atoms<<"\tisotopes="<<isotopes<<endl;
// // 		isotope.to_screen("");
// 		isotope_list.push_back(isotope);
// 	}
// // 	cout << "OUT\n";
// 	return isotope_list;
// }
// 
// bool export_origin_txt_t::parse_cluster()
// {
// 	if (raw_header_tensor.size()!=1) return false;
// 	if (raw_data_tensor.size()!=1) return false;
// 	if (raw_header_tensor[0].size()==0) return false;
// 	if (raw_data_tensor[0].size()==0) return false;
// 
// 	vector<string> isotopes_with_cluster_names=raw_header_tensor[0][0];
// 	vector<string> units = raw_header_tensor[0][1];
// 	tools::str::replace_chars(&raw_data_tensor[0],",",".");
// 	data_columns = tools::mat::transpose_matrix(raw_data_tensor[0]);
// 
// 	// other columns
// 	for (int i=0;i<isotopes_with_cluster_names.size();i++)
// 	{
// 		string isotopes_with_cluster_name=isotopes_with_cluster_names[i];
// 		if (isotopes_with_cluster_name.length()==0) continue;
// 		cluster_t cluster;
// // 		cluster.name=isotopes_with_cluster_name;
// 		quantity_t quantity;
// 		if (isotopes_with_cluster_name.find("intensity")!=string::npos && data_columns[i].size()>1)
// 		{
// 			quantity.name="intensity";
// 			quantity.unit="cnt/s";
// 			quantity.dimension="amount/time";
// 			quantity.data=tools::mat::str_vec_to_double_vec(data_columns[i]);
// 		}
// 		else if (isotopes_with_cluster_name.find("concentration")!=string::npos && data_columns[i].size()>1)
// 		{
// 			quantity.name="concentration";
// 			quantity.unit="at/cm3";
// 			quantity.dimension="amount*(length)^(-3)";
// 			quantity.data=tools::mat::str_vec_to_double_vec(data_columns[i]);
// 		}
// 		else if (isotopes_with_cluster_name.find("total_sputter_depth")!=string::npos)
// 		{
// // 			cluster.name="crater";
// 			quantity.name="total_sputter_depth";
// 			quantity.unit="nm";
// 			quantity.dimension="length";
// 			quantity.data=tools::mat::str_vec_to_double_vec(data_columns[i]);
// 			crater.total_sputter_depths=quantity;
// 		}
// 		else if (isotopes_with_cluster_name.find("total_sputter_time")!=string::npos)
// 		{
// // 			cluster.name="crater";
// 			quantity.name="total_sputter_time";
// 			quantity.unit="s";
// 			quantity.dimension="time";
// 			quantity.data=tools::mat::str_vec_to_double_vec(data_columns[i]);
// 			crater.total_sputter_time=quantity;
// 		}
// 		else if (isotopes_with_cluster_name.find("sputter_depth")!=string::npos)
// 		{
// // 			cluster.name="crater";
// 			quantity.name="sputter_depth";
// 			quantity.unit="nm";
// 			quantity.dimension="length";
// 			quantity.data=tools::mat::str_vec_to_double_vec(data_columns[i]);
// 			crater.sputter_depth=quantity;
// 		}
// 		else if (isotopes_with_cluster_name.find("sputter_time")!=string::npos)
// 		{
// // 			cluster.name="crater";
// 			quantity.name="sputter_time";
// 			quantity.unit="s";
// 			quantity.dimension="time";
// 			quantity.data=tools::mat::str_vec_to_double_vec(data_columns[i]);
// 			crater.sputter_time=quantity;
// 		}
// 		else continue;
// 		
// 		tools::str::replace_chars(&isotopes_with_cluster_name,quantity.name,"");
// 		cluster.isotopes=parse_isotopes(isotopes_with_cluster_name);
// 		if (cluster.isotopes.size()>0)
// 		{
// 			clusters[cluster.name()]=cluster;
// 		}
// 	}
// 	return true;
// }
// 
// /// Order is important
// bool export_origin_txt_t::parse(string* filename_p, string* contents)
// {
// 	filename = filename_p;
// 	file_contents = contents;
// 	parse_data_and_header_tensors();
// 	if (!check()) 
// 	{
// // 		error=true;
// 		error_messages.push_back(*filename + ": is not export_origin_txt_t"); // Warnings?
// 		return false;
// 	}
// 	if (!parse_cluster()) 
// 	{
// // 		cout << "could NOT parse cluster\n";
// 		return false;
// 	}
// // 	cout << "everything parsed\n";
// 	parse_id_and_crater_depths();
// // 	if (crater_depths.data.size()>0) cluster_t::add_to_cluster_list_if_not_exist(&clusters,"crater",{crater_depths});
// // // 	set_ids();
// 	id.tool = tool_name;
// // // 	add_crater_to_clusters();
// 	return true;
// }
