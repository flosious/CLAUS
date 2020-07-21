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

#ifndef PRINT_HPP
#define PRINT_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <set>
// #include "id.hpp"
// #include "quantity.hpp"
// #include "isotope.hpp"
// #include "measurement.hpp"




using namespace std;

/// this class is essentially just for debugging...
class print {
private:
    
public:
//     static void measurement(measurement_t* measurement, string prefix="");
// 	static void id(identifier_t* id,string prefix="");
// 	static void quantity(quantity_t* quantity, string prefix="");
// 	static void cluster(cluster_t* cluster, string prefix="");
};

template <typename T>
void print(T ska) {
    cout << "\t"<< ska; cout << endl;
}

template <typename T>
void print(vector<T> ska) {
	for (int i=0; i<ska.size();i++) {
		cout << "[" << i << "]";
		print(ska[i]);
	}
}

template <typename T>
void print(set<T> ska) {
	int i=0;
	for (auto& s:ska)
	{
		cout << "[" << i << "]";
		print(s);
		i++;
	}
}

template <typename T>
void print(vector< vector<T> > mat, string delimiter="\t") {
	cout << endl;
	for (int i=0;i<mat.size();i++) {
		for (int j=0; j<(mat.at(i)).size();j++) {
			cout << "[" << i << "]";
			cout << "[" << j << "]";
			cout << "\t";
			print((mat)[i][j]);
		}
		cout << endl;
	}
}

template <typename T>
void print_not_empty(vector< vector<T> > mat, string delimiter="\t") {
	cout << endl;
	for (int i=0;i<mat.size();i++) {
		for (int j=0; j<(mat.at(i)).size();j++) 
		{
			if(mat.at(i)[j]!="") 
			{
				cout << "[" << i << "]";
				cout << "[" << j << "]";
				cout << "\t";
				print((mat)[i][j]);
			}
		}
		cout << endl;
	}
}

template <typename T>
void print(vector< vector< vector<T> > > ten) {
	for (int z=0;z<ten.size();z++) {
		for (int i=0;i<(ten.at(z)).size();i++) {
			for (int j=0; j<(ten.at(z)[i]).size();j++) {
//                 if ((ten)[z][i][j].size()>0) {
				cout << "[" << z << "]";
				cout << "[" << i << "]";
				cout << "[" << j << "]";
				cout << "\t";
				print((ten)[z][i][j]);
//                 }
			}
		}
	}
}

template <typename T>
void print(map<T, T>  mat) {
	for (auto& it : mat) {
		cout << it.first << "\t" << it.second << endl;
//         print(it.second);
    }
}

// template <typename T>
// void print(map<T, T>  mat) {
// 	for (auto& it : mat) {
// 		cout << it.first << ":\n";
//         print(it.second);
//     }
// }


// void print(map<string, quantity_t>  mat) {
// 	for (auto& it : mat) {
//         if (it.second.data.size()==1) cout << it.first << " = " << it.second.data[0] << "[" << it.second.unit << "]";
// 		
//         print(it.second);
//     }
// }

// void print(map<string, vector<string>>  mat) {
// 	for (map<string, vector<string>>::iterator it=mat.begin(); it!=mat.end(); ++it) {
//         cout << it->first << ":\n";
//         print(it->second);
//     }
// }

#endif // TOOLS_HPP
