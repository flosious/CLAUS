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
#include "parser_methods.dsims_asc_t.hpp"

dsims_asc_t::dsims_asc_t() : parser_methods()
{
	/// overwrites parser_methods::tool_name
	tool_name = "dsims";
	/// overwrites parser_methods::file_type_endings
	file_type_endings = {"dp_rpc_asc"};
}

/// Order is important
bool dsims_asc_t::parse(filename_t* filename_p, std::__cxx11::string* contents)
{
	filename = filename_p;
	file_contents = contents;
	if (!check()) 
	{
// 		error=true;
// 		error_messages.push_back("dsims_asc_t::parse:check(): "+ filename->filename()); // Warnings?
		return false;
	}
	if (!parse_file_contents()) 
	{
// 		error=true;
// 		error_messages.push_back("dsims_asc_t::parse:parse_file_contents(): "+ filename->filename());
		return false;
	}
// 	if (!parse_filename()) 
// 	{
// 		error=true;
// 		error_messages.push_back("dsims_asc_t::parse:parse_filename(): "+ filename->filename());
// 		return false;
// 	}
	add_crater_total_sputter_time();
	add_global_sputter_time_in_sec_to_crater();
// 	copy_to_measurement_p();
	return true;
}

/// Order is important !!!
bool dsims_asc_t::parse_file_contents() 
{	
	if (!parse_data_and_header_tensors(/*&raw_data_tensor,&raw_header_tensor*/)) return false;
	if (!parse_headers(&raw_header_tensor)) return false;
	if (check_Ipr()) ipr_shift_correction(&raw_data_tensor);
	remove_corrupted_lines();
	if (!parse_cluster()) return false;
	parse_settings(&raw_header_tensor); // do nothing if false
	add_data_time_to_measurement_p();
	return true;
}

bool dsims_asc_t::check() {
	string find_this="EGate rate (%)";
	if (file_contents->find(find_this)==string::npos) return false;
	return true;
}

vector<isotope_t> dsims_asc_t:: parse_isotopes(string cluster_name)
{
	vector<isotope_t> isotopes;
	vector<string> single_isotopes= tools::str::get_strings_between_delimiter(cluster_name," ");
	for (string single_isotope:single_isotopes)
	{
		smatch match;
		regex reg ("^([0-9]{0,3})([a-zA-Z]{1,3})([0-9]*)"); 
		if (regex_search(single_isotope,match,reg)) 
		{
			isotope_t isotope;
			if (match[1]!="") isotope.nucleons = tools::str::str_to_int(match[1]);
			else return {};
			if (match[2]!="") isotope.symbol = match[2];
			else return {};
			if (match[3]!="") isotope.atoms = tools::str::str_to_int(match[3]);
			else isotope.atoms=1;
			isotopes.push_back(isotope);
		}
	}
	return isotopes;
}

bool dsims_asc_t::check_cols_size(vector<column_t>& cols)
{
// 	int last_col_size==-1;
	for (int i=1;i<cols.size();i++)
	{
		if (cols[i].cluster_name=="Ipr") continue;
		if (cols[i].data.size()!=cols[i-1].data.size()) return false;
	}
	return true;
}

vector<double> dsims_asc_t::get_common_X_dimension_vector()
{
	vector<double> mins;
	vector<double> maxs;
	vector<column_t> cols = columns();
	for (int c=0;c<cols.size();c=c+2)
	{
		int line;
		line = statistics::get_min_index_from_Y(cols[c].data);
		mins.push_back(cols[c].data[line]);
		line = statistics::get_max_index_from_Y(cols[c].data);
		maxs.push_back(cols[c].data[line]);
	}
	
	if (mins.size()==0 || maxs.size()==0)
	{
		error=true;
		error_messages.push_back("dsims_asc_t::set_crater_sputter_depth_or_time: mins.size()==0 || maxs.size()==0");
		return {};
	}
	
	double x_min = mins[statistics::get_max_index_from_Y(mins)];
	double x_max = maxs[statistics::get_min_index_from_Y(maxs)];
	
// 	cout << "x_min="<<x_min<<endl;
// 	cout << "x_max="<<x_max<<endl;
	
// 	double resolution = (x_max - x_min)/cols[0].data.size();
	double resolution = (x_max - x_min)/cols[0].data.size()*2/3;
// 	cout << "resolution()="<<resolution<<endl;
	int common_dimension_data_size = floor(x_max-x_min)/resolution;
	if (common_dimension_data_size<1) return {};
	vector<double> common_dimension_data (common_dimension_data_size);
	for (int i=0;i<common_dimension_data.size();i++)
		common_dimension_data[i]=resolution*i+x_min;
	
// 	cout << "common_dimension_data:" << endl;
// 	print(common_dimension_data);
	
	return common_dimension_data;
}

bool dsims_asc_t::parse_cluster()
{
	vector<double> common_X_dimension = get_common_X_dimension_vector();
// 	cout << "print(common_X_dimension):" << endl;
// 	print(common_X_dimension);
	vector<column_t> cols = columns();
	if (!check_cols_size(cols)) return false;
	/* sputter_depth // sputter_time */
	if (cols[0].dimension=="Depth")
	{
		measurement_p.crater.sputter_depth_p.unit = cols[0].unit;
		measurement_p.crater.sputter_depth_p.dimension = "length";
		measurement_p.crater.sputter_depth_p.name = "sputter_depth";
		measurement_p.crater.sputter_depth_p.data = common_X_dimension;
	}
	else
	{
		measurement_p.crater.sputter_time_p.unit = cols[0].unit;
		measurement_p.crater.sputter_time_p.dimension = "time";
		measurement_p.crater.sputter_time_p.name = "sputter_time";
		measurement_p.crater.sputter_time_p.data = common_X_dimension;
	}
	/* cluster */
	for (int c=1;c<cols.size();c=c+2)
	{
		map<double,double> data_XY;
		tools::vec::combine_vecs_to_map(&cols[c-1].data,&cols[c].data,&data_XY);
		if (data_XY.size()==0) return false;
		
		if (cols[c].dimension=="I") 
		{
			if (cols[c].cluster_name=="Ipr")
			{
				measurement_p.crater.sputter_current_p.unit = cols[c].unit;
				measurement_p.crater.sputter_current_p.dimension = "e_current";
				measurement_p.crater.sputter_current_p.name="sputter_current";
				measurement_p.crater.sputter_current_p.data= statistics::interpolate_data_XY(&data_XY,&common_X_dimension);
			}
			else
			{
				cluster_t cluster;
// 				if (measurement_p.crater.sputter_time_p.data.size()>0) cluster.sputter_time_p= &measurement_p.crater.sputter_time_p;
// 				if (measurement_p.crater.sputter_depth_p.data.size()>0) cluster.sputter_depth_p= &measurement_p.crater.sputter_depth_p;
// 				cluster.intensity_p.unit = cols[c].unit;
				cluster.intensity_p.unit = "cnt/s";
				cluster.intensity_p.dimension = "amount/time";
				cluster.intensity_p.name = "intensity";
				cluster.intensity_p.data = statistics::interpolate_data_XY(&data_XY,&common_X_dimension);
				cluster.isotopes = parse_isotopes(cols[c].cluster_name);
				
				measurement_p.clusters[cluster.name()]=cluster;
			}
		}
		else if (cols[c].dimension=="C") 
		{
			cluster_t cluster;
// 			if (measurement_p.crater.sputter_time_p.data.size()>0) cluster.sputter_time_p= &measurement_p.crater.sputter_time_p;
// 			if (measurement_p.crater.sputter_depth_p.data.size()>0) cluster.sputter_depth_p= &measurement_p.crater.sputter_depth_p;
// 			cluster.concentration_p.unit = cols[c].unit;
			cluster.concentration_p.unit = "at/cm^3";
			cluster.concentration_p.dimension = "amount*(length)^(-3)";
			cluster.concentration_p.name="concentration";
			cluster.concentration_p.data = statistics::interpolate_data_XY(&data_XY,&common_X_dimension);
			cluster.isotopes = parse_isotopes(cols[c].cluster_name);
			measurement_p.clusters[cluster.name()]=cluster;
		} 
		else 
		{
			error_messages.push_back("dsims_asc_t::parse_cluster: UNKOWN TYPE");
			return false;
		}

	}
// 	for (auto& cluster:measurement_p.clusters)
// 		cluster.second.to_screen();
	return true;
}

void dsims_asc_t::add_data_time_to_measurement_p()
{
	// set creation date from settings (file contents)
	measurement_p.date_time_stop = tools::time::time_t_to_string(tools::file::creation_date(filename->filename()));
	if (settings.find("Analysis date")!=settings.end()) 
	{
		measurement_p.date_time_start = settings["Analysis date"];
		if (settings.find("Analysis time")!=settings.end()) measurement_p.date_time_start += " " +settings["Analysis time"];
		measurement_p.date_time_start = tools::str::remove_NOTchars_from_string(measurement_p.date_time_start,":1234567890/ ");
		time_t tt = tools::time::string_to_time_t(measurement_p.date_time_start.c_str(), "%m/%d/%Y %H:%M");
		measurement_p.date_time_start = tools::time::time_t_to_string(tt,"%Y-%m-%d %H:%M:%S");
	}
	
}

bool dsims_asc_t::add_crater_total_sputter_time()
{
	if (settings.find("Total sputtering time (s)")!=settings.end())
	{
		measurement_p.crater.total_sputter_time_p.unit="s";
		measurement_p.crater.total_sputter_time_p.dimension="time";
		measurement_p.crater.total_sputter_time_p.name="total_sputter_time";
		measurement_p.crater.total_sputter_time_p.data.push_back(tools::str::str_to_double(settings["Total sputtering time (s)"]));
	} 
	else
	{
		quantity_t crater_time;
		double max_sputter_time=0;
		crater_time.unit="s";
		crater_time.dimension="time";
		crater_time.name="total_sputter_time";
		
		if (measurement_p.crater.sputter_time_p.data.size()==0) return false;
		
		int index_max= statistics::get_max_index_from_Y(measurement_p.crater.sputter_time_p.data);
		double val = measurement_p.crater.sputter_time_p.data[index_max];
		if (val > max_sputter_time) max_sputter_time = val;

		crater_time.data.push_back(max_sputter_time);
		if (max_sputter_time>0) measurement_p.crater.total_sputter_time_p=crater_time;
		else return false;
	}
	return true;
}

void dsims_asc_t::column_t::to_screen()
{
	cout << "unit="<<unit<<"\tcluster_name="<<cluster_name<<"\tdimension="<<dimension<<"\tdata.size()="<<data.size()<<endl;
}


vector<dsims_asc_t::column_t> dsims_asc_t::columns()
{
	vector<dsims_asc_t::column_t> cols;
	vector<vector<string>> data_cols_lines = tools::mat::transpose_matrix(raw_data_tensor[0]);
	// cols_per_element=3 --> dp_rpc_asc; =2 --> dp_asc
	int cols_per_element = dimensions.size()/cluster_names.size();
	for (int i=0;i<dimensions.size();i++)
	{
		dsims_asc_t::column_t col;
		col.cluster_name=cluster_names[i/cols_per_element];
		col.dimension=dimensions[i];
		col.unit=units[i];
		vector<string> col_data_string = data_cols_lines[i];
		col.data=tools::mat::str_vec_to_double_vec(col_data_string);
		if (col.data.size()==0) continue;
		cols.push_back(col);
	}
// 	for (auto& col:cols)
// 		col.to_screen();
	return cols;
}

bool dsims_asc_t::parse_headers(vector<vector<vector<string>>> *headers)
{
	
	/*1st header */
	vector<vector<string> > header;
	int c=0;
	tools::str::replace_chars(&headers->at(c)," [","[");
	int sample_names_line_number=(headers->at(c)).size()-3;
	int elements_line_number=(headers->at(c)).size()-2;
	int dimensions_units_line_number=(headers->at(c)).size()-1;
	
	
	vector<string> samples;
	for (int i=0;i<((headers->at(c))[sample_names_line_number]).size();i++) {
		if (( (headers->at(c))[sample_names_line_number][i]).size()>0 ) {
			string result = ((headers->at(c))[sample_names_line_number][i]);
			samples.push_back(result);
		}
	}
	if (samples.size()!=1) 
	{
		error=true;
		error_messages.push_back("dsims_asc_t::parse_headers: samples.size()!=1");
		return false;
	}
	
	for (int i=0;i<((headers->at(c))[elements_line_number]).size();i++) {
        string result = ((headers->at(c))[elements_line_number][i]);
		if (result.size()>0 ) {
			cluster_names.push_back(result);
		} else {
        }
	}
	
	for (int i=0;i<((headers->at(c))[dimensions_units_line_number]).size();i++) {
		if ((((headers->at(c))[dimensions_units_line_number])[i]).size()>0) {
			units.push_back(tools::str::get_string_between_string_A_and_next_B(&((headers->at(c))[dimensions_units_line_number][i]),"[","]"));
			dimensions.push_back((tools::str::get_strings_between_delimiter(((headers->at(c))[dimensions_units_line_number][i]),"[")).at(0));
		}
	}
        
	if (units.size()!=dimensions.size()) {
		error_messages.push_back("size of units and dimensions not equal:\t" + filename->filename());
		return false;
	}
	
	
	int cols_per_element = dimensions.size()/cluster_names.size();
	if (cols_per_element!=3)
	{
		/* 
		 * dp_asc has a bug in the exported data: dimension and unit are one X- and one Y-axis to long
		 * to fix it, we remove it from the end
		 */
		if (check_Ipr())
		{
			dimensions.erase(dimensions.end()-4,dimensions.end()-2);
			units.erase(units.end()-4,units.end()-2);
		} 
	}
// 	print(cluster_names);
// 	print(units);
// 	print(dimensions);
    return true;
}


bool dsims_asc_t::check_Ipr() 
{
	is_Ipr=false;
	string find_this = "Ipr";
	is_Ipr = tools::mat::find_str_in_mat(&cluster_names,find_this);
	return is_Ipr;
}

set<int> dsims_asc_t::corrupted_lines()
{
// 	cout << "dsims_asc_t::corrupted_lines"<<endl;
	set<int> corrupted_lines_p;
	vector<vector<string>> cols_lines = tools::mat::transpose_matrix(raw_data_tensor.at(0));
	vector<double> col;
	
	int colssize=cols_lines.size();
	int max_size=-1;
// 	cout << 0 << ": " << col.size() << endl;
	if (check_Ipr()) colssize = colssize-3;
	for (int c=0;c<colssize;c++)
	{
		col = tools::mat::str_vec_to_double_vec(cols_lines[c]);
// 		cout << c << ": " << col.size() << endl;
		if (col.size()<2) continue;
		if (max_size==-1) 
		{
			max_size = col.size();
			continue;
		}
		
		if (col.size()<max_size) 
		{
			corrupted_lines_p.insert(max_size-1);
			max_size = col.size();
		}
		if (col.size()>max_size) 
		{
			corrupted_lines_p.insert(col.size()-1);
// 			max_size = ;
		}
	}
	// removes the corrputed_lines
	
	return corrupted_lines_p;
}

void dsims_asc_t::remove_corrupted_lines()
{
	
	set<int> corrupted_lines_p=corrupted_lines();
// 	cout << "corrupted_lines.size()="  << corrupted_lines_p.size() <<  endl;
	for (set<int>::reverse_iterator it=corrupted_lines_p.rbegin();it!=corrupted_lines_p.rend();++it)
	{
		
		raw_data_tensor.at(0).erase(raw_data_tensor.at(0).begin()+*it);
	}
}


/*
 * exported data is bugged, there are to many "\t" in the lines, where there is no cluster data, but Ipr data
 */
bool dsims_asc_t::ipr_shift_correction(vector<vector<vector<string>>>* raw_data_tensor) 
{
	vector<vector<string>>* mat = &raw_data_tensor->at(0);
    if (mat->size()==0) return false;
	
	int cols_per_element = dimensions.size()/cluster_names.size();
	if (cols_per_element!=3) cols_per_element=2;
	
// 	Ipr_col = (tools::mat::find_in_vec(&elements,"Ipr"))*3+1;
	if (cols_per_element==3) Ipr_col = ((tools::mat::find_in_vec(&cluster_names,"Ipr"))+1)*3;
	else Ipr_col = dimensions.size()-1-1;
	int Ipr_offset_line;
	int Ipr_col_at_EOL;
    int total_lines=mat->size();
	for (Ipr_col_at_EOL=0;Ipr_col_at_EOL<mat->back().size();Ipr_col_at_EOL++) {
		if (!tools::str::is_empty(mat->back().at(Ipr_col_at_EOL))) break;
	}
	Ipr_col_at_EOL++;
	for (Ipr_offset_line=0;Ipr_offset_line<total_lines;Ipr_offset_line++) 
	{
		if (mat->at(Ipr_offset_line)[cols_per_element-1]=="") break; // die 3. spalte enthaelt immer daten (intensität des 1. isotops)
	}
// 	mat->erase(mat->begin()+Ipr_offset_line+1); // this line is often corrupted
	vector<vector<string>> Ipr_mat;
	// cut off Ipr from data mat
	Ipr_mat.insert(Ipr_mat.begin(),mat->begin()+Ipr_offset_line,mat->end());	
	// delete first columns within the matrix
	Ipr_mat=tools::mat::transpose_matrix(Ipr_mat);
	Ipr_mat.erase(Ipr_mat.begin(),Ipr_mat.begin()+(Ipr_mat.size()-cluster_names.size()*cols_per_element));
	Ipr_mat=tools::mat::transpose_matrix(Ipr_mat);
	mat->erase(mat->begin()+Ipr_offset_line,mat->end()); 
	// add them together
	mat->insert(mat->end(),Ipr_mat.begin(),Ipr_mat.end());
    return true;
}



// /*
//  * parse_measurement_dsims
//  * result: measured will be filled.
//  * measured->settings["name as in raw file"]="value as in raw file"
//  * measured->masses_calib contains a string matrix, each line is an element such as: 
//  *                  Species-0         | Mass (amu)-1 | Mass (field)-2 | Detector-3 | WT(s)-4   | Extra WT(s)-5 | CT(s)-6   | Offset-7
//                     -------------------------------------------------------------------------------------------------
//                     11B             | 11.009     | 473035       | EM       | 1.440   | 1.040       | 1.040   | 0
//                     30Si            | 29.974     | 781229       | EM       | 1.040   | 0.000       | 1.040   | 0
//                     70Ge            | 69.924     | 1195269      | EM       | 1.120   | 0.000       | 1.040   | 0
//  * 
// */
bool dsims_asc_t::parse_settings(vector<vector<vector<string>>> *headers)
{
// 	/*1st header headers[0]*/
// 	for (int i=0;i<headers->at(0).size();i++)
// 	{
// 		for (int j=1;j<headers->at(t)[i].size();j++) 
// 		{
// 				if (headers->at(t)[i].size()>1 && headers->at(t)[i][j]!="") 
// 				{
// 					string setting_name = headers->at(t)[i][0];
//                     string setting_value = headers->at(t)[i][j]
//                     tools::str::remove_spaces_from_string_start(&setting_value);
//                     tools::str::remove_spaces_from_string_start(&setting_name);
//                     tools::str::remove_spaces_from_string_end(&setting_value);
//                     tools::str::remove_spaces_from_string_end(&setting_name);
//                     settings[setting_name]=setting_value;
// 				}
// 		}
// 	}
	/*2nd header headers[1]*/
    int line_masses=-1;
	for (int t=0;t<headers->size();t++)
	{
    for (int i=0;i<headers->at(t).size();i++) 
	{
        if (headers->at(t)[i][0].find("---------------------------------------")!=string::npos) line_masses = i;
		if (headers->at(t)[i][0].find("CALIBRATION PARAMETERS")!=string::npos) break;
		if (headers->at(t)[i][0].find("DATA START")!=string::npos) break;
        if (headers->at(t)[i][0]!="") {
// 			string line;
            for (int j=1;j<headers->at(t)[i].size();j++) 
			{
// 				line+=" " + headers->at(t)[i][j];
                if (headers->at(t)[i].size()>1 && headers->at(t)[i][j]!="") {
                    string setting_name = headers->at(t)[i][0];
                    string setting_value = headers->at(t)[i][j];
					string unit="";
					unit = tools::str::get_string_between_string_A_and_next_B(&setting_name,"(",")");
// 					setting_name.erase
                    tools::str::remove_spaces_from_string_start(&setting_value);
                    tools::str::remove_spaces_from_string_start(&setting_name);
                    tools::str::remove_spaces_from_string_end(&setting_value);
                    tools::str::remove_spaces_from_string_end(&setting_name);
                    settings[setting_name]=setting_value;
// 					settings[setting_name]=setting_value+"\t"+unit;
                    break;
                }
            }
//             regex setting ("([a-zA-Z]+)\\s+\\((.+)\\)\\s+([a-zA-Z0-9]+)"); 
// 			smatch match;
// 			if (regex_search(line,match,setting)) {
// 				string temp = string(match[3])+" "+string(match[2]);
// 				settings[match[1]]=temp;
// 			} 
        }
    }
	}

	if (line_masses>0) {
        for (int i=line_masses+1;i<headers->at(1).size();i++) {
            vector<string> mass_calib=tools::str::get_strings_between_delimiter(tools::str::remove_chars_from_string(headers->at(1)[i][0],"\\+() "),"|");
            if (mass_calib.size()<2) break;
            calibrated_masses[mass_calib[0]]=mass_calib;
        }
    }
// 	print("calibrated_masses:");
//     print(calibrated_masses);
// 	print(settings);
	if (settings.find("Electron gun")!=settings.end()) if (settings["Electron gun"]!="No") measurement_p.settings.egun_p=true;
	if (settings.find("Impact energy (eV)")!=settings.end()) 
	{
		measurement_p.settings.sputter_energy_p.name = "sputter_energy";
		measurement_p.settings.sputter_energy_p.unit="eV";
		measurement_p.settings.sputter_energy_p.dimension="energy";
		measurement_p.settings.sputter_energy_p.data.resize(1);
		measurement_p.settings.sputter_energy_p.data[0]=tools::str::str_to_double(settings["Impact energy (eV)"]);
	}
	if (settings.find("Secondary ion polarity")!=settings.end()) 
		measurement_p.settings.polarity_p=settings["Secondary ion polarity"];
	if (settings.find("Primary ions")!=settings.end()) 
	{
		if (settings["Primary ions"]=="O2+") measurement_p.settings.sputter_element_p="O";
		else measurement_p.settings.sputter_element_p="Cs";
	}
    return true;
}

// bool dsims_asc_t::parse_clusters(vector<vector<vector<string>>> *raw_data_tensor) {
// 	
//     // removes empty columns at the end of data
//     while (data.size()>dimensions.size()) {
//         data.erase(data.end());
//     }
// 	return true;
// }
