#include "parser_methods.xps_csv_t.hpp"


xps_csv_t::xps_csv_t()
{
	/// overwrites parser_methods::tool_name
	tool_name = "versaprobe";
	/// overwrites parser_methods::file_type_endings
	file_type_endings = {"csv"};
}

bool xps_csv_t::parse(filename_t* filename_p, string* contents)
{
	filename = filename_p;
	file_contents = contents;

	if (!parse_data_and_header_tensors(",")) 
	{
		error_messages.push_back(filename->filename_with_path() + ": xps_csv_t::parse:parse_data_and_header_tensors()"); // Warnings?
		return false;
	}

	if (!parse_clusters())
	{
		error_messages.push_back(filename->filename_with_path() + ": xps_csv_t::parse:parse_clusters()"); // Warnings?
		return false;
	}
// 	copy_to_measurement_p();
	return true;
}

bool xps_csv_t::parse_elements()
{
	if (raw_data_tensor.size()<2) return false;
	if (raw_header_tensor.size()<2) return false;
// 	cout << "raw_data_tensor.size()=" << raw_data_tensor.size() << endl;
// 	cout << "raw_header_tensor.size()=" << raw_header_tensor.size() << endl;
	
	if (raw_data_tensor.size()<raw_header_tensor.size()) return false;
	
	for (int i=0;i<raw_header_tensor.size();i++)
	{
		if (raw_header_tensor[i].size()!=2) continue;
		element_t element;
		element.name = raw_header_tensor[i][1][0];
		if (raw_data_tensor.size()<i) return false;
		if (i+1!=raw_data_tensor.size()-1) raw_data_tensor[i+1].erase(raw_data_tensor[i+1].end()-1,raw_data_tensor[i+1].end());
		/*
		 * we have to asume that the input values are sputter_time and concentration,
		 * because there is no information in the exported data (no unit, dimension, ...)
		 */
		element.sputter_time.data= tools::mat::str_vec_to_double_vec(tools::mat::transpose_matrix(raw_data_tensor[i+1])[0]);
		element.sputter_time.data.erase(element.sputter_time.data.begin(),element.sputter_time.data.begin()+1);
		element.sputter_time.unit="s";
		element.sputter_time.dimension="time";
		element.sputter_time.name="sputter_time";
		
		element.concentration.name="concentration";
		element.concentration.unit="at%";
		element.concentration.dimension="relative";
		element.concentration.data=tools::mat::str_vec_to_double_vec(tools::mat::transpose_matrix(raw_data_tensor[i+1])[1]);
		elements.push_back(element);
	}
	if (elements.size()==0) return false;
	return true;
}

bool xps_csv_t::parse_clusters()
{
	if (!parse_elements()) return false;
	
	
	cluster_t cluster;
	for (auto& element:elements)
	{
		cluster.name_p=element.name;
		cluster.concentration_p=element.concentration;
		measurement_p.clusters.insert(pair<string,cluster_t>(element.name,cluster));
	}
	measurement_p.crater.sputter_time_p=elements[0].sputter_time;
	
	return true;
}
