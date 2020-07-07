/*
	Copyright (C) 2020 Florian Bärwolf

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
#include "tools.hpp"

// template <typename T> 
// static vector<T> add(vector<T> *start, vector<T> *ende)
// {
// 	vector<T> result={*start,*ende};
// // 	return start->insert(start->end(),ende->begin(),ende->end());
// 	return result;
// }

// template <typename T> 
// void tools::vec::add(vector<T> *result_vec,vector<T> adder)
// {
// 	result_vec->insert(result_vec->end(),adder.begin(),adder.end());
// }

string tools::vec::combine_vec_to_string(vector<string>& vec, string delimiter)
{
	string result="";
	for (int i=0;i<vec.size();i++)
		result+=delimiter+vec[i];
// 	if (delimiter!="" && result.length()>delimiter.length()) result.erase(result.begin()+result.size()-delimiter.size()-1,result.begin()+result.size()-1);
	if (delimiter!="" && result.length()>delimiter.length()) result.erase(0,delimiter.length());
	return result;
}

void tools::vec::add(vector<string> *result_vec,vector<string> adder)
{
	result_vec->insert(result_vec->end(),adder.begin(),adder.end());
}

void tools::vec::add(vector<double> *result_vec,vector<double> adder)
{
	result_vec->insert(result_vec->end(),adder.begin(),adder.end());
}

void tools::vec::remove(vector<double> *result_vec,vector<double> subtractors)
{
	for (auto& subtractor:subtractors)
	{
		for (int i=result_vec->size()-1;i>=0;i--)
		{
			if (result_vec->at(i)==subtractor) result_vec->erase(result_vec->begin()+i);
		}
	}
}

void tools::vec::split_map_to_vecs(map<double,double>* XY_mat, vector<double> *X, vector<double>* Y)
{
	int i;
	map<double,double>::iterator it;
	if (X!=nullptr)
	{
		X->clear();
		X->resize(XY_mat->size());
		i=0;
		for (it=XY_mat->begin();it!=XY_mat->end();it++)
		{
			X->at(i)=it->first;
			i++;
		}
	}
	if (Y!=nullptr)
	{
		Y->clear();
		Y->resize(XY_mat->size());
		i=0;
		for (it=XY_mat->begin();it!=XY_mat->end();it++)
		{
			Y->at(i)=it->second;
			i++;
		}
	}
	
	/*
	for (it=XY_mat->begin();it!=XY_mat->end();it++)
	{
		X->at(i)=it->first;
		Y->at(i)=it->second;
		i++;
	}
	*/
	return;
}


void tools::vec::combine_vecs_to_map(vector<double> *X, vector<double>* Y, map<double,double>* XY_mat)
{
	int size=X->size();
	if (Y->size()<size) size = Y->size();
	
	XY_mat->clear();
	double x_shift = 1E-7;
	for (int i=0;i<size;i++)
	{
		/**************************************************
		 * inserting same X values, will overwrite them  **
		 * ... so this a fast hack:     X->at(i)*1E-5    **
		 *************************************************/
// 		if (XY_mat->find(X->at(i))!=XY_mat->end()) XY_mat->insert(std::pair<double, double>(X->at(i)+X->at(i)*1E-5,Y->at(i)));
		if (XY_mat->find(X->at(i))!=XY_mat->end()) XY_mat->insert(std::pair<double, double>(X->at(i)+x_shift,Y->at(i)));
		else XY_mat->insert(std::pair<double, double>(X->at(i),Y->at(i)));
	}	
	return;
}

void tools::vec::combine_vecs_to_map(vector<double> X, vector<double>* Y, map<double,double>* XY_mat)
{
	combine_vecs_to_map(&X,Y,XY_mat);
}
void tools::vec::combine_vecs_to_map(vector<double> *X, vector<double> Y, map<double,double>* XY_mat)
{
	combine_vecs_to_map(X,&Y,XY_mat);
}
void tools::vec::combine_vecs_to_map(vector<double> X, vector<double> Y, map<double,double>* XY_mat)
{
	combine_vecs_to_map(&X,&Y,XY_mat);
}
