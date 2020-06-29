#include "tools.hpp"

vector<double> tools::math::multiply(vector<double>* values, double factor)
{
	vector<double> results(values->size());
	for (int i=0;i<values->size();i++)
	{
		results[i]=values->at(i)*factor;
	}
	return results;
}

vector<double> tools::math::add(vector<double>* values, double summand)
{
	vector<double> results(values->size());
	for (int i=0;i<values->size();i++)
	{
		results[i]=summand+values->at(i);
	}
	return results;
}
