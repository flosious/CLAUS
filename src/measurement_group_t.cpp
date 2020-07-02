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
#include "measurement_group_t.hpp"
#include "measurement.hpp"

measurement_group_t::measurement_group_t()
{
	
}

bool measurement_group_t::check_cluster_names()
{
	set<string> clusternames = cluster_names();
	for (auto& measurement:measurements)
	{
		for (auto& clustername:clusternames)
			if (measurement->clusters.find(clustername)==measurement->clusters.end()) return false;
	}
	return true;
}

bool measurement_group_t::check_cluster_names(measurement_group_t* measurement_group)
{
	set<string> clusternames = cluster_names();
	for (auto& measurement:measurement_group->measurements)
	{
		for (auto& clustername:clusternames)
			if (measurement->clusters.find(clustername)==measurement->clusters.end()) return false;
		for (auto& cluster:measurement->clusters)
			if (clusternames.find(cluster.second.name())==clusternames.end()) return false;
	}
	return true;
}


measurement_group_t::measurement_group_t(measurement_t* measurement_p)
{
	tool_name_p = measurement_p->tool_name();
	group_p = measurement_p->filename_p->group;
// 	cout << "group_p = " << group_p <<endl;
	olcdb_p = measurement_p->filename_p->olcdb;
	settings_p = measurement_p->settings;
	measurements.push_back(measurement_p);
}

map<quantity_t, sample_t::matrix_t> measurement_group_t::SR_vs_matrix()
{
	map<quantity_t, sample_t::matrix_t> SR_vs_matrix_p;
	for (auto& measurement:measurements)
	{
		if (measurement->crater.sputter_rate().is_set() && measurement->sample_p->matrix.isotopes.size()>0)
			SR_vs_matrix_p.insert({measurement->crater.sputter_rate(),measurement->sample_p->matrix});
	}
	return SR_vs_matrix_p;
}

map<quantity_t, sample_t::matrix_t> measurement_group_t::RSF_vs_matrix(std::__cxx11::string cluster_name)
{
	map<quantity_t, sample_t::matrix_t> RSF_vs_matrix_p;
	
	
	for (auto& measurement:measurements)
	{
		if (measurement->clusters[cluster_name].RSF().is_set() && measurement->sample_p->matrix.isotopes.size()>0)
			RSF_vs_matrix_p.insert({measurement->clusters[cluster_name].RSF(),measurement->sample_p->matrix});
	}
	return RSF_vs_matrix_p;
}

map<sample_t::matrix_t, vector<cluster_t *> > measurement_group_t::matrix_vs_ref_clusters()
{
	map<sample_t::matrix_t, vector<cluster_t *> > matrix_vs_ref_clusters_p;
	
	for (auto& measurement:measurements)
	{
		if (measurement->sample_p->matrix.isotopes.size()>0)
			matrix_vs_ref_clusters_p.insert({measurement->sample_p->matrix,measurement->reference_clusters()});
	}
	
	return matrix_vs_ref_clusters_p;
}

quantity_t measurement_group_t::RSFs(string cluster_name)
{
	if (RSFs_from_custer_name.find(cluster_name)!=RSFs_from_custer_name.end()) return RSFs_from_custer_name[cluster_name];
	/// denying multiple tries --> breaking endless loops
	if (checked_RSF_clusters_list_p.find(cluster_name)!=checked_RSF_clusters_list_p.end()) return {};
	checked_RSF_clusters_list_p.insert(cluster_name);
	
	if (reference_matrix_cluster_names().size()==1)
	{
		quantity_t rsf_s;
		for (auto& measurement_in_MG:measurements)
		{
			if (measurement_in_MG->clusters.find(cluster_name)==measurement_in_MG->clusters.end()) continue;
			if (!measurement_in_MG->clusters[cluster_name].RSF().is_set()) continue;
// 			cout << measurement_in_MG->filename_p->filename_with_path() << "\t" << cluster_name << endl;
			if (!rsf_s.is_set()) rsf_s = measurement_in_MG->clusters[cluster_name].RSF();
			else rsf_s=rsf_s.add_to_data(measurement_in_MG->clusters[cluster_name].RSF());
// 			if (rsf_s.is_set())
// 			{
// 				if (RSFs_from_custer_name[cluster_name].is_set()) RSFs_from_custer_name[cluster_name].add_to_data(rsf_s);
// 				else RSFs_from_custer_name[cluster_name] = rsf_s;
// 			}
		}
		if (rsf_s.is_set())
			RSFs_from_custer_name[cluster_name] = rsf_s;
		else
			return {};
// 		if (!RSFs_from_custer_name[cluster_name].is_set()) return {};
// 		cout << "RSFs_from_custer_name[cluster_name].to_screen()" << endl;
// 		RSFs_from_custer_name[cluster_name].to_screen();
		return RSFs(cluster_name);
	}
	return {};
}

quantity_t measurement_group_t::SRs()
{
	if (SRs_p.is_set()) return SRs_p;
	/// denying multiple tries --> breaking endless loops
	if (checked_SR) return {};
	checked_SR=true;
	
	if (reference_matrix_cluster_names().size()==1)
	{
		for (auto& measurement_in_MG:measurements)
		{
			if (!measurement_in_MG->crater.sputter_rate().is_set()) continue;
			if (SRs_p.is_set()) SRs_p.add_to_data(measurement_in_MG->crater.sputter_rate());
			else SRs_p=measurement_in_MG->crater.sputter_rate();
		}
		if (!SRs_p.is_set()) return {};
		return SRs();
	}
	return {};
}

bool measurement_group_t::add(measurement_t* measurement)
{
	measurement_group_t mgroup(measurement);
	return add(&mgroup);
}

bool measurement_group_t::add(measurement_group_t* measurement_group)
{
	
	if (*measurement_group==*this)
	{
		measurements.merge(measurement_group->measurements);
		return true;
	}
	return false;
}

list<measurement_group_t> measurement_group_t::measurement_groups(list<measurement_t>* measurements)
{
	list<measurement_group_t> measurement_groups;
	bool added;
	for (auto& measurement:*measurements)
	{
		added=false;
		measurement_group_t mgroup(&measurement);
		// is there an exisiting measurement_group, which matches?
		for (auto& measurement_group:measurement_groups)
		{
			if (measurement_group.add(&mgroup))
			{
				measurement.measurement_group=&measurement_group;
				added=true;
				break;
			}
		}
		if (!added)
		{
			measurement_groups.push_back(mgroup);
			measurement.measurement_group=&measurement_groups.back();
		}
	}
	return measurement_groups;
}

void measurement_group_t::to_screen(std::__cxx11::string prefix)
{
	cout << prefix << "tool_name = " << tool_name_p << endl;
	cout << prefix << "group = " << group_p << endl;
	cout << prefix << "olcdb = " << olcdb_p << endl;
	settings_p.to_screen(prefix);
	cout << prefix << "measurements.size() = " << measurements.size() << endl;
	for (auto& measurement:measurements)
	{
		cout << prefix << "\t" << measurement->filename_p->filename_with_path() << endl;
	}
	return;
}

int measurement_group_t::group()
{
	return group_p;
}

int measurement_group_t::olcdb()
{
	return olcdb_p;
}

string measurement_group_t::tool_name()
{
	return tool_name_p;
}

measurement_settings_t measurement_group_t::settings()
{
	return settings_p;
}

set<string> measurement_group_t::reference_matrix_cluster_names()
{
	if (reference_cluster_names_p.size()>0) return reference_cluster_names_p;
	for (auto& measurement:measurements)
	{
		if (!measurement->load_from_database()) continue;
		for (auto& cluster:measurement->clusters)
		{
			if (cluster.second.leftover_elements(measurement->sample_p->matrix.isotopes).size()==0)
				reference_cluster_names_p.insert(cluster.second.name());
		}
	}
	return reference_cluster_names_p;
}


vector<std::__cxx11::string> measurement_group_t::error_messages()
{
	return error_messages_p;
}

set<isotope_t> measurement_group_t::reference_matrix_isotopes()
{
	if (reference_matrix_isotopes_p.size()>0) return reference_matrix_isotopes_p;
	for (auto& measurement:measurements)
	{
		if (!measurement->load_from_database()) continue;
		reference_matrix_isotopes_p.insert(measurement->sample_p->matrix.isotopes.begin(),measurement->sample_p->matrix.isotopes.end());
	}
	return reference_matrix_isotopes_p;
}


isotope_t measurement_group_t::isotope_from_cluster_name(string cluster_name)
{
	if (cluster_name_to_isotope_p.size()>0 && cluster_name_to_isotope_p.find(cluster_name)!=cluster_name_to_isotope_p.end()) return cluster_name_to_isotope_p[cluster_name];
	
	for (auto& isotope:isotopes())
	{
		string cluster_name = cluster_name_from_isotope(isotope);
		if (cluster_name!="") cluster_name_to_isotope_p[cluster_name] = isotope;
	}
	
	if (cluster_name_to_isotope_p.find(cluster_name)!=cluster_name_to_isotope_p.end()) return cluster_name_to_isotope_p[cluster_name];
	return {};

}

set<isotope_t> measurement_group_t::isotopes()
{
	set<isotope_t> isotopes_p;
	for (auto& measurement:measurements)
	{
		set<isotope_t> isotopes_in_m = measurement->isotopes();
		for (auto& isotope:isotopes_in_m)
		isotopes_p.insert(isotopes_in_m.begin(),isotopes_in_m.end());
	}
	return isotopes_p;
}

set<std::__cxx11::string> measurement_group_t::cluster_names()
{
	set<string> cluster_names_p;
	for (auto& measurement:measurements)
	{
		for (auto& cluster:measurement->clusters)
			cluster_names_p.insert(cluster.second.name());
	}
	return cluster_names_p;
}

std::__cxx11::string measurement_group_t::cluster_name_from_isotope(isotope_t isotope)
{	
// 	vector<string> isotope_names_in_cluster = tools::str::get_strings_between_delimiter();
	// matrix isotope?	
	for (auto& matrix_cluster_name:reference_matrix_cluster_names())
	{
		if (isotope.nucleons>0) // isotop
		{
			if (matrix_cluster_name.find(to_string(isotope.nucleons)+isotope.symbol)!=string::npos) return matrix_cluster_name;
		}
		else // element
			if (matrix_cluster_name.find(isotope.symbol)!=string::npos) return matrix_cluster_name;
	}
	
	for (auto& cluster_name:cluster_names())
	{
		if (isotope.nucleons>0) // isotop
		{
			if (cluster_name.find(to_string(isotope.nucleons)+isotope.symbol)!=string::npos) return cluster_name;
		}
		else // element
			if (cluster_name.find(isotope.symbol)!=string::npos) return cluster_name;
	}
	
	return "";
}

std::__cxx11::string measurement_group_t::name()
{
	stringstream name_p;
	if (conf.measurement_group_definition_olcdbid) name_p << olcdb_p << "_";
	if (conf.measurement_group_definition_groupid) name_p << group_p << "_";
	if (conf.measurement_group_definition_settings) 
	{
		if (settings_p.sputter_energy().is_set()) name_p << to_string(settings_p.sputter_energy().data[0])+settings_p.sputter_energy().unit;
		if (settings_p.sputter_element()!="") name_p << settings_p.sputter_element();
		if (settings_p.polarity()!="") name_p << settings_p.polarity();
		name_p << "_";
	}
	if (conf.measurement_group_definition_tool)  name_p  << tool_name_p << "_";
	string result = name_p.str();
	result.erase(result.begin()+result.size()-1);
	return result;
}


/*OPERATORS*/

bool measurement_group_t::operator==(measurement_group_t& measurement_group)
{
	// only add measurements with exactly same cluster_names into one common group
	if (!check_cluster_names(&measurement_group)) return false;
	if (conf.measurement_group_definition_tool && tool_name_p!=measurement_group.tool_name_p) return false;
	if (conf.measurement_group_definition_groupid && group_p != measurement_group.group_p) return false;
	if (conf.measurement_group_definition_olcdbid && olcdb_p != measurement_group.olcdb_p) return false;
	if (conf.measurement_group_definition_settings && settings_p != measurement_group.settings_p) return false;
	return true;
}


bool measurement_group_t::operator!=(measurement_group_t& measurement_group)
{
	return !operator==(measurement_group);
}

