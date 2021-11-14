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

#include "measurement.hpp"
#include "measurement_group_t.hpp"
#include "cluster_t.hpp"

#define TABLENAME_everything "everything"
#define TABLENAME_measurements "measurements"
#define TABLENAME_measurement_settings "measurement_settings"
#define TABLENAME_measurement_statistics "measurement_statistics"
#define TABLENAME_reference_measurement_isotopes "reference_measurement_isotopes"

bool measurement_t::defined_olcdbid				=true;
bool measurement_t::defined_lot					=true;
bool measurement_t::defined_lot_split			=false;
bool measurement_t::defined_wafer				=true;
bool measurement_t::defined_monitor				=true;
bool measurement_t::defined_chip				=true;
bool measurement_t::defined_groupid				=true;
bool measurement_t::defined_repetition			=true;
bool measurement_t::defined_polarity			=true;
bool measurement_t::defined_sputter_element		=true;
bool measurement_t::defined_sputter_energy		=true;

quantity_t measurement_t::reference_intensity()
{
	if (reference_intensity_p.is_set()) return reference_intensity_p;
	if (reference_clusters().size()==0) return {};
	
	reference_intensity_p = reference_clusters()[0]->intensity();
	for (int i=1;i<reference_clusters().size();i++)
		reference_intensity_p=reference_intensity_p + reference_clusters()[i]->intensity();

// 	if (equilibrium_starting_pos>0) reference_intensity_p = reduce_quantity_by_equlibrium_starting_pos(reference_intensity_p);

	if (reference_intensity_p.is_set()) 
	{
		stringstream refs;
		for (int i=0;i<reference_clusters().size();i++)
		{
			refs << reference_clusters()[i]->name();
			if (i!=reference_clusters().size()-1)
				refs << "+";
		}
		calc_history.push_back(filename_p->filename_with_path()+"\t"+refs.str()+"\treference_intensity\t" + reference_intensity_p.to_str());
	}

	reference_intensity_p.name = "reference_intensity";
	
	return reference_intensity_p;
}

int measurement_t::chip_x()
{
	return filename_p->chip_x;
}
int measurement_t::chip_y()
{
	return filename_p->chip_y;
}
string measurement_t::lot()
{
	return filename_p->lot;
}
string measurement_t::monitor()
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
string measurement_t::tool_name()
{
	return tool_name_p;
}
string measurement_t::repetition()
{
	return filename_p->repetition;
}

string measurement_t::to_str(string prefix)
{
	stringstream ss;
	ss << endl;
	ss << prefix << filename_p->filename() << endl;
	ss << prefix << "{" << endl;
// 	if (crater.linescans.size()>0)
// 	{
// 		for (int ls=0;ls<crater.linescans.size();ls++)
// 			ss <<std::scientific <<   prefix << "\t" << crater.linescans[ls].to_str() << endl;
// 	}
// 	if (crater.total_sputter_depths().is_set())
// 		ss <<std::scientific <<   prefix << "\ttotal_sputter_depths(): " << crater.total_sputter_depths().to_str() << endl;
// 	if (crater.total_sputter_time().is_set())
// 		ss <<std::scientific  << prefix << "\ttotal_sputter_time(): " << crater.total_sputter_time().to_str() << endl;
	ss << crater.to_str(prefix + "\t") << endl;
	if (clusters.size()==0)
	{
		ss << prefix<<"\tno clusters" << endl;
		return ss.str();
	}
	
	if (reference_intensity().is_set())
		ss  <<std::scientific  << prefix <<"\t" << reference_intensity().to_str() << endl;
	for (auto& C:clusters)
	{
		ss << C.second.to_str(prefix+"\t");
	}
	ss << prefix << "}" << endl;
	return ss.str();
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
// 	cout << "measurement_t::load_from_database: " << lot() << " " << wafer() << " " << settings.sputter_element() << endl;
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
	if (sample_p->matrix.isotopes().size()==0)
	if (!sample_p->matrix.set_isotopes_from_name(lines_map["matrix_elements"][0]))
	{
		cout << "measurement_t::load_from_database: !sample_p->matrix.set_isotopes_from_name(" << lines_map["substrat_elements"][0] << ")" << endl;
	}
	
// 	/*populate refence_cluster_names*/ --> shifted to measurement_group_t
// 	for (auto& cluster:clusters)
// 	{
// 		if (cluster.second.leftover_elements(sample_p->matrix.isotopes()).size()==0)
// 		{
// // 			cout << "cluster.second.leftover_elements(sample_p->matrix.isotopes()).size()=" << cluster.second.leftover_elements(sample_p->matrix.isotopes()).size() << endl;
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
// 		return {};
	}
	return reference_clusters_p;
}

vector<string> measurement_t::error_messages()
{
	return error_messages_p;
}

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
// 	filename_p->to_screen("1\t");
// 	measurement.filename_p->to_screen("2\t");

	if (defined_lot && filename_p->lot!=measurement.filename_p->lot) return false;
	if (defined_olcdbid && filename_p->olcdb!=measurement.filename_p->olcdb) return false;
	if (defined_lot_split && filename_p->lot_split!=measurement.filename_p->lot_split) return false;
	if (defined_wafer && filename_p->wafer!=measurement.filename_p->wafer) return false;
	if (defined_monitor && filename_p->monitor!=measurement.filename_p->monitor) return false;
	if (defined_chip && filename_p->chip_x!=measurement.filename_p->chip_x) return false;
	if (defined_chip && filename_p->chip_y!=measurement.filename_p->chip_y) return false;
	if (defined_groupid && filename_p->group!=measurement.filename_p->group) return false;
	if (defined_repetition && filename_p->repetition!=measurement.filename_p->repetition) return false;
	if (defined_sputter_element && filename_p->sputter_element != measurement.filename_p->sputter_element) return false;
	if (defined_sputter_energy && filename_p->sputter_energy != measurement.filename_p->sputter_energy) return false;
	if (defined_polarity && filename_p->polarity != measurement.filename_p->polarity) return false;
	
	return true;
}

bool measurement_t::operator!=(const measurement_t& measurement)
{
	return !operator==(measurement);
}







