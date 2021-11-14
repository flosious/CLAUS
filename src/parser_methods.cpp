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
#include "parser_methods.hpp"
#include "cluster_t.hpp"

bool parser_methods::use_impulse_filter_on_data=true;

parser_methods::~parser_methods()
{
	/// geht so nicht, wird zu spät gecalled
// 	if (!error) measurement_p.tool_name_p = tool_name;
// 	cout << "tool_name = " << tool_name << endl;
// 	cout << "measurement_p.tool_name_p = " << measurement_p.tool_name_p << endl;
}

parser_methods::parser_methods()
{
}

void parser_methods::set_secondary_measurement()
{
	measurement_p.secondary=true;
}

bool parser_methods::check_file_type()
{
// 	string parsed_file_type_ending = tools::str::get_strings_between_delimiter(*filename,".").back();
	for (auto const& file_type_ending:file_type_endings)
	{
		if (filename->type_ending() == file_type_ending) return true;
	}
	return false;
}

void parser_methods::copy_secondaries_and_delete_from_list(list<measurement_t>* measurements)
{
	for (list<measurement_t>::iterator it_A=measurements->begin();it_A!=measurements->end();++it_A)
	{
		for (list<measurement_t>::iterator it_B=measurements->begin();it_B!=measurements->end();++it_B)
		{			
			if (add_secondary_to_primary(&(*it_A),&(*it_B)))
			{
				measurements->erase(it_B);
				// reset, because iterator now is invalid
				it_A=measurements->begin();
				it_B=measurements->begin();
			} 
		}
	}
// 	cout << "measurements->size(): " << measurements->size() << endl;
}


bool parser_methods::add_secondary_to_primary(measurement_t* primary, measurement_t* secondary)
{

	if (!secondary->is_secondary()) return false;
	if (primary->is_secondary()) return false;
	if (primary->filename_p->filename_without_crater_depths()!=secondary->filename_p->filename_without_crater_depths()) return false;
	if (primary->filename_p->directory()!=secondary->filename_p->directory()) return false;

// 	if (*primary != *secondary) return false;
	
	/*COPY important values*/
	
	for (auto cluster:secondary->clusters)
	{
		// add cluster to map, if it is not already in there
		if (primary->clusters.find(cluster.first)==primary->clusters.end())	primary->clusters[cluster.first]=cluster.second;
	}
	
	if (primary->crater.sputter_depth_p.data.size()==0) primary->crater.sputter_depth_p=secondary->crater.sputter_depth_p;
	if (primary->crater.sputter_time_p.data.size()==0) primary->crater.sputter_time_p=secondary->crater.sputter_time_p;
// 	if (primary->crater.sputter_rate_p.data.size()==0) primary->crater.sputter_rate_p=secondary->crater.sputter_rate_p;
	if (primary->crater.total_sputter_time_p.data.size()==0) primary->crater.total_sputter_time_p=secondary->crater.total_sputter_time_p;
	
	// combine total_sputter_depths
	if (!primary->filename_p->total_sputter_depths.is_set()) primary->filename_p->total_sputter_depths = secondary->filename_p->total_sputter_depths;
	else primary->filename_p->total_sputter_depths.add_to_data(secondary->filename_p->total_sputter_depths);
	
	primary->crater.linescans.insert(primary->crater.linescans.begin(),secondary->crater.linescans.begin(),secondary->crater.linescans.end());
	
	return true;
}


measurement_t parser_methods::measurement()
{
	measurement_p.filename_p=filename;
	measurement_p.settings.filename=filename;
	measurement_p.crater.filename=filename;
	if (!error) measurement_p.tool_name_p = tool_name;
	for (auto& cluster:measurement_p.clusters)
		cluster.second.measurement=&measurement_p;
	return measurement_p;
}

unordered_set<string> parser_methods::get_file_type_endings()
{
	return file_type_endings;
}

vector<string> parser_methods::get_error_messages()
{
	return error_messages;
}

string parser_methods::get_tool_name()
{
	return tool_name;
}

bool parser_methods::is_error()
{
	return error;
}

bool parser_methods::parse_data_and_header_tensors(string line_delimiter) 
{
	raw_data_tensor.clear();
	raw_header_tensor.clear();
	vector<vector<string>> raw_mat = tools::mat::format_string_to_matrix(file_contents,"\n",line_delimiter);
// 	cout << "raw_mat.size()1 = " << raw_mat.size() << endl;
// 	cout << "raw_mat[1].size()1 = " << raw_mat[1].size() << endl;
	tools::mat::remove_empty_lines_from_matrix(&raw_mat);
	vector<vector<string> > header_temp, data_temp;
	bool data_scanned=false;
	bool header_scanned=false;
	if (raw_mat.size()<1) return false;
	int check_number;
	for (int i=0;i<raw_mat.size();i++) {
		check_number=1;
		for (int j=0;j<(raw_mat.at(i)).size();j++) {
		  if ((raw_mat.at(i)[j].size()>0) && tools::str::is_number((raw_mat).at(i)[j])!=1) {
			check_number=0;
		  }
		}
		if (check_number==0 || ((raw_mat.at(i).size())==2 && ((raw_mat.at(i)[1].size())==0) )) { // no number, so its a header-part
			header_temp.push_back((raw_mat).at(i));
			header_scanned=true;
			if (data_scanned ) { 
				raw_data_tensor.push_back(data_temp);
				data_temp.clear();
				data_scanned=false;
			}
		} else { // it's a number, so its a data-part
			data_temp.push_back((raw_mat).at(i));
			data_scanned=true;
			if (header_scanned) {
				raw_header_tensor.push_back(header_temp);
				header_temp.clear();
				header_scanned=false;
			}
		}
	}
	if (data_scanned ) { 
		raw_data_tensor.push_back(data_temp);
		data_temp.clear();
		data_scanned=false;
	}
	if (header_scanned) {
		raw_header_tensor.push_back(header_temp);
		header_temp.clear();
		header_scanned=false;
	}
	return true;
}

bool parser_methods::global_sputter_time_in_sec_from_STARTtime()
{
	double global_start_time = (double)tools::time::string_to_time_t(measurement_p.date_time_start);
	measurement_p.crater.global_sputter_time_p=measurement_p.crater.sputter_time_p;
	measurement_p.crater.global_sputter_time_p.name="global_sputter_time";
	
	for (int i=0;i<measurement_p.crater.global_sputter_time_p.data.size();i++)
		measurement_p.crater.global_sputter_time_p.data[i]+=global_start_time;
	if (measurement_p.date_time_stop=="")
	{
		double sputter_max_time = measurement_p.crater.global_sputter_time_p.data.back();
		time_t global_start_stop = tools::time::string_to_time_t(measurement_p.date_time_start) + sputter_max_time;
		measurement_p.date_time_stop = tools::time::time_t_to_string(global_start_stop);
	}
	return true;
}

bool parser_methods::global_sputter_time_in_sec_from_STOPtime()
{
// 	double global_stop_time = (double)tools::time::string_to_time_t(id.date_time_stop);
	measurement_p.crater.global_sputter_time_p = measurement_p.crater.sputter_time_p;
	double sputter_max_time = measurement_p.crater.sputter_time_p.data.back();
	time_t global_start_time = tools::time::string_to_time_t(measurement_p.date_time_stop) - sputter_max_time;
	measurement_p.date_time_start = tools::time::time_t_to_string(global_start_time);
	return global_sputter_time_in_sec_from_STARTtime();
}

bool parser_methods::add_global_sputter_time_in_sec_to_crater()
{
	if (measurement_p.date_time_start=="" && measurement_p.date_time_stop=="") return false;
	if (measurement_p.crater.sputter_time_p.data.size()==0) return false;
	if (measurement_p.date_time_start!="") return global_sputter_time_in_sec_from_STARTtime();
	if (measurement_p.date_time_stop!="") return global_sputter_time_in_sec_from_STOPtime();
	return false;
}

