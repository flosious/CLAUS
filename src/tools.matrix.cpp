
#include "tools.hpp"

vector<vector<double>> tools::mat::cut_mat_double_rectengular(vector<vector<double>> mat_transposed) {
    vector<vector<double>> mat;
    int col_min;
    if (mat_transposed.size()>0) col_min=mat_transposed[0].size();
    else return mat;
    for (int i=1;i<mat_transposed.size();i++) {
        if (mat_transposed[i].size()<col_min) col_min = mat_transposed[i].size();
    }
    for (int i=0;i<mat_transposed.size();i++) {
        vector<double> vec = mat_transposed[i];
        vec.resize(col_min);
        mat.push_back(vec);
    }
    return mat;
}

vector<vector<string>> tools::mat::cut_mat_rectengular(vector<vector<string>> mat_transposed) {
    vector<vector<string>> mat;
    int col_min;
    if (mat_transposed.size()>0) col_min=mat_transposed[0].size();
    else return mat;
    for (int i=1;i<mat_transposed.size();i++) {
        if (mat_transposed[i].size()<col_min) col_min = mat_transposed[i].size();
    }
    for (int i=0;i<mat_transposed.size();i++) {
        vector<string> vec = mat_transposed[i];
        vec.resize(col_min);
        mat.push_back(vec);
    }
    return mat;
}

/*transforms matrix_XYY to tensor_{XY} */
vector<vector<vector<double>>> tools::mat::matrix_XYY_to_tensor(vector<vector<double>> matrix_XYY) {
    vector<vector<vector<double>>> tensor;
    vector<vector<double>> matrix_transposed = tools::mat::transpose_matrix_double(matrix_XYY);
    for (int i=1;i<matrix_transposed.size();i++) {
        tensor.push_back(tools::mat::transpose_matrix_double({matrix_transposed[0],matrix_transposed[i]}));
    }
    return tensor;
}


/*transforms matrix_XYXY to tensor_{XY} */
vector<vector<vector<double>>> tools::mat::matrix_XYXY_to_tensor(vector<vector<double>> matrix_XYXY) {
    vector<vector<vector<double>>> tensor;
    vector<vector<double>> matrix_transposed = tools::mat::transpose_matrix_double(matrix_XYXY);
    for (int i=0;i<matrix_transposed.size();i=i+2) {
        tensor.push_back(tools::mat::transpose_matrix_double({matrix_transposed[i],matrix_transposed[i+1]}));
    }
    return tensor;
}

vector<vector<double> > tools::mat::transpose_matrix_double(vector<vector<double> > matrix) {
	vector<vector<double> > matrix_T((matrix.at(0)).size(),vector<double>(matrix.size()));
	
	for (int i=0;i<matrix.size();i++) {
		for (int j=0;j<(matrix.at(i)).size();j++) {
			matrix_T[j][i]=(matrix)[i][j];
		}
	}
	return matrix_T;
}
vector<vector<double> > tools::mat::transpose_matrix(vector<vector<double> > matrix) {
	vector<vector<double> > matrix_T((matrix.at(0)).size(),vector<double>(matrix.size()));
	
	for (int i=0;i<matrix.size();i++) {
		for (int j=0;j<(matrix.at(i)).size();j++) {
			matrix_T[j][i]=(matrix)[i][j];
		}
	}
	return matrix_T;
}

vector<vector<string>> tools::mat::double_matrix_to_str_matrix(vector<vector<double>> mat_double) {
    vector<vector<string>> mat_string;
    vector<string> vec_string;
    for (int i=0;i<mat_double.size();i++) {        
        vec_string.clear();
        for (int j=0;j<mat_double.at(i).size();j++) {
            vec_string.push_back(to_string(mat_double.at(i)[j]));
        }
        mat_string.push_back(vec_string);
    }
    return mat_string;
}

vector<vector<double>> tools::mat::str_matrix_to_double_matrix(vector<vector<string>> mat_string) {
    vector<double> vec_ld;
    vector<vector<double>> mat_ld;
    for (int i=0;i<mat_string.size();i++) {        
        vec_ld.clear();
        for (int j=0;j<mat_string.at(i).size();j++) {
            if (mat_string.at(i)[j]!="")
                vec_ld.push_back(tools::str::str_to_double(mat_string.at(i)[j]));
        }
        mat_ld.push_back(vec_ld);
    }
    mat_ld=tools::mat::cut_mat_double_rectengular(mat_ld);
    return mat_ld;
}

vector<double> tools::mat::str_vec_to_double_vec(vector<string> vec_string) {
    vector<double> vec_ld;
	vec_ld.clear();
    for (int j=0;j<vec_string.size();j++) {
		if (vec_string[j]!="")
			vec_ld.push_back(tools::str::str_to_double(vec_string[j]));
	}
    return vec_ld;
}

vector<string> tools::mat::double_vec_to_str_vec(vector<double> vec_double) {
    vector<string> vec_ld(vec_double.size());
    for (int j=0;j<vec_double.size();j++) 
	{
// 		vec_ld[j]=(to_string(vec_double[j]));
		stringstream ss;
		ss << std::scientific <<vec_double[j];
		vec_ld[j] = ss.str();
	}
    return vec_ld;
}

string tools::mat::create_overview_string_from_matrix(vector< vector<string> > *overview) {
	string overview_string="";
	for (int i=0;i<overview->size();i++) {
		if ((overview->at(i)).size()==2) {
			overview_string.append((overview->at(i))[0]);
			overview_string.append("\t->\t");
			overview_string.append((overview->at(i))[1]);
			overview_string.append("\n");
		}
	}
	return overview_string;
}


vector<vector<string> > tools::mat::transform_tensor_unifying_columns_to_matrix(vector<vector<vector<string> > > *tensor) {
	vector<vector<string> > matrix, matrix_temp;
	matrix.clear();
	if (tensor->size()==0) return matrix;
	for (int i=0;i<tensor->size();i++) {
		  if ((tensor->at(i).size())>0) { // matrix
		  matrix_temp=tools::mat::transpose_matrix((tensor->at(i)));
		  matrix.insert(matrix.end(),matrix_temp.begin(),matrix_temp.end());
		  }
	}
	return tools::mat::transpose_matrix(matrix);
}

vector<vector<double> > tools::mat::transform_tensor_unifying_columns_to_matrix(vector<vector<vector<double> > > *tensor) {
	vector<vector<double> > matrix, matrix_temp;
	matrix.clear();
	if (tensor->size()==0) return matrix;
	for (int i=0;i<tensor->size();i++) {
		  if ((tensor->at(i).size())>0) { // matrix
		  matrix_temp=tools::mat::transpose_matrix((tensor->at(i)));
		  matrix.insert(matrix.end(),matrix_temp.begin(),matrix_temp.end());
		  }
	}
	return tools::mat::transpose_matrix(matrix);
}

vector<vector<string> > tools::mat::unify_2_vectors_to_matrix(vector<string > vector1, vector<string > vector2) {
	vector<vector<string> > matrix;
	matrix.clear();
	int max_size;
	if (vector1.size()<vector2.size()) max_size=vector2.size();
	else max_size=vector1.size();
	vector1.resize(max_size,"");
	vector2.resize(max_size,"");
	for (int i=0;i<max_size;i++) {
		  matrix.push_back({vector1[i],vector2[i]});
	}
	return matrix;
}

vector<vector<string> > tools::mat::add_vector_to_matrix(vector<vector<string > > matrix_summand, vector<string > vec) {
	
	vector<vector<string> > matrix_T=tools::mat::transpose_matrix(matrix_summand);
	matrix_T.push_back(vec);
	return tools::mat::transpose_matrix(matrix_T);
}

vector<vector<string> > tools::mat::transform_tensor_unifying_lines_to_matrix(vector<vector<vector<string> > > *tensor) {
	vector<vector<string> > result_matrix;
	for (int i=0;i<tensor->size();i++) {
		result_matrix.insert(result_matrix.end(),(tensor->at(i)).begin(),(tensor->at(i)).end());
	}
	return result_matrix;
}

void tools::mat::remove_lines_or_columns_from_matrix(vector<vector<string> > *matrix,vector<int> *lines, vector<int> *columns) {
	
	if (lines==nullptr && columns==nullptr) return;
	if (lines!=nullptr) sort(lines->begin(),lines->end());
	if (columns!=nullptr) sort(columns->begin(),columns->end());
	if (lines!=nullptr && lines->size()>0) {
		for (int i=lines->size()-1;i>=0;i--) {
			if (lines->at(i)>=0 && matrix->size()>lines->at(i)) { 
			  matrix->erase(matrix->begin()+lines->at(i));
			}
		}
	}
	
	if (columns!=nullptr && columns->size()>0) {
		for (int i=0;i<matrix->size();i++) {
			for (int j=columns->size()-1;j>=0;j--) {
				if (columns->at(j)>=0 && (matrix->at(i)).size()>columns->at(j)) {
				  (matrix->at(i)).erase((matrix->at(i)).begin()+columns->at(j));
				}
			}
		}
	}
}

vector<vector<string> > tools::mat::transpose_matrix(vector<vector<string> > matrix) {
	
	tools::mat::fillup_matrix(&matrix);
	vector<vector<string> > matrix_T((matrix.at(0)).size(),vector<string>(matrix.size()));
	
	for (int i=0;i<matrix.size();i++) {
		for (int j=0;j<(matrix.at(i)).size();j++) {
			matrix_T[j][i]=(matrix)[i][j];
		}
	}
	return matrix_T;
}

bool tools::mat::find_str_in_mat(vector<vector<string>>* matrix, string find_this) {
    for (int x=0;x<matrix->size();x++) {
        for (int y=0;y<matrix->at(x).size();y++) {
            if (matrix->at(x)[y].find(find_this)!=string::npos) return true;
        }
    }
    return false;
}

bool tools::mat::find_str_in_mat(vector<string>* matrix, string find_this) {
    for (int x=0;x<matrix->size();x++)
	{
            if (matrix->at(x).find(find_this)!=string::npos) return true;
    }
    return false;
}

int tools::mat::find_in_matrix_xy_return_y(vector<vector<string>>* matrix, string find_this) {
    int y=-1;
    for (int x=0;x<matrix->size();x++) {
        for (int y=0;y<matrix->at(x).size();y++) {
            if (matrix->at(x)[y].find(find_this)!=string::npos) return y;
        }
    }
    return y;
}

int tools::mat::find_in_vec(vector<string>* vec, string find_this) {
    for (int x=0;x<vec->size();x++) {
		if (vec->at(x).find(find_this)!=string::npos) return x;
    }
    return -1;
}

int tools::mat::is_in_vec(vector<string>* vec, string find_this) {
    for (int x=0;x<vec->size();x++) {
		if (vec->at(x)==find_this) return x;
    }
    return -1;
}

void tools::mat::fillup_matrix(vector<vector<string> > *matrix) {
	int max_column_size=0;
	int size;
	for (int i=0;i<matrix->size();i++) {
		size = (matrix->at(i)).size();
		if (size>max_column_size) max_column_size=size;
	}
	for (int i=0;i<matrix->size();i++) {
		(matrix->at(i)).resize(max_column_size,"");
	}
}

vector<int> tools::mat::get_empty_columns_from_matrix(std::vector<std::vector<std::string>> *mat) {
    vector<int> cols;
    if (mat->size()==0) return cols;
    for (int j=0;j<mat->at(0).size();j++) {
        bool empty=false;
        for (int i=0;i<mat->size();i++) {
            if (mat->at(i)[j]=="") empty=true;
			else {	
			  empty = false;
			  break;
			}
        }
        if (empty) cols.push_back(j);
    }
    return cols;
}

void tools::mat::cut_matrix(vector<vector<string>>* mat) {
// 	for (int i=mat->size()-1;i>=0;i--) {
// 		bool empty=false;
// 		for (int j=0;j<mat->at(i).size();j++) {
// 			if (mat->at(i)[j]=="") empty=true;
// 			else {	
// 			  empty = false;
// 			  break;
// 			}
// 		}
// 		if (empty) mat->erase(mat->begin()+i);
// 	}
}

void tools::mat::remove_empty_lines_from_matrix(std::vector<std::vector<std::string>> *mat) {
	for (int i=mat->size()-1;i>=0;i--) {
		bool empty=false;
		for (int j=0;j<mat->at(i).size();j++) {
			if (mat->at(i)[j]=="") empty=true;
			else {	
			  empty = false;
			  break;
			}
		}
		if (empty) mat->erase(mat->begin()+i);
	}
}

void tools::mat::remove_empty_cols_from_matrix(std::vector<std::vector<std::string>> *mat) {
    vector<vector<string>> mat_rem;
    mat_rem=tools::mat::transpose_matrix(*mat);
    tools::mat::remove_empty_lines_from_matrix(&mat_rem);
    *mat=tools::mat::transpose_matrix(mat_rem);
}


vector<vector <string> > tools::mat::format_string_to_matrix(string *mainstring, string line_delimiter, string column_delimiter) {
	string line;
	vector<string> columns_in_line;
	vector<vector< string >> matrix;
// 	cout << mainstring->length() << endl;
// 	cout << "line_delimiter=" << line_delimiter << endl;
// 	cout << "OUT\n";
	vector<string> lines = tools::str::get_strings_between_delimiter(*mainstring,line_delimiter);
// 	cout << "OUT2\n";
	for (int i=0;i<lines.size();i++) {
		if (lines[i].length()==0) continue;
		line = lines[i];
		tools::str::replace_chars(&(line),"\r",""); // win
		if (line.length()>0) {
			columns_in_line = tools::str::get_strings_between_delimiter(line,column_delimiter,true);
			matrix.push_back(columns_in_line);
		}
	}
// 	cout << "OUT3\n";
	return matrix;
}

string tools::mat::format_matrix_to_string(vector<vector<string> > *matrix, string line_delimiter, string column_delimiter) 
{
	stringstream ss;
	for (int i=0;i<matrix->size();i++) {
		if (i!=0) ss <<line_delimiter;
		for (int j=0;j<(matrix->at(i)).size();j++) {
			if (((*matrix)[i][j]).size()>0) {
				if (j!=0) ss << column_delimiter;// content=content +column_delimiter;
				ss << (*matrix)[i][j];
			} else {
				if (j!=0) ss<< column_delimiter; // content=content +column_delimiter;
				ss << "";	// content=content+"";
			}
		}
	}
	return ss.str();
}

// string tools::mat::format_matrix_to_string(vector<vector<string> > *matrix, string line_delimiter, string column_delimiter) {
// 	string content;
// 	for (int i=0;i<matrix->size();i++) {
// 		if (i!=0) content=content+(line_delimiter);
// 		for (int j=0;j<(matrix->at(i)).size();j++) {
// 			if (((*matrix)[i][j]).size()>0) {
// 				if (j!=0) content=content +column_delimiter;
// 				content=content+((*matrix)[i][j]);
// 			} else {
// 				if (j!=0) content=content +column_delimiter;
// 				content=content+"";
// 			}
// 		}
// 		
// 	}
// 	return content;
// }
