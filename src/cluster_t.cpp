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
#include "cluster_t.hpp"
#include "measurement.hpp"
#include "measurement_group_t.hpp"


// vector<string> calc_history;

/************** cluster_t ********************/

quantity_t cluster_t::total_sputter_depth()
{
	return measurement->crater.total_sputter_depths().mean();
}
quantity_t cluster_t::total_sputter_time()
{
	return measurement->crater.total_sputter_time();
}

quantity_t cluster_t::sputter_rate()
{
	if (sputter_rate_p.is_set()) return sputter_rate_p;
	if (measurement->crater.sputter_rate_p.is_set()) return measurement->crater.sputter_rate_p;
	if (already_checked_sputter_rate) return {};
	already_checked_sputter_rate=true;

	
	if (total_sputter_time().is_set() && total_sputter_depth().is_set())
	{
		measurement->crater.sputter_rate_p = total_sputter_depth() / (total_sputter_time());
	}
	else if (depth_at_maximum_concentration().is_set() && equilibrium().intensity().is_set() && equilibrium().sputter_time().is_set())
	{
		measurement->crater.sputter_rate_p=depth_at_maximum_concentration() / (equilibrium().intensity().polyfit().max_at_x(equilibrium().sputter_time() ));
	}
	else if (depth_at_maximum_concentration().is_set() && equilibrium().concentration().is_set() && equilibrium().sputter_time().is_set())
	{
		measurement->crater.sputter_rate_p=depth_at_maximum_concentration() / (equilibrium().concentration().polyfit().max_at_x(equilibrium().sputter_time() ));
	}
	else if (measurement->measurement_group->SRs().is_set())
	{
		measurement->crater.sputter_rate_p = measurement->measurement_group->SRs().mean();
	}
	
	if (measurement->crater.sputter_rate_p.is_set())calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + measurement->crater.sputter_rate_p.to_str());
	measurement->crater.sputter_rate_p.unit="nm/s";
	measurement->crater.sputter_rate_p.name="sputter_rate";
	return measurement->crater.sputter_rate_p;
}

quantity_t cluster_t::sputter_depth()
{
	if (sputter_depth_p.is_set()) return sputter_depth_p;
	if (already_checked_sputter_depth) return measurement->crater.sputter_depth_p;
	already_checked_sputter_depth=true;
	if (measurement->crater.sputter_depth().is_set()) return measurement->crater.sputter_depth_p;
	
	if (sputter_rate().is_set() && sputter_time().is_set()) 
	{
		if (sputter_rate().data.size()==1) 
		{
			measurement->crater.sputter_depth_p = sputter_time() * sputter_rate();
		}
		else
		{
			measurement->crater.sputter_depth_p = sputter_rate().integrate_pbp(sputter_time());
		}
	}
	if (measurement->crater.sputter_depth_p.is_set()) calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + measurement->crater.sputter_depth_p.to_str());
	measurement->crater.sputter_depth_p.unit="nm";
	measurement->crater.sputter_depth_p.name="sputter_depth";
	return measurement->crater.sputter_depth_p;
}

quantity_t cluster_t::sputter_time()
{
	if (sputter_time_p.is_set()) return sputter_time_p;
	if (already_checked_sputter_time) return measurement->crater.sputter_time_p;
	already_checked_sputter_time=true;
	if (measurement->crater.sputter_time_p.is_set()) 
	{
		if (measurement->crater.sputter_time_p.is_set()) calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + measurement->crater.sputter_time_p.to_str());
		return measurement->crater.sputter_time_p;
	}
	
	
// 	else if (sputter_depth().is_set() && sputter_rate().is_set())
// 		measurement->crater.sputter_time_p = sputter_depth() / sputter_rate();
	
	if (measurement->crater.sputter_time_p.is_set()) calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + measurement->crater.sputter_time_p.to_str());
	return measurement->crater.sputter_time_p;
}

quantity_t cluster_t::depth_at_maximum_concentration()
{
	if (depth_at_maximum_concentration_p.is_set()) return depth_at_maximum_concentration_p;
	else if (measurement->load_from_database() && depth_at_maximum_concentration_p.is_set()) 
	{
		if (depth_at_maximum_concentration_p.is_set()) calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + depth_at_maximum_concentration_p.to_str());
		return depth_at_maximum_concentration_p;
	}
	if (depth_at_maximum_concentration_p.is_set()) calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + depth_at_maximum_concentration_p.to_str());
	return depth_at_maximum_concentration_p;
}

void cluster_t::set_depth_at_maximum_concentration_p(quantity_t& quantity)
{
	depth_at_maximum_concentration_p = quantity;
}

void cluster_t::set_depth_at_maximum_concentration_p(double& value)
{
	depth_at_maximum_concentration_p.name="depth(max(concentration))";
	depth_at_maximum_concentration_p.unit="nm";
	depth_at_maximum_concentration_p.dimension="length";
	depth_at_maximum_concentration_p.data.resize(1);
	depth_at_maximum_concentration_p.data[0]=value;
}

void cluster_t::set_dose_p(quantity_t& quantity)
{
	dose_p = quantity;
}

void cluster_t::set_dose_p(double& value)
{
	dose_p.name="dose";
	dose_p.unit="at/cm^2";
	dose_p.dimension = "amount*(length)^(-2)";
	dose_p.data.resize(1);
	dose_p.data[0]=value;
}

void cluster_t::set_maximum_concentration_p(double& value)
{
	maximum_concentration_p.name="max(concentration)";
	maximum_concentration_p.unit="at/cm^3";
	maximum_concentration_p.dimension="amount*(length)^(-3)";
	maximum_concentration_p.data.resize(1);
	maximum_concentration_p.data[0]=value;
}

quantity_t cluster_t::maximum_concentration()
{
	return maximum_concentration_p;
}

quantity_t cluster_t::dose()
{
	if (already_checked_dose || dose_p.is_set()) return dose_p;
	else if (equilibrium().concentration().is_set() && equilibrium().sputter_depth().is_set()) 
	{
		dose_p=equilibrium().concentration().integrate(equilibrium().sputter_depth());
		if (dose_p.is_set()) calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + dose_p.to_str());
	}
	else if (measurement->load_from_database() && dose_p.is_set()) 
	{
		if (dose_p.is_set()) calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + dose_p.to_str());
		return dose_p;
	}
	already_checked_dose=true;
	
	return dose_p;
}


isotope_t cluster_t::reference_matrix_isotope()
{
	for (auto& ref_isotope:measurement->sample_p->matrix.isotopes)
	{
		if (leftover_elements({ref_isotope}).size()==0) return ref_isotope;
	}
	return {};
}

quantity_t cluster_t::concentration()
{
	if (already_checked_concentration || concentration_p.is_set()) return concentration_p;
	already_checked_concentration=true;
	
	/*reference sample from database*/
// 	if (is_reference() && measurement->load_from_database() && reference_matrix_isotope().is_set() && intensity().is_set())
// 		concentration_p = measurement->sample_p->matrix.relative_concentration(reference_matrix_isotope())*intensity()/intensity().median();
	
	if (intensity().is_set() && SF().is_set())
	{
		concentration_p=intensity() * SF();
		concentration_p.unit="at/cm^3";
		concentration_p.name="concentration";
// 		if (concentration_p.is_set()) calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + concentration_p.to_str());
// 		return concentration_p;
	}
	
	if (concentration_p.is_set()) calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + concentration_p.to_str());
	return concentration_p;
}

quantity_t cluster_t::intensity()
{
// 	cout << "quantity_t cluster_t::intensity()" << endl;
	if (already_checked_intensity || intensity_p.is_set()) return intensity_p;
	already_checked_intensity=true;
// 	else if (concentration().is_set() && SF().is_set())
// 		intensity_p = concentration() / (SF());
	if (intensity_p.is_set()) calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + intensity_p.to_str());
	return {};
}

quantity_t cluster_t::SF()
{
	// will be iterated multiple times because of RSF calculation for measurement_group
	if (/*already_checked_SF || */SF_p.is_set()) return SF_p;
// 	already_checked_SF=true;
// 	else if (concentration().is_set() && intensity().is_set())
// 		SF_p = concentration().divide_by(intensity());
	
	if (dose().is_set() && equilibrium().intensity().is_set() && equilibrium().sputter_depth().is_set() && equilibrium().intensity().max().is_set() && equilibrium().intensity().max().data[0]*0.05>intensity().data.back())
	{
		// nur wenn die intensität am ende des profils auf unter 5% des maximums fällt
		SF_p = dose() / (equilibrium().intensity().integrate(equilibrium().sputter_depth()));
		SF_p = SF_p * 1E7; // nm -> cm
		SF_p.unit = "at/cm^3 / (cnt/s)";
// 		cout << "\n+++SF_p.data.size() = " << SF_p.data.size() << endl;
	}
	
	else if (maximum_concentration().is_set() && equilibrium().intensity().polyfit().max().is_set())
	{
		SF_p = maximum_concentration() / equilibrium().intensity().polyfit().max();
// 		cout << "\n---SF_p.data.size() = " << SF_p.data.size() << endl;
	}
	
	else if (reference_intensity().is_set() && RSF().is_set())
	{
		if(conf.standard_reference_intensity_calculation_method=="pbp") SF_p = RSF() / reference_intensity();
		else if(conf.standard_reference_intensity_calculation_method=="median") SF_p = RSF() / reference_intensity().median();
		else if(conf.standard_reference_intensity_calculation_method=="mean") SF_p = RSF() / reference_intensity().mean();
		else if(conf.standard_reference_intensity_calculation_method=="quantile50") SF_p = RSF() / reference_intensity().quantile(0.5);
		else if(conf.standard_reference_intensity_calculation_method=="quantile75") SF_p = RSF() / reference_intensity().quantile(0.75);
		else RSF() / reference_intensity();
	}
	
	if (SF_p.is_set()) calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + SF_p.to_str());
	return SF_p;
}

quantity_t cluster_t::RSF()
{
	if (RSF_p.is_set()) return RSF_p;
	if (measurement->reference_clusters().size()==1 && measurement->measurement_group->RSFs(name()).is_set()) return measurement->measurement_group->RSFs(name()).mean();
	if (already_checked_RSF) return {};

	already_checked_RSF=true;
// 	cout << measurement->filename_p->filename_with_path() << "\t" << name() << endl;
	
	if (reference_intensity().is_set() && SF().is_set())
	{
// 		cout << "SF\n";
		RSF_p = SF() * reference_intensity().median();
		if (RSF_p.is_set()) calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + RSF_p.to_str());
		RSF_p.dimension = "amount*(length)^(-3)";
		RSF_p.unit = "at/cm^3";
// 		return RSF_p;
	}

	// look in other measurements within this group for the RSF of this cluster
// 	if (measurement->measurement_group->RSFs(name()).is_set())
// 		RSF_p = measurement->measurement_group->RSFs(name()).mean();
	
	if (RSF_p.is_set()) calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + RSF_p.to_str());
	return RSF_p;
}

bool cluster_t::is_reference()
{
	for (auto& reference_cluster:measurement->reference_clusters())
		if (reference_cluster->name() == name()) return true;
	return false;
}

quantity_t cluster_t::reference_intensity()
{
	if (reference_intensity_p.is_set()) return reference_intensity_p;
// 	reference_intensity_p.data.clear();
	if (measurement->reference_clusters().size()==0) return {};
	
	reference_intensity_p = measurement->reference_clusters()[0]->intensity();
	for (int i=1;i<measurement->reference_clusters().size();i++)
		reference_intensity_p=reference_intensity_p + measurement->reference_clusters()[i]->intensity();
// 	cout << "reference_intensity_p.data.size()A=" << reference_intensity_p.data.size() << endl;
	reference_intensity_p = reduce_quantity_by_equlibrium_starting_pos(reference_intensity_p);
// 	cout << "reference_intensity_p.data.size()B=" << reference_intensity_p.data.size() << endl;
	if (reference_intensity_p.is_set()) calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + reference_intensity_p.to_str());
// 	cout << "reference_intensity_p.data.size()=" << reference_intensity_p.data.size() << endl;
// 	cout << "equilibrium_starting_pos = " << equilibrium_starting_pos << "\tname=" << name() << endl;
	reference_intensity_p.name = "reference_intensity";
	return reference_intensity_p;
}




quantity_t cluster_t::mass()
{
	quantity_t mass_p;
	if (isotopes.size()==0) return mass_p;
	mass_p=isotopes[0].mass();
	for (vector<isotope_t>::iterator it=next(isotopes.begin());it!=isotopes.end();++it)
	{
		mass_p = mass_p * (it->mass());
	}
	return mass_p;
}

quantity_t cluster_t::abundance()
{
	quantity_t abundance_p;
	if (isotopes.size()==0) return abundance_p;
	abundance_p=isotopes[0].abundance();
	for (vector<isotope_t>::iterator it=next(isotopes.begin());it!=isotopes.end();++it)
	{
		abundance_p = abundance_p * (it->abundance());
	}
	return abundance_p;
}

string cluster_t::name()
{
	if (name_p!="") return name_p;
	if (isotopes.size()==0) return "NO NAME";
	/// sort isotopes for lowest mass 1st
	std::sort(isotopes.begin(),isotopes.end(),isotope_t::sort_lower_mass);
	for (int i=0;i<isotopes.size();i++)
		name_p+=isotopes[i].name()+" ";
	name_p.erase(name_p.end()-1,name_p.end()); // remove the last space
	return name_p;
}



void cluster_t::to_screen(string prefix)
{
	cout << prefix << name() << endl;
	if (isotopes.size()>0) cout << prefix << "\t" << "isotopes:" << endl;
	for (int i=0;i<isotopes.size();i++)
	{
		isotopes[i].to_screen(prefix +"\t");
	}
	if (intensity_p.is_set()) intensity_p.to_screen(prefix+"\t");
	if (concentration_p.is_set()) concentration_p.to_screen(prefix+"\t");
 	if (dose_p.is_set()) dose_p.to_screen(prefix+"\t");
	if (maximum_concentration_p.is_set()) maximum_concentration_p.to_screen(prefix+"\t");
	if (depth_at_maximum_concentration_p.is_set()) depth_at_maximum_concentration_p.to_screen(prefix+"\t");
	
	return;
}

// bool cluster_t::is_element_in_cluster(vector<isotope_t>& isotopes_p)
// {
// 	bool found;
// 	for (auto& isotope:isotopes_p)
// 	{
// 		found = false;
// 		for (auto& isotope_in_cluster:isotopes)
// 		{
// 			if (isotope_in_cluster.symbol == isotope.symbol)
// 			{
// 				return true;
// // 				break;
// 			}
// 		}
// 		if (!found) return false;
// 	}
// 	return true;
// }
// 
// bool cluster_t::is_isotope_in_cluster(vector<isotope_t>& isotopes_p)
// {
// 	bool found;
// 	for (auto& isotope:isotopes_p)
// 	{
// 		found = false;
// 		for (auto& isotope_in_cluster:isotopes)
// 		{
// 			if (isotope_in_cluster.symbol == isotope.symbol && isotope_in_cluster.nucleons==isotope.nucleons)
// 			{
// 				return true;
// // 				break;
// 			}
// 		}
// 		if (!found) return false;
// 	}
// 	return true;
// }

bool cluster_t::is_element_in_cluster ( isotope_t isotope )
{
	for (auto& is:isotopes)
	{
		if (isotope.symbol==is.symbol) return true;
	}
	return false;
}

bool cluster_t::is_isotope_in_cluster( isotope_t isotope )
{
	for (auto& is:isotopes)
	{
		if (isotope.symbol==is.symbol && isotope.nucleons == is.nucleons) return true;
	}
	return false;
}

bool cluster_t::are_elements_in_cluster(vector<isotope_t> isotopes_p)
{
	bool found;
	for (auto& isotope:isotopes_p)
	{
		found = false;
		for (auto& isotope_in_cluster:isotopes)
		{
			if (isotope_in_cluster.symbol == isotope.symbol)
			{
				found = true;
				break;
			}
		}
		if (!found) return false;
	}
	return true;
}
bool cluster_t::are_isotopes_in_cluster(vector<isotope_t> isotopes_p)
{
	bool found;
	for (auto& isotope:isotopes_p)
	{
		found = false;
		for (auto& isotope_in_cluster:isotopes)
		{
			if (isotope_in_cluster.symbol == isotope.symbol && isotope_in_cluster.nucleons==isotope.nucleons)
			{
				found = true;
				break;
			}
		}
		if (!found) return false;
	}
	return true;
}

vector<isotope_t> cluster_t::leftover_isotopes(vector<isotope_t> isotopes_p)
{
	vector<isotope_t> leftovers = isotopes;
	for (auto& isotope:isotopes_p)
	{
		for (int i=0;i<leftovers.size();i++)
		{
			if (leftovers[i].symbol == isotope.symbol && leftovers[i].nucleons==isotope.nucleons)
			{
				leftovers.erase(leftovers.begin()+i);
				break;
			}
		}
	}
	return leftovers;
}
vector<isotope_t> cluster_t::leftover_elements(vector<isotope_t> isotopes_p)
{
	vector<isotope_t> leftovers = isotopes;
	for (auto& isotope:isotopes_p)
	{
		for (int i=0;i<leftovers.size();i++)
		{
			if (leftovers[i].symbol == isotope.symbol)
			{
				leftovers.erase(leftovers.begin()+i);
				break;
			}
		}
	}
	return leftovers;
}

quantity_t cluster_t::reduce_quantity_by_equlibrium_starting_pos(quantity_t& quantity)
{
	if (equilibrium_starting_pos<1) return quantity;
	if (quantity.data.size()<=1) return quantity;
	quantity_t result=quantity;
	if (result.is_set()) 
	{
		result.data.erase(result.data.begin(),result.data.begin()+equilibrium_starting_pos-1);
	}
// 	cout << "reduce_quantity_by_equlibrium_starting_pos von " << quantity.name << " data.size()=" << quantity.data.size() <<endl;
	return result;
}


cluster_t cluster_t::equilibrium(bool use_local_cluster_minimum_at_start)
{
	/*already in equilibrium*/
	if (equilibrium_starting_pos>0) return *this;
	cluster_t result=*this;
	result.equilibrium_starting_pos=measurement->equilibrium_starting_pos();
	
	if (use_local_cluster_minimum_at_start) 
	{
	
	/*******************************/
	/// get local minimum of non-reference-clusters in vicinity of measurement->equilibrium_starting_pos
	vector<double> local_minima_indices, Y;
	vector<int> l_ext;
	
	if (intensity().is_set()) Y=intensity().moving_window_median(5).data;
	else  Y=concentration().moving_window_median(5).data;
// 	// skip low intensity/noisy clusters
	
	l_ext = statistics::get_local_minima_indices_by_triangulation(Y,2,4); // 0.1*Y[statistics::get_min_index_from_Y(Y)]

	/// filter local_minima_indices for positions
	for (auto& min:l_ext)
	{
		/*black magic*/
		if (Y[result.equilibrium_starting_pos]>Y[min] && min<15*result.equilibrium_starting_pos && min>0.9*result.equilibrium_starting_pos && (min<Y.size()*0.25 || min<15*result.equilibrium_starting_pos )) 
		{
			local_minima_indices.push_back(min);
		}
	}
	for (auto& local_minimum_index:local_minima_indices)
	{
		if (Y[result.equilibrium_starting_pos] > Y[local_minimum_index]) result.equilibrium_starting_pos=local_minimum_index;
	}
	}
	
	result.concentration_p=result.reduce_quantity_by_equlibrium_starting_pos(concentration_p);
	result.intensity_p=result.reduce_quantity_by_equlibrium_starting_pos(intensity_p);
	result.reference_intensity_p=result.reduce_quantity_by_equlibrium_starting_pos(reference_intensity_p);
	
	result.SF_p=result.reduce_quantity_by_equlibrium_starting_pos(SF_p);
	result.RSF_p=result.reduce_quantity_by_equlibrium_starting_pos(RSF_p);
	result.dose_p=result.reduce_quantity_by_equlibrium_starting_pos(dose_p);
	
	result.sputter_time_p=result.reduce_quantity_by_equlibrium_starting_pos(measurement->crater.sputter_time_p);
	result.sputter_depth_p=result.reduce_quantity_by_equlibrium_starting_pos(measurement->crater.sputter_depth_p);
	result.sputter_rate_p=result.reduce_quantity_by_equlibrium_starting_pos(measurement->crater.sputter_rate_p);
	return result;
}

