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
#include "quantity.hpp"


void quantity_t::reduce_string(string reduce_this)
{
	// (length)^(-3)/time*amount/(current/intensity)
// 	set<string> operators={"*","/"};
	string reduced = reduce_this;
	tools::str::remove_spaces(&reduced);
	string string_wo_brackets = tools::str::get_string_between_string_A_and_next_B(&reduced,"(",")");
	if (string_wo_brackets!="") reduce_string(reduced);
	if (reduced.find("/")!=string::npos)
	{
		string counter = reduced.substr(0,reduced.find("/"));
		string denominator = reduced.substr(reduced.find("/"));
	}
}


bool quantity_t::is_data()
{
	if (data.size()>0) return true;
	return false;
}

bool quantity_t::is_skalar_data()
{
	if (data.size()==1) return true;
	return false;
}


void quantity_t::to_screen(string prefix)
{
	cout << prefix << name << endl;
	cout << prefix << "\tdimension: " << dimension << endl;
	cout << prefix << "\tunit: " << unit << endl;
	if(data.size()!=1) cout << prefix << "\tdata.size(): " << data.size() << "\tmedian = " << median().data[0] << endl;
	else cout << prefix << "\tdata: " << data[0] << endl;
	return;
}

string quantity_t::to_str(string prefix)
{
// 	if (name=="") return "";
	stringstream ss;
	ss << prefix;
	ss << name;
	if (data.size() == 1) ss <<std::scientific  <<  " = "  << data[0];
	else if (data.size() == 0) ss << " = EMPTY";
	else 
	{
		ss <<std::scientific  << " = vector(" << data.size() <<"), median = " << median().data[0];
	}
	
	if (unit == "") ss << " [unkown]";
	else ss << " [" + unit + "]";
	
	return ss.str();
}


bool quantity_t::is_same_unit(quantity_t *quantity_p)
{
	if (unit!=quantity_p->unit) return false;
	return true;
}

bool quantity_t::are_same_dimensions(string dim1, string dim2)
{
	if (dim1==dim2) return true;
	/*
	 *	dim might be something like: 
	 * 	intensity
	 * 	concentration
	 * 	current
	 */
	return false;
}

bool quantity_t::is_same_dimension(quantity_t* quantity_p)
{
	return are_same_dimensions(dimension,quantity_p->dimension);
}

bool quantity_t::is_same_dimension(string dimension_p)
{
	return are_same_dimensions(dimension,dimension_p);
}

quantity_t quantity_t::fit_polynom_by_x_data(quantity_t x_data, quantity_t new_x_data, int polynom_grade)
{
	if (!x_data.is_set()) return quantity_t();
	if (!is_set()) return quantity_t();
	if (polynom_grade==-1) polynom_grade=17; // seems to be good for implant profiles
	quantity_t fit;
	fit_functions::polynom_t polynom;
	map<double,double> data_XY;
	tools::vec::combine_vecs_to_map(&x_data.data,&data,&data_XY);
	fit.dimension = dimension;
	fit.unit = unit;
	fit.name = "poly"+to_string(polynom_grade)+"_fit("+name+")";
	polynom.fit(data_XY,polynom_grade);
	
	if (new_x_data.is_set()) fit.data=polynom.fitted_y_data(new_x_data.data);
	else fit.data=polynom.fitted_y_data(x_data.data);
	return fit;
}

quantity_t quantity_t::polyfit(int polynom_grade)
{
	if (!is_set()) return quantity_t();
	if (polynom_grade==-1) polynom_grade=17; // seems to be good for implant profiles
	quantity_t fit;
	fit_functions::polynom_t polynom;
	map<double,double> data_XY;
	vector<double> X(data.size());
	for (int x=0;x<X.size();x++)
		X[x]=x;
	tools::vec::combine_vecs_to_map(&X,&data,&data_XY);
	fit.dimension = dimension;
	fit.unit = unit;
	fit.name = "poly"+to_string(polynom_grade)+"_fit("+name+")";
	if (!polynom.fit(data_XY,polynom_grade)) return {};
	fit.data=polynom.fitted_y_data(X);
	
	return fit;
}

quantity_t quantity_t::integrate_pbp(quantity_t x_data)
{
	if (data.size()!=x_data.data.size()) return {};
	quantity_t area;

	area.dimension = x_data.dimension + "*" + dimension;
	area.unit = x_data.unit + "*" + unit;
	area.name = "integral_pbp("+name+")d("+x_data.name+")";
	area.data.resize(data.size());

	double sum=0;
	double dY,dX;
	area.data[0]=0;
    for (int i=1;i<data.size();i++)
	{
        dY=abs(data[i]+data[i-1])/2;
        dX=abs(x_data.data[i]-x_data.data[i-1]);
		sum=sum+dX*dY;
        area.data[i] = sum; 
    }
	return area;
}

quantity_t quantity_t::log10()
{
	if (data.size()==0)
		return {};
	quantity_t out = *this;
	for (auto& o : out.data)
	{
		if (o>0)
			o = std::log10(o);
		else
			o = 0;
	}
	return out;
}

quantity_t quantity_t::integrate(quantity_t x_data, double lower_X_limit, double upper_X_limit)
{
	return integrate(&x_data,lower_X_limit,upper_X_limit);
}

quantity_t quantity_t::integrate(quantity_t* x_data, double lower_X_limit, double upper_X_limit)
{
	quantity_t area;
	map<double,double> data_XY;
	tools::vec::combine_vecs_to_map(&x_data->data,&data,&data_XY);
	area.dimension = x_data->dimension + "*" + dimension;
	area.unit = x_data->unit + "*" + unit;
	area.name = "integral("+name+")d("+x_data->name+")";
	area.data.push_back(statistics::integrate(&data_XY,lower_X_limit,upper_X_limit));
	return area;
}

quantity_t quantity_t::max_at_x(quantity_t X)
{
	quantity_t x_at_max_p;
	x_at_max_p.dimension = X.dimension;
	x_at_max_p.unit = X.unit;
	x_at_max_p.name = "("+X.name+")at_max("+name+")";
	x_at_max_p.data.resize(1);
// 	if (x_at_max_s.is_set() && x_at_max_s)
	map<double,double> XY_data;
	tools::vec::combine_vecs_to_map(&X.data,&data,&XY_data);
	vector<double> X_data,Y_data;
	tools::vec::split_map_to_vecs(&XY_data,&X_data,&Y_data);
	if (X_data.size()==0) return {};
	int vec_pos=statistics::get_max_index_from_Y(Y_data);
	x_at_max_p.data[0] = X_data[vec_pos];
	return x_at_max_p;
}

/// TESTEN!!!
quantity_t quantity_t::max_at_x(quantity_t X, double lower_X_limit, double upper_X_limit)
{
	for(int x=0;x<X.data.size();x++)
	{
		if (x<lower_X_limit || x>upper_X_limit) 
		{
			X.data.erase(X.data.begin()+x);
			x--;
		}
	}
	return max_at_x(X);
}

quantity_t quantity_t::min_at_x(quantity_t X, double lower_X_limit, double upper_X_limit)
{
	quantity_t x_at_min_p;
	x_at_min_p.dimension = X.dimension;
	x_at_min_p.unit = X.unit;
	x_at_min_p.name = "("+X.name+")at_max("+name+")";
	x_at_min_p.data.resize(1);
	map<double,double> XY_data;
	tools::vec::combine_vecs_to_map(&X.data,&data,&XY_data);
	for(map<double,double>::iterator it=XY_data.begin();it!=XY_data.end();++it)
	{
		if (it->first<lower_X_limit || it->first>upper_X_limit) 
		{
			XY_data.erase(it);
			--it;
		}
	}
	vector<double> X_data,Y_data;
	tools::vec::split_map_to_vecs(&XY_data,&X_data,&Y_data);
	int vec_pos=statistics::get_min_index_from_Y(Y_data);
	x_at_min_p.data[0] = X_data[vec_pos];
	return x_at_min_p;
}


quantity_t quantity_t::median()
{
	quantity_t median;
	median.unit=unit;
	median.dimension=dimension;
	median.name="median("+name+")";
	median.data={statistics::get_median_from_Y(data)};
	return median;
}

quantity_t quantity_t::quantile(double percentile)
{
	if (percentile>1 || percentile<0) percentile=0.75;
	quantity_t quantile;
	quantile.unit=unit;
	quantile.dimension=dimension;
	quantile.name="percentile"+to_string(percentile)+"("+name+")";
	quantile.data={statistics::get_quantile_from_Y(data,percentile)};
	return quantile;
}

quantity_t quantity_t::mean()
{
	quantity_t mean;
	mean.unit=unit;
	mean.dimension=dimension;
	mean.name="mean("+name+")";
	mean.data={statistics::get_mean_from_Y(data)};
	return mean;
}

quantity_t quantity_t::trimmed_mean(float alpha)
{
	quantity_t tr_mean;
	tr_mean.unit=unit;
	tr_mean.dimension=dimension;
	tr_mean.name="tr_mean("+name+")";
	tr_mean.data={statistics::get_trimmed_mean_from_Y(data,alpha)};
	return tr_mean;
}

quantity_t quantity_t::trimmed_mean_reduced()
{
	return remove_data_by_index(0,data.size()/2).trimmed_mean();
}

quantity_t quantity_t::gastwirth()
{
	quantity_t gastw;
	gastw.unit=unit;
	gastw.dimension=dimension;
	gastw.name="gastwirth("+name+")";
	gastw.data={statistics::get_gastwirth_estimator_from_Y(data)};
	return gastw;
}



quantity_t quantity_t::mad()
{
	quantity_t mad;
	mad.unit=unit;
	mad.dimension=dimension;
	mad.name="mad("+name+")";
	mad.data={statistics::get_mad_from_Y(data)};
	return mad;
}

quantity_t quantity_t::moving_window_mad(int window_size)
{
	if (window_size==0) window_size = 0.05*data.size();
	if (window_size==0) return quantity_t();
	quantity_t mad;
	mad.unit=unit;
	mad.dimension=dimension;
	mad.name="moving_window"+to_string(window_size)+"_mad("+name+")";
	mad.data={statistics::get_moving_window_MAD_from_Y(data,window_size)};
	return mad;
}

quantity_t quantity_t::moving_window_mean(int window_size)
{
	if (window_size==0) window_size = 0.05*data.size();
	if (window_size==0) return quantity_t();
	quantity_t mad;
	mad.unit=unit;
	mad.dimension=dimension;
	mad.name="moving_window"+to_string(window_size)+"_mean("+name+")";
	mad.data={statistics::get_moving_window_mean_from_Y(data,window_size)};
	return mad;
}

quantity_t quantity_t::moving_window_median(int window_size)
{
	if (window_size==0) window_size = 0.05*data.size();
	if (window_size==0) return quantity_t();
	quantity_t mad;
	mad.unit=unit;
	mad.dimension=dimension;
	mad.name="moving_window"+to_string(window_size)+"_median("+name+")";
	mad.data={statistics::get_moving_window_median_from_Y(data,window_size)};
	return mad;
}

quantity_t quantity_t::moving_window_sd(int window_size)
{
	if (window_size==0) window_size = 0.05*data.size();
	if (window_size==0) return quantity_t();
	quantity_t mad;
	mad.unit=unit;
	mad.dimension=dimension;
	mad.name="moving_window"+to_string(window_size)+"_sd("+name+")";
	mad.data={statistics::get_moving_window_sd_from_Y(data,window_size)};
	return mad;
}

quantity_t quantity_t::sd()
{
	quantity_t sd;
	sd.unit=unit;
	sd.dimension=dimension;
	sd.name="sd("+name+")";
	sd.data={statistics::get_sd_from_Y(data)};
	return sd;
}

quantity_t quantity_t::max()
{
	if (!is_set()) return {};
	quantity_t max;
	max.unit=unit;
	max.dimension=dimension;
	max.name="max("+name+")";
// 	if (max_s>-1)
// 	{
// 		max.data.push_back(max_s);
// 		return max;
// 	}
	int max_index = statistics::get_max_index_from_Y(data);
// 	cout << endl << "max_index=" << max_index << endl;
	if (max_index>=0) max.data={data[max_index]};
	else max.data={};
// 	if (max_index>data.size()-2) 
// 	{
// 		cout << endl <<  "quantity_t::max()\tindex>data.size()-2" << endl;
// 		return {}; 
// 	}
// 	if (max_index<2) 
// 	{
// 		cout << endl << "quantity_t::max()\tmax_index<2" << endl;
// 		return {}; 
// 	}
	return max;
}

quantity_t quantity_t::min()
{
	quantity_t min;
	min.unit=unit;
	min.dimension=dimension;
	min.name="min("+name+")";
	int min_index = statistics::get_min_index_from_Y(data);
	if (min_index>=0) min.data={data[min_index]};
	else min.data={};
	if (min_index>data.size()-2) return {}; 
	if (min_index<2) return {}; 
	return min;
}

bool quantity_t::is_set()
{
	if (data.size()==0) return false;
// 	if (dimension.length()==0) return false;
	if (unit.length()==0) return false;
	return true;
}

quantity_t quantity_t::filter_impulse(int window_size, float factor)
{
	quantity_t filtered;
	if (data.size()<window_size)  return filtered;
	filtered.name = "impulse_filtered("+name+")";
	filtered.dimension=dimension;
	filtered.unit = unit;
	if (window_size==0) window_size=0.05*data.size();
	if (window_size==0) window_size=3;
// 	if (factor<0) factor= statistics::get_mad_from_Y(data)*1;
	filtered.data = statistics::impulse_filter(data,window_size,factor);
	return filtered;
}

quantity_t quantity_t::filter_gaussian(int window_size, double alpha)
{
	quantity_t filtered;
	if (data.size()<window_size)  return filtered;
	filtered.name = "gaussian_filtered("+name+")";
	filtered.dimension=dimension;
	filtered.unit = unit;
	if (window_size==0) window_size=0.05*data.size();
	filtered.data = statistics::impulse_filter(data,window_size,alpha);
	return filtered;
}


quantity_t quantity_t::filter_recursive_median(int window_size)
{
	quantity_t filtered;
	if (data.size()<window_size)  return filtered;
	filtered.name = "recursiveMedian_filtered("+name+")";
	filtered.dimension=dimension;
	filtered.unit = unit;
	if (window_size==0) window_size=0.05*data.size();
	filtered.data = statistics::recursive_median_filter(data,window_size);
	return filtered;
}


quantity_t quantity_t::add_to_data(quantity_t quantity_p)
{
	if (!is_same_dimension(&quantity_p)) return quantity_t();
	if (!is_same_unit(&quantity_p)) return quantity_t();
	quantity_t result = *this;
	tools::vec::add(&result.data,quantity_p.data);
	return result;
}

quantity_t quantity_t::remove_data_by_index(unsigned int start, unsigned int stop)
{
	if (!is_set())
		return {};
	if (start<0)
		start=0;
	if (start>=data.size())
	{
// 		logger::error("quantity_t::remove_data_by_index()","start>=data.size()","","return empty");
		return {};
	}
	if (stop>data.size())
	{
		stop=data.size();
	}
	quantity_t copy = *this;

	copy.data.erase(copy.data.begin()+start,copy.data.begin()+stop);
	return copy;
}

quantity_t quantity_t::remove_from_data(quantity_t& quantity)
{
	if (!is_same_dimension(&quantity)) return quantity_t();
	if (!is_same_unit(&quantity)) return quantity_t();
	quantity_t result = *this;
	int start;
	bool found;
	for (int i=0;i<data.size();i++)
	{
		found = false;
		if (data[i]==quantity.data[0])
		{
			start=i;
			for (int j=1;j<quantity.data.size() && i+j<data.size();j++)
			{
				if (data[i+j]!=quantity.data[j]) 
					found = true;
				else 
				{
					found = false;
					break;
				}
			}
		}
		if (found) 
		{
			result.data.erase(result.data.begin()+start,result.data.begin()+start+quantity.data.size()-1);
			break;
		}
	}
	return result;
}






/*OPERATORS*/

bool quantity_t::operator<(const quantity_t& quantity) const
{
	if (name > quantity.name) return false;
	if (dimension!=quantity.dimension) return false;
	if (unit != quantity.unit) return false;
	if (data.size()>quantity.data.size()) return false;
	if (data.size()<quantity.data.size()) return true;
	
	double sum1=0;
	double sum2=0;
	for (int i=0;i<data.size();i++)
	{
		sum1+=data[i];
		sum2+=quantity.data[i];
	}
	if (sum1>sum2) return false;
	return true;
}

bool quantity_t::operator==(const quantity_t& quantity)
{
	if (dimension != quantity.dimension) return false;
	if (unit != quantity.unit) return false;
	if (data.size() != quantity.data.size()) return false;
	for (int i=0;i<data.size();i++)
		if (data[i]!=quantity.data[i]) return false;
	return true;
}


bool quantity_t::operator!=(const quantity_t& quantity)
{
	return !operator==(quantity);
}

quantity_t quantity_t::operator+(const quantity_t& quantity)
{
	quantity_t sum;
	if (!is_set()) return sum;
	if (dimension != quantity.dimension) return sum;
	if (unit != quantity.unit) return sum;
	if (data.size()==0 || quantity.data.size()==0 ) return sum;
	sum.dimension=dimension;
	sum.unit = unit;
	sum.name = name;
	
	if (data.size() == quantity.data.size())
	{
		sum.data.resize(data.size());
		for (int i=0; i< data.size()&&i<quantity.data.size();i++)
		{
			sum.data[i] = data[i] + quantity.data[i];
		}
	} 
	else if (data.size()==1)
	{
		sum.data.resize(quantity.data.size());
		for (int i=0; i< sum.data.size();i++)
		{
			sum.data[i] = data[0] + quantity.data[i];
		}
	}
	else if (quantity.data.size()==1)
	{
		sum.data.resize(data.size());
		for (int i=0; i< sum.data.size();i++)
		{
			sum.data[i] = quantity.data[0] + data[i];
		}
	}
	return sum;
}

quantity_t quantity_t::operator+(const double summand)
{
	quantity_t sum;
	if (!is_set()) return sum;
	sum.dimension=dimension;
	sum.unit = unit;
	sum.name = name;
	sum.data.resize(data.size());
	for (int i=0; i< data.size()&&i<data.size();i++)
	{
		sum.data[i] = data[i] + summand;
	}
	return sum;
}

quantity_t quantity_t::operator-(const quantity_t& quantity)
{
	quantity_t difference;
	if (!is_set()) return difference;
	if (dimension != quantity.dimension) return difference;
	if (unit != quantity.unit) return difference;
	if (data.size()==0 || quantity.data.size()==0 ) return difference;
	difference.dimension=dimension;
	difference.unit = unit;
	difference.name = name;
	if (data.size() == quantity.data.size())
	{
		difference.data.resize(data.size());
		for (int i=0; i< data.size()&&i<quantity.data.size();i++)
		{
			difference.data[i] = data[i] - quantity.data[i];
		}
	} 
	else if (data.size()==1)
	{
		difference.data.resize(quantity.data.size());
		for (int i=0; i< difference.data.size();i++)
		{
			difference.data[i] = data[0] - quantity.data[i];
		}
	}
	else if (quantity.data.size()==1)
	{
		difference.data.resize(data.size());
		for (int i=0; i< difference.data.size();i++)
		{
			difference.data[i] = quantity.data[0] - data[i];
		}
	}
	return difference;
}


quantity_t quantity_t::operator * (const quantity_t& quantity_p)
{
	quantity_t produkt;
	
	produkt.name=name + "*" + quantity_p.name;
	produkt.dimension=dimension + "*" + quantity_p.dimension;
	produkt.unit=unit + "*" + quantity_p.unit;
	
	if (data.size()==1)
	{
		produkt.data=quantity_p.data;
		for (auto& d:produkt.data)
			d=d*data[0];
	}
	else if (quantity_p.data.size()==1)
	{
		produkt.data=data;
		for (auto& d:produkt.data)
			d=d*quantity_p.data[0];
	}
	else if (quantity_p.data.size()==data.size())
	{
		produkt.data.resize(data.size());
		for (int i=0;i<data.size();i++)
			produkt.data[i]=data[i]*quantity_p.data[i];
	}
	else return quantity_t();
	return produkt;
}

quantity_t quantity_t::operator / (const quantity_t& quantity_p)
{
	quantity_t quotient;
	quotient.name=name + "/ (" + quantity_p.name +")";
	quotient.dimension=dimension + "/ (" + quantity_p.dimension +")";
	quotient.unit=unit + "/ (" + quantity_p.unit +")";
	
	if (quantity_p.data.size()==1)
	{
		quotient.data.resize(data.size());
		for (int i=0;i<data.size();i++)
		{
// 			cout << "A: " << data[i] << "/" << quantity_p.data[0] << endl;
			quotient.data[i]=data[i]/quantity_p.data[0];
		}
	}
	else if (quantity_p.data.size()==data.size())
	{
		quotient.data.resize(data.size());
		for (int i=0;i<data.size();i++)
		{
// 			cout << "B: " << data[i] << "/" << quantity_p.data[i] << endl;
			quotient.data[i]=data[i]/quantity_p.data[i];
		}
	}
	else if (data.size()==1)
	{
		quotient.data.resize(quantity_p.data.size());
		for (int i=0;i<quotient.data.size();i++)
		{
// 			cout << "B: " << data[i] << "/" << quantity_p.data[i] << endl;
			quotient.data[i]=data[0]/quantity_p.data[i];
		}
	}
	else
	{
		cout << ">>>ERROR<<<: " <<  quotient.name << endl;
		cout << name << ".data.size()= " << data.size() << " divide by " << quantity_p.name << ".data.size()= " << quantity_p.data.size() << endl;
	}
// 	quotient.to_screen("QUOTIENT:\t");
	return quotient;
}

quantity_t quantity_t::operator / (const double divisor)
{
	quantity_t quotient;
	quotient.name=name;
	quotient.dimension=dimension;
	quotient.unit=unit;
	quotient.data.resize(data.size());
	for (int i=0;i<data.size();i++)
	{
		quotient.data[i]=data[i]/divisor;
	}

	return quotient;
}
quantity_t quantity_t::operator * (const double factor)
{
	quantity_t quotient;
	quotient.name=name;
	quotient.dimension=dimension;
	quotient.unit=unit;
	quotient.data.resize(data.size());
	for (int i=0;i<data.size();i++)
	{
		quotient.data[i]=data[i]*factor;
	}

	return quotient;
}





