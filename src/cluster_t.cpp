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
#include "cluster_t.hpp"
#include "measurement.hpp"
#include "measurement_group_t.hpp"


// vector<string> calc_history;

/************** cluster_t ********************/

quantity_t cluster_t::total_sputter_depth()
{
	if (measurement->crater.total_sputter_depths().mean().is_set())
		return measurement->crater.total_sputter_depths().mean();
	
// 	if (measurement->crater.sputter_rate().is_set() && total_sputter_time().is_set())
// 	{
// 		quantity_t tsd =  total_sputter_time()*sputter_rate();
// 		tsd.data[0] = tsd.data.back();
// 		tsd.data.resize(1);
// 		tsd.name = "total_sputter_depth";
// 		return tsd;
// 	}
	return quantity_t();
}
quantity_t cluster_t::total_sputter_time()
{
	return measurement->crater.total_sputter_time();
}

quantity_t cluster_t::sputter_rate()
{
	if (sputter_rate_p.is_set()) return sputter_rate_p;
	if (measurement->crater.sputter_rate_p.is_set()) return measurement->crater.sputter_rate_p;
	if (already_checked_sputter_rate) return {}; // results from previouse calculation are saved in crater
	already_checked_sputter_rate=true;

	
	if (total_sputter_time().is_set() && total_sputter_depth().is_set())
	{
		calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + "total_sputter_time().is_set() && total_sputter_depth().is_set()");
		measurement->crater.sputter_rate_p = total_sputter_depth() / (total_sputter_time());
	}
	else if (depth_at_maximum_concentration().is_set() && equilibrium().intensity().is_set() && equilibrium().sputter_time().is_set())
	{
		cout << measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + "depth_at_maximum_concentration().is_set() && equilibrium().intensity().is_set() && equilibrium().sputter_time().is_set()" << endl;
		calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + "depth_at_maximum_concentration().is_set() && equilibrium().intensity().is_set() && equilibrium().sputter_time().is_set()");
		measurement->crater.sputter_rate_p=depth_at_maximum_concentration() / (equilibrium().intensity().polyfit().max_at_x(equilibrium().sputter_time() ));
	}
	else if (depth_at_maximum_concentration().is_set() && equilibrium().concentration().is_set() && equilibrium().sputter_time().is_set())
	{
		calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + "depth_at_maximum_concentration().is_set() && equilibrium().concentration().is_set() && equilibrium().sputter_time().is_set()");
		measurement->crater.sputter_rate_p=depth_at_maximum_concentration() / (equilibrium().concentration().polyfit().max_at_x(equilibrium().sputter_time() ));
	}
// 	else if (measurement->measurement_group->SRs(measurement->sample_p->matrix).is_set())
// 	{
// 		calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + "measurement->measurement_group->SRs(measurement->sample_p->matrix).is_set()");
// 		measurement->crater.sputter_rate_p = measurement->measurement_group->SRs(measurement->sample_p->matrix).mean();
// 	}
// 	/*just single element matrices in MG? --> SRs should be the same*/
// 	else if (measurement->measurement_group->reference_matrix_isotopes().size()==1)
// 	{
// 		quantity_t SR;
// 		for (auto& M:measurement->measurement_group->measurements)
// 		{
// 			if (M==measurement) continue;
// 			cout << "IN3\n";
// 			if (M->crater.sputter_rate().is_set())
// 			{
// 				cout << "IN4\n";
// 				if (SR.is_set()) SR = SR.add_to_data(M->crater.sputter_rate());
// 				else SR = M->crater.sputter_rate();
// 			}
// 		}
// 		measurement->crater.sputter_rate_p = SR.mean();
// 	}
	if (measurement->crater.sputter_rate_p.is_set()) calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + measurement->crater.sputter_rate_p.to_str());
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
	if (value<=0) return;
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
	if (value<=0) return;
	dose_p.name="dose";
	dose_p.unit="at/cm^2";
	dose_p.dimension = "amount*(length)^(-2)";
	dose_p.data.resize(1);
	dose_p.data[0]=value;
}

void cluster_t::set_maximum_concentration_p(double& value)
{
	if (value<=0) return;
	maximum_concentration_p.name="max(concentration)";
	maximum_concentration_p.unit="at/cm^3";
	maximum_concentration_p.dimension="amount*(length)^(-3)";
	maximum_concentration_p.data.resize(1);
	maximum_concentration_p.data[0]=value;
}

quantity_t cluster_t::maximum_concentration()
{
// 	if (maximum_concentration_p.is_set()) return maximum_concentration_p;
	return maximum_concentration_p;
}

quantity_t cluster_t::dose()
{
	if (already_checked_dose || dose_p.is_set()) return dose_p;
	if (is_reference()) return quantity_t();
	if (equilibrium().concentration().is_set() && equilibrium().sputter_depth().is_set()) 
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
	for (auto& ref_isotope:measurement->sample_p->matrix.isotopes())
	{
		if (leftover_elements({ref_isotope}).size()==0) return ref_isotope;
	}
	return {};
}

quantity_t cluster_t::concentration()
{
	if (already_checked_concentration || concentration_p.is_set()) return concentration_p;
	already_checked_concentration=true;
	
	if (intensity().is_set() && SF().is_set())
	{
		concentration_p=intensity() * SF();
		concentration_p.unit="at/cm^3";
		concentration_p.name="concentration";
	}
	
	/*reference sample from database*/
// 	else if (measurement->load_from_database() && is_reference() && reference_matrix_isotope().is_set() && intensity().is_set())
	else if (measurement->load_from_database() && is_reference() && intensity().is_set())
	{
		if (reference_matrix_isotope().is_set())
			concentration_p = measurement->sample_p->matrix.relative_concentration(reference_matrix_isotope())*intensity()/(intensity().trimmed_mean());
		else 
			concentration_p.data.assign(intensity().data.size(),0);
		concentration_p.unit="at%";
		concentration_p.name="concentration";
		concentration_p.dimension="relative";
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
	
	/*low intensity back ground*/
	if (dose().is_set() && equilibrium().intensity().is_set() && equilibrium().sputter_depth().is_set() && equilibrium().intensity().max().is_set() && equilibrium().intensity().max().data[0]*0.05>intensity().data.back())
	{
		// nur wenn die intensität am ende des profils auf unter 5% des maximums fällt
		SF_p = dose() / (equilibrium().intensity().integrate(equilibrium().sputter_depth()));
		SF_p = SF_p * 1E7; // nm -> cm
		SF_p.unit = "at/cm^3 / (cnt/s)";
	}
	
	/*RSF from other measurements*/
	else if (reference_intensity().is_set() && RSF().is_set())
	{
		if(conf.standard_reference_intensity_calculation_method=="pbp") SF_p = RSF() / reference_intensity();
		else if(conf.standard_reference_intensity_calculation_method=="median") SF_p = RSF() / reference_intensity().median();
		else if(conf.standard_reference_intensity_calculation_method=="mean") SF_p = RSF() / reference_intensity().mean();
		else if(conf.standard_reference_intensity_calculation_method=="trimmed_mean") SF_p = RSF() / reference_intensity().trimmed_mean();
		else if(conf.standard_reference_intensity_calculation_method=="gastwirth") SF_p = RSF() / reference_intensity().gastwirth();
		else if(conf.standard_reference_intensity_calculation_method=="quantile50") SF_p = RSF() / reference_intensity().quantile(0.5);
		else if(conf.standard_reference_intensity_calculation_method=="quantile75") SF_p = RSF() / reference_intensity().quantile(0.75);
		else SF_p = RSF() / reference_intensity().trimmed_mean();
	}
	
	/*use maximum concentration and intensity*/
	else if (maximum_concentration().is_set() && equilibrium().intensity().polyfit().max().is_set())
		SF_p = maximum_concentration() / (equilibrium().intensity().polyfit().max());
	
	
	/*high intensity back ground*/
	else if (dose().is_set() && equilibrium().intensity().is_set() && equilibrium().sputter_depth().is_set())
	{
		quantity_t bg = equilibrium().intensity();
		bg.data.resize(1);
		bg.data[0] = intensity().data.back();
		SF_p = dose() / ((equilibrium().intensity()-bg).integrate(equilibrium().sputter_depth()));
		SF_p.unit = "at/cm^3 / (cnt/s)";
		SF_p = SF_p * 1E7; // nm -> cm
		
	}
	if (SF_p.is_set()) calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + SF_p.to_str());
	return SF_p;
}

quantity_t cluster_t::RSF()
{
	if (RSF_p.is_set()) return RSF_p;
// 	if (measurement->reference_clusters().size()==1 && measurement->measurement_group->RSFs(name()).is_set()) return measurement->measurement_group->RSFs(name()).mean();
	if (already_checked_RSF) return {};

	already_checked_RSF=true;
// 	cout << measurement->filename_p->filename_with_path() << "\t" << name() << endl;
	
	if (reference_intensity().is_set() && SF().is_set())
	{
// 		cout << "SF\n";
		RSF_p = SF() * reference_intensity().trimmed_mean();
		if (RSF_p.is_set()) calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + RSF_p.to_str());
		RSF_p.dimension = "amount*(length)^(-3)";
		RSF_p.unit = "at/cm^3";
		if (RSF_p.is_set()) calc_history.push_back(measurement->filename_p->filename_with_path()+"\t"+name()+"\t" + RSF_p.to_str());
		return RSF_p;
	}
	
	// look in other measurements within this group for the RSF of this cluster (and matrix)
	else if (measurement->measurement_group->RSFs(*this).is_set())
		RSF_p = measurement->measurement_group->RSFs(*this).mean();
	
	/*just single element matrices in MG? --> RSFs should be the same*/
	else if (measurement->measurement_group->reference_matrix_isotopes().size()==1)
	{
		quantity_t RSFs;
		for (auto& M:measurement->measurement_group->measurements)
		{
			if (M->clusters.find(name())==M->clusters.end()) continue;
			if (M->clusters[name()].RSF().is_set())
			{
				if (RSFs.is_set()) RSFs = RSFs.add_to_data(M->clusters[name()].RSF());
				else RSFs = M->clusters[name()].RSF();
			}
		}
		RSF_p = RSFs.mean();
	}
	
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
	if (quantities_reduced_by_equilibrium_starting_pos) reference_intensity_p = reduce_quantity_by_equlibrium_starting_pos(reference_intensity_p);
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
	if (quantity.data.size()<=1) return quantity; // no vector
	quantity_t result=quantity;
	if (result.is_set()) 
	{
		result.data.erase(result.data.begin(),result.data.begin()+equilibrium_starting_pos-1);
	}
// 	cout << "reduce_quantity_by_equlibrium_starting_pos von " << quantity.name << " data.size()=" << quantity.data.size() <<endl;
	return result;
}

void cluster_t::reduce_quantities_by_equlibrium_starting_pos(cluster_t& reduces_this)
{
	
	reduces_this.concentration_p=reduces_this.reduce_quantity_by_equlibrium_starting_pos(concentration_p);
	reduces_this.intensity_p=reduces_this.reduce_quantity_by_equlibrium_starting_pos(intensity_p);
	reduces_this.reference_intensity_p=reduces_this.reduce_quantity_by_equlibrium_starting_pos(reference_intensity_p);
	reduces_this.SF_p=reduces_this.reduce_quantity_by_equlibrium_starting_pos(SF_p);
	reduces_this.RSF_p=reduces_this.reduce_quantity_by_equlibrium_starting_pos(RSF_p);
	reduces_this.dose_p=reduces_this.reduce_quantity_by_equlibrium_starting_pos(dose_p);
	
	reduces_this.sputter_time_p=reduces_this.reduce_quantity_by_equlibrium_starting_pos(measurement->crater.sputter_time_p);
	reduces_this.sputter_depth_p=reduces_this.reduce_quantity_by_equlibrium_starting_pos(measurement->crater.sputter_depth_p);
	reduces_this.sputter_rate_p=reduces_this.reduce_quantity_by_equlibrium_starting_pos(measurement->crater.sputter_rate_p);
	
	reduces_this.quantities_reduced_by_equilibrium_starting_pos = true;
	
	/*renaming*/
	
	if (reduces_this.concentration_p.is_set()) reduces_this.concentration_p.name="equilibrium("+reduces_this.concentration_p.name+")";
	if (reduces_this.intensity_p.is_set()) reduces_this.intensity_p.name="equilibrium("+reduces_this.intensity_p.name+")";
	if (reduces_this.reference_intensity_p.is_set()) reduces_this.reference_intensity_p.name="equilibrium("+reduces_this.reference_intensity_p.name+")";
	if (reduces_this.SF_p.is_set()) reduces_this.SF_p.name="equilibrium("+reduces_this.SF_p.name+")";
	if (reduces_this.RSF_p.is_set()) reduces_this.RSF_p.name="equilibrium("+reduces_this.RSF_p.name+")";
	if (reduces_this.dose_p.is_set()) reduces_this.dose_p.name="equilibrium("+reduces_this.dose_p.name+")";
	
	if (reduces_this.sputter_time_p.is_set()) reduces_this.sputter_time_p.name="equilibrium("+reduces_this.sputter_time_p.name+")";
	if (reduces_this.sputter_depth_p.is_set()) reduces_this.sputter_depth_p.name="equilibrium("+reduces_this.sputter_depth_p.name+")";
	if (reduces_this.sputter_rate_p.is_set()) reduces_this.sputter_rate_p.name="equilibrium("+reduces_this.sputter_rate_p.name+")";
}



cluster_t cluster_t::equilibrium()
{
	/*already in equilibrium*/
	if (quantities_reduced_by_equilibrium_starting_pos) return *this;
	cluster_t result=*this;
// 	print(name());
// 	intensity_p.to_screen();
// 	intensity().to_screen();
// 	return *this;
	
	if (equilibrium_starting_pos==-1)
	{
// 		int offset = 0;
		equilibrium_starting_pos=0;
		vector<double> Y;
		
		if (intensity().is_set())
			Y = intensity().filter_gaussian(5,4).data; 
		else if (concentration().is_set())
			Y = concentration().filter_gaussian(5,4).data; 
		else return result;
		
// 		double treshold = statistics::get_quantile_from_Y(Y,0.01);
		double treshold = statistics::get_mad_from_Y(Y)/2;
		double median = statistics::get_median_from_Y(Y);
		set<int> extrema_idx;
// 		extrema_idx.insert(0);
		vector<int> maxIdx, minIdx;
		
// 		for (int i=0; i<Y.size();i++ )
// 		{
// 			if (Y[i]==0)
// 			{
// 				Y[i]=1;
// 				offset = i;
// 			} 
// 			else break;
// 		}
			
		if (!statistics::get_extrema_indices(maxIdx,minIdx,Y,treshold))
		{
// 			cout << endl << "!statistics::get_extrema_indices(maxIdx,minIdx,Y,statistics::get_mad_from_Y(Y)*2)" << endl;
// 			cout << name() << " type C1" << endl;
			if (is_reference()) equilibrium_starting_pos = statistics::get_index_for_next_value_within_treshold(Y,median-treshold/2,median+treshold/2,1);
// 			else equilibrium_starting_pos = measurement->equilibrium_starting_pos();
			else equilibrium_starting_pos = 0;
		}
		//type C
		else if (minIdx.size()==0 && maxIdx.size()==1) // just the global maximum
		{
// 			cout << name() << " type C2" << endl;
			if (is_reference()) equilibrium_starting_pos = statistics::get_index_for_next_value_within_treshold(Y,median-treshold/2,median+treshold/2,1);
// 			else equilibrium_starting_pos = measurement->equilibrium_starting_pos();
			else equilibrium_starting_pos = 0;
		}
		else
		{
			/*remove right sided*/
			for (auto& m:maxIdx)
				if (m>0.5*Y.size()) m=0;
			for (auto& m:minIdx)
				if (m>0.5*Y.size()) m=0;
				
			set<int> maxIdx_set (maxIdx.begin(),maxIdx.end());
			set<int> minIdx_set (minIdx.begin(),minIdx.end());
			
			/*remove trivials*/
			maxIdx_set.erase(0);
			maxIdx_set.erase(1);
			minIdx_set.erase(0);
			minIdx_set.erase(1);
			
// 			print("maxIdx_set: " + name());
// 			print(maxIdx_set);
// 			print("minIdx_set: " + name());
// 			print(minIdx_set);
			
			//type E D
			if (minIdx_set.size()==0 && maxIdx_set.size()==0)
			{
// 				cout << name() << " type E or D" << endl;
				if (is_reference()) equilibrium_starting_pos = statistics::get_index_for_next_value_within_treshold(Y,median-treshold/2,median+treshold/2,1);
// 				else equilibrium_starting_pos = measurement->equilibrium_starting_pos();
				else equilibrium_starting_pos = 0;
			}
			// type G H
			else if (minIdx_set.size()==0 && maxIdx_set.size()!=0 )
			{
				if (is_reference()) equilibrium_starting_pos = statistics::get_index_for_next_value_within_treshold(Y,median-treshold/2,median+treshold/2,*maxIdx_set.begin());
				else equilibrium_starting_pos = 0;
// 				equilibrium_starting_pos=*maxIdx_set.begin(); 
// 				cout << name() << " type G or H" << endl;
			}
			//type A B F
			else 
			{
				equilibrium_starting_pos=*minIdx_set.begin(); 
				/* check total signal sum for miss interpretation */
				double sum=0;
				double sum_equilibrium=0;
				for (int i=0;i<Y.size();i++)
					sum+=Y[i];
				for (int i=equilibrium_starting_pos;i<Y.size();i++)
					sum_equilibrium+=Y[i];
				if (sum_equilibrium/sum<0.7)
					equilibrium_starting_pos=0;
// 				cout << name() << " type A or B or F" << endl;
			}
		}
// 		cout << name() <<" equilibrium_starting_pos = " << equilibrium_starting_pos << endl;
	}
	
	result.equilibrium_starting_pos=equilibrium_starting_pos;
	reduce_quantities_by_equlibrium_starting_pos(result);
	return result;
}



std::__cxx11::string cluster_t::to_str(std::__cxx11::string prefix)
{
	stringstream ss;
	ss << prefix << name() << endl;
	ss << prefix << "{" << endl;
	if (equilibrium().sputter_depth().is_set())
		ss  <<std::scientific << prefix+"\tequilibrium_depth: " << equilibrium().sputter_depth().data[0] << " " << equilibrium().sputter_depth().unit << endl;
	else if (equilibrium().sputter_time().is_set())
		ss <<std::scientific  << prefix+"\tequilibrium_sputter_time: " << equilibrium().sputter_time().data[0] << " " << equilibrium().sputter_time().unit << endl;
	if (dose().is_set()) 
		ss <<std::scientific  << prefix << "\tDB_dose(): " << dose().to_str() << endl;
	if (concentration().is_set() && sputter_depth().is_set())
		ss <<std::scientific  << prefix << "\tdose_integral(): " << concentration().integrate(sputter_depth()).to_str() << endl;
	if ( equilibrium().concentration().is_set())
	{
		if (equilibrium().sputter_time().is_set())
			ss  <<std::scientific  << prefix << "\tequilibrium_concentration_integral(sputter_time): " << equilibrium().concentration().integrate(equilibrium().sputter_time()).to_str() << endl;
		if (equilibrium().sputter_depth().is_set())
			ss  <<std::scientific << prefix << "\tequilibrium_concentration_integral(sputter_depth): " << equilibrium().concentration().integrate(equilibrium().sputter_depth()).to_str() << endl;
	}
	if ( equilibrium().intensity().is_set())
	{
		if (equilibrium().sputter_time().is_set())
			ss <<std::scientific  << prefix << "\tequilibrium_intensity_integral(sputter_time): " << equilibrium().intensity().integrate(equilibrium().sputter_time()).to_str() << endl;
		if (equilibrium().sputter_depth().is_set())
			ss  <<std::scientific << prefix << "\tequilibrium_intensity_integral(sputter_depth): " << equilibrium().intensity().integrate(equilibrium().sputter_depth()).to_str() << endl;
	}
	
	if (SF().is_set())
		ss  <<std::scientific  << prefix << "\tSF: " << SF().to_str() << endl;
	if (RSF().is_set()) 
		ss  <<std::scientific  << prefix << "\tRSF: " << RSF().to_str() << endl;
	
	if (maximum_concentration().is_set())
		ss  <<std::scientific  << prefix << "\tDB_max_concentration: " << maximum_concentration().to_str() << endl;
	if (depth_at_maximum_concentration().is_set())
		ss  <<std::scientific  << prefix << "\tDB_depth_at_max_concentration: " << depth_at_maximum_concentration().to_str() << endl;
	
	if (equilibrium().concentration().polyfit(17).max().is_set())
	{
		ss  <<std::scientific  << prefix << "\tmax_concentration: " << equilibrium().concentration().polyfit(17).max().to_str() << endl;
		if (sputter_time().is_set())
			ss <<std::scientific  << prefix << "\tsputter_time_at_max_concentration: " << equilibrium().concentration().polyfit(17).max_at_x(equilibrium().sputter_time()).to_str() << endl;
		if (sputter_depth().is_set())
			ss <<std::scientific  << prefix << "\tdepth_at_max_concentration: " << equilibrium().concentration().polyfit(17).max_at_x(equilibrium().sputter_depth()).to_str() << endl;
	}
	if (equilibrium().intensity().polyfit(17).max().is_set())
	{
		ss <<std::scientific  << prefix << "\tmax_intensity: " << equilibrium().intensity().polyfit(17).max().to_str() << endl;
		if (sputter_time().is_set())
			ss <<std::scientific  << prefix << "\tsputter_time_at_max_intensity: " << equilibrium().intensity().polyfit(17).max_at_x(equilibrium().sputter_time()).to_str() << endl;
		if (sputter_depth().is_set())
			ss <<std::scientific  << prefix << "\tdepth_at_max_intensity: " << equilibrium().intensity().polyfit(17).max_at_x(equilibrium().sputter_depth()).to_str() << endl;
	}
	ss << prefix << "}" << endl;
	return ss.str();
}
















