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

#include "measurement.hpp"
#include "measurement_group_t.hpp"
#include "cluster_t.hpp"

#define TABLENAME_everything "everything"
#define TABLENAME_measurements "measurements"
#define TABLENAME_measurement_settings "measurement_settings"
#define TABLENAME_measurement_statistics "measurement_statistics"
#define TABLENAME_reference_measurement_isotopes "reference_measurement_isotopes"

/// this function consumed 1 GF and 1 sunny weekend :-(
int measurement_t::equilibrium_starting_pos()
{
	if (equilibrium_starting_pos_p>-1) return equilibrium_starting_pos_p;
	
	vector<double> local_minima_indices, local_maxima_indices, Y;
	/*calculate the equlibirium*/
	/// get local minima from reference clusters
	set<int> equilibrium_starting_positions;
	set<int> local_minima_set, local_maxima_set;
	int ext;
	vector<int> l_ext;
	for (auto& ref_cluster:reference_clusters())
	{
		if (ref_cluster->intensity().is_set())
			Y = ref_cluster->intensity().filter_gaussian(5,4)/*.moving_window_sd(5).moving_window_sd(5).moving_window_median(5)*/.data; 
		else if (ref_cluster->concentration().is_set())
			Y = ref_cluster->concentration().filter_gaussian(5,4)/*.moving_window_sd(5).moving_window_sd(5).moving_window_median(5)*/.data; 
		
		// global minimum
// 		local_minima_indices.push_back(statistics::get_min_index_from_Y(Y));
// 		cout << measurement->filename_p->filename_with_path() << "\t" << ref_cluster->name() << endl;
// 		print("global minimum");
// 		cout << statistics::get_min_index_from_Y(Y) << "\t" << Y[statistics::get_min_index_from_Y(Y)] << endl;
		// global maximum
// 		local_maxima_indices.push_back(statistics::get_max_index_from_Y(Y));
// 		print("global maximum");
// 		cout << statistics::get_max_index_from_Y(Y) << "\t" << Y[statistics::get_max_index_from_Y(Y)] << endl;
		// local minima
		l_ext = statistics::get_local_minima_indices(Y,0.5*statistics::get_median_from_Y(Y));
		local_minima_set.insert(l_ext.begin(),l_ext.end());
// 		print("local minima");
// 		print(l_ext);
		// local maxima
		l_ext = statistics::get_local_maxima_indices(Y,1.5*statistics::get_median_from_Y(Y)); // statistics::get_median_from_Y(Y)+2*statistics::get_mad_from_Y(Y)
		local_maxima_set.insert(l_ext.begin(),l_ext.end());
// 		local_maxima_indices.insert(local_maxima_indices.end(),l_ext.begin(),l_ext.end());
// 		print("local maxima");
// 		print(l_ext);
		
		local_maxima_set.erase(0);
		local_minima_set.erase(0);
		
		if (local_maxima_set.size()>0 )
		{
			local_maxima_indices.assign(local_maxima_set.begin(),local_maxima_set.end());
			ext = local_maxima_indices[statistics::get_min_index_from_Y(local_minima_indices)];
			equilibrium_starting_positions.insert( statistics::get_index_for_next_value_lower_than_treshold(Y,1.25*statistics::get_median_from_Y(Y)/*+10*statistics::get_mad_from_Y(Y)*/,ext));
		}
		else if (local_minima_set.size()>0 )
		{
			local_minima_indices.assign(local_minima_set.begin(),local_minima_set.end());
			ext = local_minima_indices[statistics::get_min_index_from_Y(local_minima_indices)];
			equilibrium_starting_positions.insert( statistics::get_index_for_next_value_higher_than_treshold(Y,0.9*statistics::get_median_from_Y(Y)/*-10*statistics::get_mad_from_Y(Y)*/,ext));
		}
		else
		{
			/// minimum value at start
			if (statistics::get_min_index_from_Y(Y)<1) equilibrium_starting_positions.insert( statistics::get_index_for_next_value_higher_than_treshold(Y,0.9*statistics::get_median_from_Y(Y)));
			/// maximum value at start
			else equilibrium_starting_positions.insert( statistics::get_index_for_next_value_lower_than_treshold(Y,1.25*statistics::get_median_from_Y(Y)));	
		}
	}

	equilibrium_starting_pos_p = 0;
	if (equilibrium_starting_positions.size()>0)
	{
		vector<double> positions(equilibrium_starting_positions.begin(),equilibrium_starting_positions.end());
		if (positions[statistics::get_min_index_from_Y(positions)]<0.5*Y.size()) equilibrium_starting_pos_p = positions[statistics::get_min_index_from_Y(positions)];
		else equilibrium_starting_pos_p=0;
	} 
	if (equilibrium_starting_pos_p==0) return equilibrium_starting_pos_p;
	
	return equilibrium_starting_pos_p;
}


int measurement_t::chip_x()
{
	return filename_p->chip_x;
}
int measurement_t::chip_y()
{
	return filename_p->chip_y;
}
std::__cxx11::string measurement_t::lot()
{
	return filename_p->lot;
}
std::__cxx11::string measurement_t::monitor()
{
	return filename_p->monitor;
}
int measurement_t::wafer()
{
	return filename_p->wafer;
}

int measurement_t::group()
{
	return filename_p->group;
}
bool measurement_t::is_secondary()
{
	return secondary;
}

bool measurement_t::is_reference()
{
	if (reference_calculation_method=="") return false;
	if (reference_calculation_method=="NULL") return false;
	return true;
}

int measurement_t::olcdb()
{
	return filename_p->olcdb;
}
std::__cxx11::string measurement_t::tool_name()
{
	return tool_name_p;
}
std::__cxx11::string measurement_t::repition()
{
	return filename_p->repition;
}

void measurement_t::to_screen(string prefix) 
{
	filename_p->to_screen(prefix);
	cout << prefix << "tool_name: " << tool_name() << endl;
// 	for (auto file:files_locations)
// 	cout << prefix <<"\t" << file << endl;
	cout << prefix << "clusters: " << endl;
	for (auto cluster:clusters)
	{
		cluster.second.to_screen(prefix+"\t");
	}
	crater.to_screen(prefix+"\t");
}


void measurement_t::to_screen(vector<measurement_t>* measurements)
{
	for (int i=0;i<measurements->size();i++)
	{
		measurements->at(i).to_screen("["+to_string(i)+"]");
	}
	return;
}



/// TODO !!!
// check if reference_calculation_method is "NULL" in db
bool measurement_t::load_from_database()
{
	if (loaded_from_database) return true;
	std::string sql1, sql2;
		 
	sql1 = "SELECT * FROM " \
			TABLENAME_everything \
			" WHERE " \
			"lot='" + lot()+ "' AND " \
			"wafer=" +to_string(wafer())+ " AND " \
			"chip_x=" +to_string(chip_x())+ " AND " \
			"chip_y=" +to_string(chip_y())+ " AND " ;
	sql2 = sql1;
	
	if (settings.sputter_energy().is_set())
		sql1+="sputter_energy=" +to_string(settings.sputter_energy().data[0])+ " AND ";
	
	if (settings.sputter_element()!="")
		sql1+="sputter_element='" +settings.sputter_element()+ "' AND ";
	
	if (settings.polarity()!="")
		sql1+="polarity='" +settings.polarity()+ "' AND ";
	
	if (tool_name()!="NULL")
		sql1+="tool_name='" +tool_name()+ "' AND ";
	
	sql1+="monitor='" +monitor()+ "';";
	sql2+="monitor='" +monitor()+ "';";
	
	map<string,vector<string>> lines_map;
	if (!db.execute_sql(sql1,database_t::callback_lines_map,&lines_map)) return false;
	
	if (lines_map.size()==0 || lines_map.begin()->second.size()==0) if (!db.execute_sql(sql2,database_t::callback_lines_map,&lines_map)) return false;
	if (lines_map.size()==0 || lines_map.begin()->second.size()==0) return false;
	
	
	/*set matrix/substrat*/
	if (sample_p->matrix.isotopes.size()==0)
	if (!sample_p->matrix.set_isotopes_from_name(lines_map["matrix_elements"][0]))
	{
		cout << "measurement_t::load_from_database: !sample_p->matrix.set_isotopes_from_name(" << lines_map["substrat_elements"][0] << ")" << endl;
	}
	
// 	/*populate refence_cluster_names*/ --> shifted to measurement_group_t
// 	for (auto& cluster:clusters)
// 	{
// 		if (cluster.second.leftover_elements(sample_p->matrix.isotopes).size()==0)
// 		{
// // 			cout << "cluster.second.leftover_elements(sample_p->matrix.isotopes).size()=" << cluster.second.leftover_elements(sample_p->matrix.isotopes).size() << endl;
// 			measurement_group->add_to_reference_cluster_names(cluster.second.name());
// 		}
// 	}
	
	/*populate reference sample */
	double t;
	for (int i=0;i<lines_map.begin()->second.size();i++)
	{
		isotope_t isotope(lines_map["implant_isotope"][i]);
		for (auto& cluster:clusters)
		{
// 			for (auto& reference_cluster_name:measurement_group->reference_cluster_names())
// 				if(reference_cluster_name==cluster.second.name()) continue;
			
			/// do not populate the reference cluster --> skip it
// 			if (measurement_group->reference_cluster_names().find(cluster.first)!=measurement_group->reference_cluster_names().end()) continue;

			if (cluster.first.find(isotope.name())!=std::string::npos)
			{
				t= tools::str::str_to_double(lines_map["maximum_concentration"][i]);
				cluster.second.set_maximum_concentration_p(t); 
				
				t= tools::str::str_to_double(lines_map["depth_at_maximum_concentration"][i]);
				cluster.second.set_depth_at_maximum_concentration_p(t); 
				
				t= tools::str::str_to_double(lines_map["dose"][i]);
				cluster.second.set_dose_p(t); 
			}
		}
	}
	reference_calculation_method = lines_map["reference_calculation_method"][0];
	loaded_from_database = true;
	return true;
}

bool measurement_t::save_to_database()
{
	return false;
}

cluster_t* measurement_t::reference_cluster(isotope_t& isotope_or_element)
{
	for (auto& ref_cluster:reference_clusters())
	{
		if (isotope_or_element.nucleons<1) 
		{
			if (ref_cluster->are_elements_in_cluster({isotope_or_element})) return ref_cluster;
		}
		else if (ref_cluster->are_isotopes_in_cluster({isotope_or_element})) return ref_cluster;
	}
	return nullptr;
}


vector<cluster_t*> measurement_t::reference_clusters()
{
	if (reference_clusters_p.size()>0) return reference_clusters_p;
	for (auto& cluster_name:measurement_group->reference_matrix_cluster_names())
		if (clusters.find(cluster_name)!=clusters.end()) reference_clusters_p.push_back(&clusters[cluster_name]);
	if (reference_clusters_p.size()!=measurement_group->reference_matrix_cluster_names().size())
	{
		error_messages_p.push_back("measurement_t::reference_clusters(): reference_clusters_p.size()!=measurement_group->reference_cluster_names().size() --> measurements in measurement_group have not the same tool export settings");
		return {};
	}
	return reference_clusters_p;
}

vector<std::__cxx11::string> measurement_t::error_messages()
{
	return error_messages_p;
}

// map<std::__cxx11::string, quantity_t> measurement_t::reference_clusters_intensities()
// {
// 	if (reference_clusters_intensities_p.size()>0) return reference_clusters_intensities_p;
// 	
// 	// get I of each ref_cluster
// 	for (auto& ref_cluster:reference_clusters())
// 	{
// 		reference_clusters_intensities_p[ref_cluster->name()]=ref_cluster->intensity().median();
// 	}
// 	return reference_clusters_intensities_p;
// }
// 
// 
// map<std::__cxx11::string, quantity_t> measurement_t::reference_clusters_concentrations()
// {
// 	if (reference_clusters_concentrations_p.size()>0) return reference_clusters_concentrations_p;
// 	quantity_t conc;
// 	conc.dimension="amount";
// 	conc.unit="at";
// 	conc.name="concentration";
// 	conc.data.resize(1);
// 	bool found;
// 	for (auto& ref_cluster:reference_clusters())
// 	{
// 		found = false;
// 		for (auto& matrix_isotope:sample_p->matrix.isotopes)
// 		{
// 			
// 			if (ref_cluster->name().find(matrix_isotope.symbol)!=string::npos)
// 			{
// 				found = true;
// 				conc.data[0]=matrix_isotope.atoms;
// // 				cout << filename_p->filename_with_path() << "::"<< ref_cluster->name() << "::::" << matrix_isotope.nucleons << matrix_isotope.symbol << matrix_isotope.atoms << endl;
// // 				cout << "ref_cluster->name():" << ref_cluster->name() << "\t" << matrix_isotope.atoms << endl;
// // 				conc.to_screen();
// // 				reference_clusters_concentrations_p[ref_cluster->name()]=conc;
// 				break;
// 			}
// 		}
// 		if (!found)	conc.data[0]=0;
// 		reference_clusters_concentrations_p[ref_cluster->name()]=conc;
// 		
// 		
// 	}
// 	return reference_clusters_concentrations_p;
// }

set<isotope_t> measurement_t::isotopes()
{
	set<isotope_t> isotopes_p;
	for (auto& cluster:clusters)
	{
		isotopes_p.insert(cluster.second.isotopes.begin(),cluster.second.isotopes.end());
	}
	return isotopes_p;
}

/*OPERATORS*/

bool measurement_t::operator==(const measurement_t& measurement)
{
	/*return id.is_same_measurement(measurement->id)*/;
	
// 	if (filename_p->filename_without_crater_depths()==measurement.filename_p->filename_without_crater_depths()) return true;
	
	if (conf.measurement_definition_lot && filename_p->lot!=measurement.filename_p->lot) return false;
	if (conf.measurement_definition_olcdbid && filename_p->olcdb!=measurement.filename_p->olcdb) return false;
	if (conf.measurement_definition_lot_split && filename_p->lot_split!=measurement.filename_p->lot_split) return false;
	if (conf.measurement_definition_wafer && filename_p->wafer!=measurement.filename_p->wafer) return false;
	if (conf.measurement_definition_monitor && filename_p->monitor!=measurement.filename_p->monitor) return false;
	if (conf.measurement_definition_chip && filename_p->chip_x!=measurement.filename_p->chip_x) return false;
	if (conf.measurement_definition_chip && filename_p->chip_y!=measurement.filename_p->chip_y) return false;
	if (conf.measurement_definition_groupid && filename_p->group!=measurement.filename_p->group) return false;
	if (conf.measurement_definition_repition && filename_p->repition!=measurement.filename_p->repition) return false;
	
	
	if ((filename_p->lot=="" && measurement.filename_p->lot=="") || (filename_p->wafer==-1 && measurement.filename_p->wafer==-1) )
	{
// 		int dist = tools::str::levenshtein_distance(filename_p->filename_without_crater_depths(), measurement.filename_p->filename_without_crater_depths());
// 		if (dist>1) return false;
// 		if (filename_p->filename_without_crater_depths() == measurement.filename_p->filename_without_crater_depths()) return true;
		if ((filename_p->not_parseable_filename_parts).size() != (measurement.filename_p->not_parseable_filename_parts).size()) return false;
		for (int i=0;i<filename_p->not_parseable_filename_parts.size();i++)
			if (filename_p->not_parseable_filename_parts[i] != measurement.filename_p->not_parseable_filename_parts[i]) return false;
			
	}
	
	return true;
}

bool measurement_t::operator!=(const measurement_t& measurement)
{
	return !operator==(measurement);
}







