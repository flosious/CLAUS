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

// #define DEBUG
#include "crater_t.hpp"
#include "cluster_t.hpp"
#include "export.hpp"

void crater_t::to_screen(string prefix)
{
	cout << prefix << "crater" << endl;
	if(sputter_time().data.size()>0) sputter_time().to_screen(prefix+"\t");
	if(global_sputter_time().data.size()>0) global_sputter_time().to_screen(prefix+"\t");
	if(sputter_depth().data.size()>0) sputter_depth().to_screen(prefix+"\t");
	if(sputter_rate().data.size()>0) sputter_rate().to_screen(prefix+"\t");
	if(total_sputter_depths_from_filename().data.size()>0) total_sputter_depths_from_filename().to_screen(prefix+"\t");
	if(total_sputter_depths_from_linescans().data.size()>0) total_sputter_depths_from_linescans().to_screen(prefix+"\t");
	if(total_sputter_time().data.size()>0) total_sputter_time().to_screen(prefix+"\t");
	return;
}

string crater_t::to_str(string prefix)
{
	stringstream ss;
	ss << prefix << "crater" << endl;
	ss << prefix << "{" << endl;
	if(sputter_time().is_set()) 
		ss << prefix << "\t" << sputter_time().to_str() << endl;
	if(sputter_depth().is_set()) 
		ss << prefix << "\t"<< sputter_depth().to_str() << endl;
	if(sputter_rate().is_set()) 
		ss << prefix << "\t"<< sputter_rate().to_str() << endl;
	if(global_sputter_time().is_set()) 
		ss << prefix << "\t"<< global_sputter_time().to_str() << endl;
	if(total_sputter_time().is_set()) 
		ss << prefix << "\t"<< total_sputter_time().to_str() << endl;
	if(total_sputter_depths_from_filename().is_set()) 
		ss << prefix << "\tfilename: "<< total_sputter_depths_from_filename().to_str() << endl;
	if(total_sputter_depths_from_linescans().is_set()) 
		ss << prefix << "\tlinescans: "<< total_sputter_depths_from_linescans().to_str() << endl;
	for (auto& ls:linescans)
		ss << prefix << "\t\t" << ls.to_str() << endl;
	ss << prefix << "}";
	return ss.str();
}

quantity_t crater_t::total_sputter_depths()
{
	if (total_sputter_depths_p.is_set()) return total_sputter_depths_p;
	/// this prefers manual input over automatically calculated 
	if (total_sputter_depths_from_filename().is_set()) return total_sputter_depths_from_filename(); // manually
	if (total_sputter_depths_from_linescans().is_set()) return total_sputter_depths_from_linescans();
	if (sputter_rate().is_set() && total_sputter_time().is_set())
	{
// 		quantity_t tsd;
		if (sputter_rate().data.size()==1)
		{
			total_sputter_depths_p =  total_sputter_time()*sputter_rate();
			calc_history.push_back(clusters->begin()->second.measurement->filename_p->filename_with_path()+"\t"+"crater"+"\t" + "total_sputter_depths from sputter_rate skalar");
		}
//ATTENTION this has not been tested yet!!!
		else if (sputter_depth().is_set()) 
		{
			total_sputter_depths_p = sputter_depth();
			total_sputter_depths_p.data.resize(1);
			total_sputter_depths_p.data[0]=sputter_depth().data.back();
			// adding skipped points at the end and beginning of sputter_time/depth
			if (sputter_time().is_set() && sputter_time().data.back()<total_sputter_time().data.back())
			{
				total_sputter_depths_p = total_sputter_depths_p + (total_sputter_time().data.back()-(sputter_time().data.back()))*sputter_rate().data.back();
				total_sputter_depths_p = total_sputter_depths_p + (sputter_time().data.front()*sputter_rate().data.front());
			}
// 				total_sputter_time() - sputter_time();
			calc_history.push_back(clusters->begin()->second.measurement->filename_p->filename_with_path()+"\t"+"crater"+"\t" + "total_sputter_depths from sputter_rate vector");
		}
		total_sputter_depths_p.name = "total_sputter_depth";
		return total_sputter_depths_p;
	}
	return quantity_t();
}

quantity_t crater_t::total_sputter_depths_from_linescans()
{
	quantity_t total_sputter_depths_temp;
	if (linescans.empty()) return total_sputter_depths_temp;
	total_sputter_depths_temp=linescans[0].depth();
	for (int i=1;i<linescans.size();i++)
	{
		total_sputter_depths_temp.add_to_data(linescans[i].depth()); // autmatically
	}
	return total_sputter_depths_temp;
}

quantity_t crater_t::total_sputter_depths_from_filename()
{
// 	cout << "filename->total_sputter_depths.to_screen()" << endl;
// 	filename->total_sputter_depths.to_screen();
	return filename->total_sputter_depths;
}

quantity_t crater_t::global_sputter_time()
{
	return global_sputter_time_p;
}

quantity_t crater_t::sputter_current()
{
	return sputter_current_p;
}

quantity_t crater_t::sputter_depth(double resolution)
{
	if (already_calculated_sputter_depth==false && !sputter_depth_p.is_set())
	{
		already_calculated_sputter_depth=true;
		for (map<string,cluster_t>::iterator it=clusters->begin();it!=clusters->end();++it)
		{
			if (it->second.sputter_depth().is_set()) break;
		}
	}
	/*change base data resolution*/
	if (resolution!=sputter_depth_resolution && resolution > 0) 
	{
		sputter_depth_resolution = resolution;
		if (sputter_depth_p.is_set())
		{
			double start = sputter_depth_p.data[0];
			double ende = sputter_depth_p.data.back();
			int size = (ende-start)/resolution+1;
			vector<double> sputter_depth_new(size);
			for (int i=0;i<size;i++)
				sputter_depth_new[i]=i*sputter_depth_resolution+start;
			sputter_depth_old = sputter_depth_p.data;
			sputter_depth_p.data = sputter_depth_new;
			map<double,double> data_XY;
			if (sputter_time_p.is_set())
			{
				data_XY.clear();
				tools::vec::combine_vecs_to_map(sputter_depth_old,sputter_time_p.data,&data_XY);
				sputter_time_p.data = statistics::interpolate_data_XY(&data_XY,&sputter_depth_p.data);
			}
			if (sputter_current_p.is_set())
			{
				data_XY.clear();
				tools::vec::combine_vecs_to_map(sputter_depth_old,sputter_current_p.data,&data_XY);
				sputter_current_p.data = statistics::interpolate_data_XY(&data_XY,&sputter_depth_p.data);
			}
			for (map<string,cluster_t>::iterator it=clusters->begin();it!=clusters->end();++it)
			{
				if (it->second.concentration_p.is_set())
				{
					data_XY.clear();
					tools::vec::combine_vecs_to_map(sputter_depth_old,it->second.concentration_p.data,&data_XY);
					it->second.concentration_p.data = statistics::interpolate_data_XY(&data_XY,&sputter_depth_p.data);
				}
				if (it->second.intensity_p.is_set())
				{
					data_XY.clear();
					tools::vec::combine_vecs_to_map(sputter_depth_old,it->second.intensity_p.data,&data_XY);
					it->second.intensity_p.data = statistics::interpolate_data_XY(&data_XY,&sputter_depth_p.data);
				}
				
			}
		}
	}
	return sputter_depth_p;
}

quantity_t crater_t::sputter_energy()
{
	return sputter_energy_p;
}

quantity_t crater_t::sputter_rate()
{
	if (sputter_rate_p.is_set()) return sputter_rate_p;
	if (already_calculated_sputter_rate) return sputter_rate_p;
	already_calculated_sputter_rate=true;
	for (map<string,cluster_t>::iterator it=clusters->begin();it!=clusters->end();++it)
	{
		/*will automatically populate sputter_rate_p on success*/
		if (it->second.sputter_rate().is_set()) return sputter_rate_p;
	}
// 	/*look in other measurements/samples within this measurement_group with exactly the same matrix*/
	if (clusters->size()==0) return quantity_t();
// 	for (auto& M:clusters->begin()->second.measurement->measurement_group->measurements)
// 	{
// 		if (M == clusters->begin()->second.measurement) continue;
// 		M->load_from_database();
// 		if (M->sample_p->matrix == clusters->begin()->second.measurement->sample_p->matrix && M->crater.sputter_rate().is_set())
// 		{
// 			if (!sputter_rate_p.is_set()) sputter_rate_p = M->crater.sputter_rate();
// 			else sputter_rate_p = sputter_rate_p.add_to_data(M->crater.sputter_rate());
// 		}
// 	}
	
	/*if the sample matrix is known, check if there is another measurement in this group with a known SR for this matrix*/
	if (clusters->begin()->second.measurement->measurement_group->SRs(clusters->begin()->second.measurement->sample_p->matrix).is_set())
	{
		calc_history.push_back(clusters->begin()->second.measurement->filename_p->filename_with_path()+"\t"+"crater"+"\t" + "measurement->measurement_group->SRs(measurement->sample_p->matrix).is_set()");
		clusters->begin()->second.measurement->crater.sputter_rate_p = clusters->begin()->second.measurement->measurement_group->SRs(clusters->begin()->second.measurement->sample_p->matrix).mean();
	}
	
	/*just single element matrices in MG? --> SRs should be the same*/
	else if (clusters->begin()->second.measurement->measurement_group->reference_matrix_isotopes().size()==1)
	{
		quantity_t SR;
		for (auto& M:clusters->begin()->second.measurement->measurement_group->measurements)
		{
			if (M==clusters->begin()->second.measurement) continue;
// 			cout << "IN3\n";
			if (M->crater.sputter_rate().is_set())
			{
// 				cout << "IN4\n";
				if (SR.is_set()) SR = SR.add_to_data(M->crater.sputter_rate());
				else SR = M->crater.sputter_rate();
			}
		}
		calc_history.push_back(clusters->begin()->second.measurement->filename_p->filename_with_path()+"\t"+"crater"+"\t" + "sputter_rate: clusters->begin()->second.measurement->measurement_group->reference_matrix_isotopes().size()==1");
		clusters->begin()->second.measurement->crater.sputter_rate_p = SR.mean();
	}
	return sputter_rate_p;
}

quantity_t crater_t::sputter_time()
{
	if (already_calculated_sputter_time==false && !sputter_time_p.is_set())
	{
		already_calculated_sputter_time = true;
		for (map<string,cluster_t>::iterator it=clusters->begin();it!=clusters->end();++it)
		{
			if (it->second.sputter_time().is_set()) break;
		}
	}
	return sputter_time_p;
}

quantity_t crater_t::total_sputter_time()
{
	return total_sputter_time_p;
}





/*LINESCAN_T*/

fit_functions::asym2sig_t linescan_t::fit_params()
{
	return asym2sig;
}

quantity_t linescan_t::fit()
{
	quantity_t fitted_z_profile=z;
	fitted_z_profile.name = "linescan_z_fitted";
	fitted_z_profile.data={};
	if (z.data.size()<21) return fitted_z_profile;
	map<double,double> data_XY;
	// sampling rate: 3000 points for 600um scan length 10%=300 points window size is a good value
	tools::vec::combine_vecs_to_map(&xy.data,z.filter_impulse(0,2).data,&data_XY);
	if (!fitted) 
	{
		double chisq_rel = 1;
		for (int j=1;j<4;j++)
		{
			fit_functions::asym2sig_t asym2sig_p;
			asym2sig_p.fit(data_XY,NAN,NAN,NAN,NAN,j/4 * (data_XY.end()->first));
			if (asym2sig_p.chisq()<asym2sig.chisq() || asym2sig.chisq()==-1) asym2sig=asym2sig_p;
#ifdef DEBUG
			cout << endl;
			cout << "j=" << j <<  "\tasym2sig.chisq()\t" << asym2sig.chisq() << endl;
			cout << "j=" << j <<  "\tasym2sig.chisq0()\t" << asym2sig.chisq0() << endl;
#endif
// 			cout << "asym2sig.chisq()/asym2sig.chisq0()\t" << asym2sig.chisq()/asym2sig.chisq0() << endl;
			
			if (asym2sig.fitted() && asym2sig.chisq()/asym2sig.chisq0()<0.01 && asym2sig.chisq()/asym2sig.chisq0()>0) break;
		}
		fitted=asym2sig.fitted();
	}
	if (!fitted) return fitted_z_profile;
	fitted_z_profile.data=asym2sig.fitted_y_data(xy.data);
// 	plot_t::fast_plot({xy,xy},{z,fitted_z_profile},"/tmp/linescan"+to_string((int)z.median().data[0]),false);
	return fitted_z_profile;
}

quantity_t linescan_t::depth()
{
// 	cout << "quantity_t linescan_t::depth()" << endl;
	quantity_t linescan_depth;
	
	quantity_t fitted_z_profile = fit();
	linescan_depth.name="total_sputter_depth";
	linescan_depth.unit=fitted_z_profile.unit;
	linescan_depth.dimension=fitted_z_profile.dimension;


	double y0=asym2sig.fitted_y_data({0})[0];
	double m=(asym2sig.fitted_y_data({xy.data.back()})[0]-y0)/xy.data.back();
	
// 	cout << "asym2sig.xc = " << asym2sig.xc << endl;
// 	cout << "m = " << asym2sig.m << "\t" << m << endl;
// 	cout << "y0 = " << asym2sig.y0 << "\t" << y0 << endl;
// 	cout << "y0+m*asym2sig.xc = " << y0+m*asym2sig.xc<< endl;
	
	
	// fitted function value in the centre of the crater minus the valu at the same position on the (virtual) surface
	linescan_depth.data={abs(asym2sig.fitted_y_data({asym2sig.xc})[0] - (y0+m*asym2sig.xc))};
	
// 	linescan_depth.data={abs(asym2sig.fitted_y_data({asym2sig.xc})[0]-asym2sig.y0-asym2sig.m*asym2sig.xc)};
// 	linescan_depth.data={abs(asym2sig.fitted_y_data({asym2sig.xc})[0]-asym2sig.fitted_y_data({0})[0]-asym2sig.y0-asym2sig.m*asym2sig.xc)};
	
	return linescan_depth;
}

string linescan_t::to_str(string prefix)
{
	stringstream ss;
	ss << prefix;
	ss << std::scientific << depth().to_str() <<"; " << fit_params().to_str();
	return ss.str();
}

