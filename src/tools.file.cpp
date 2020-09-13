#include "tools.hpp"


time_t tools::file::creation_date(string filename)
{
	
	struct stat t_stat;
// 	cout << filename << endl;
	stat(filename.c_str(),&t_stat);
// 	stat("data/51398_SJZ649_w4_450VO+_2.dp_rpc_asc",&t_stat);
	struct tm *creation_date = localtime(&t_stat.st_ctime);
// 	asctime(creation_date);
// 	printf("File time and date: %s", asctime(creation_date));
	return t_stat.st_ctime;
// 	time_t test = 0;
// 	return test;
}

vector<string> tools::file::filter_files_from_types(vector<string> * file_list, vector<string> * file_types, bool exclude) {
    vector<string> included_files, excluded_files;
    bool found=false;
    for (int i=0;i<file_list->size();i++) { // iterate the file_list
        found = false;
        for (int j=0;j<file_types->size();j++) { // iterate the config file endings (cfg or conf ...)
            if (file_list->at(i).find(file_types->at(j),file_list->at(i).length()-file_types->at(j).length())!=string::npos) {
                included_files.push_back(file_list->at(i));
                found = true;
                break;
            }
        }
        if (!found) excluded_files.push_back(file_list->at(i));
    }
    if (exclude) return excluded_files;
    return included_files;
}

void tools::file::remove_file_type_from_name(string* filename, string filetye_delimiter) 
{
	if (filetye_delimiter=="") return;
	vector<string> file_parts=tools::str::get_strings_between_delimiter(*filename,filetye_delimiter);
	*filename = file_parts[0];
}

void tools::file::remove_file_type_from_name(vector<string>* filenames, string filetye_delimiter) 
{
	for (int i=0;i<filenames->size();i++)
	{
		remove_file_type_from_name(&filenames->at(i),filetye_delimiter);
	}
}


/*remove path structure and optinally file ending from the name*/
string tools::file::extract_filetype_ending(string filename,string filetye_delimiter) 
{
	if (filetye_delimiter=="") return "";
	string filename_with_type=(tools::str::get_strings_between_delimiter(filename,PATH_DELIMITER)).back();
	string filename_type=tools::str::get_strings_between_delimiter(filename_with_type,filetye_delimiter).back();
    return filename_type;
}

/*remove path structure and optinally file ending from the name*/
string tools::file::extract_filename(string filename,string filetye_delimiter) {
	string filename_with_type=(tools::str::get_strings_between_delimiter(filename,PATH_DELIMITER)).back();
	if (filetye_delimiter=="") return filename_with_type;
	vector<string> file_parts=tools::str::get_strings_between_delimiter(filename_with_type,filetye_delimiter);
	
	if (file_parts.size()<2) return filename;
    return file_parts[0];
}

/*remove path structure and optinally file ending from the name*/
vector<string> tools::file::extract_filenames(vector<string> filenames,string filetye_delimiter) {
    vector<string> extraced_filenames;
	for (int i=0;i<filenames.size();i++) {
        extraced_filenames.push_back(extract_filename(filenames[i],filetye_delimiter));
    }
	return extraced_filenames;
}

string tools::file::extract_directory_from_filename(string filename) {
    
	string directory="";
	vector<string> directory_parts = tools::str::get_strings_between_delimiter(filename,PATH_DELIMITER);
	if (directory_parts.size()<2) {
		directory=string(".")+PATH_DELIMITER;
		return directory;
	}
	for (int i=0; i<directory_parts.size()-1;i++) {
		directory.append(directory_parts[i]);
		directory.append(PATH_DELIMITER);
	}
	if (directory.compare(directory.size()-PATH_DELIMITER.size(),PATH_DELIMITER.size(),PATH_DELIMITER)!=0) {
		directory.append(PATH_DELIMITER);
	}
	return directory;
}


void tools::file::remove_files(vector<string> files) {
    for (int i=0;i<files.size();i++) {
        remove( files[i].c_str() );
    }
    return;
}

vector<string> tools::file::load_file_to_vector_string(string filename_with_path) {
    vector<string> contents;
    #ifdef __unix__
    #else
	tools::str::replace_umlaute_to_windows_compatible(&filename_with_path);
    #endif
	ifstream myfile;
	string line;
	myfile.open (filename_with_path.c_str(),ios::in);
	if (!myfile.is_open()) return contents;
	contents.clear();
	while ( getline (myfile,line) ) {
		contents.push_back(tools::str::remove_linebreakers_from_string(line));
	}
	myfile.close();
	return contents;
}

// OBSOLET
// string tools::file::load_file_to_string(string filename_with_path) {
//     string contents="";
//     #ifdef __unix__
//     #else
// 	tools::str::replace_umlaute_to_windows_compatible(&filename_with_path);
//     #endif
// 	streampos size;
// 	char * memblock;
// 	ifstream myfile;
// 	myfile.clear();
//     string line="";
// 	myfile.open (filename_with_path.c_str(), ios::in|ios::binary|ios::ate);
// 	if (!myfile.is_open()) return contents;
// 	size = myfile.tellg();
// 	
//     memblock = new char [size];
// 	
//     myfile.seekg (0, ios::beg);
//     myfile.read (memblock, size);
//     myfile.close();
// 	/// ERROR ///
// 	contents = std::string(memblock);
// 	cout << "contents.size()="<<contents.size() << endl;
// 	/////////////
// 	delete[] memblock;
// 	return contents;
// }

string tools::file::load_file_to_string(string filename_with_path) {
	std::ifstream in(filename_with_path, std::ios::in | std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
// 	throw(errno);
	return "";
}


vector<vector<string>> tools::file::load(string filename, string delimiter) {
    vector<vector<string>> mat;
    ifstream myfile;
    string line;
	myfile.open (filename.c_str());
	if (!myfile.is_open()) return mat;
    while ( getline (myfile,line) )   {
		line = tools::str::remove_linebreakers_from_string(line);
        mat.push_back(tools::str::get_strings_between_delimiter(line,delimiter));
    }
    myfile.close();
    return mat;
}

bool tools::file::write_mat_to_file(string filename_with_path,vector<vector<double>> *matrix, bool append, string column_delimiter, string line_delimiter) {
    ofstream myfile;
    if (append) myfile.open (filename_with_path.c_str(), ios::app);
	else myfile.open (filename_with_path.c_str());
	if (!myfile.is_open()) return false;
    for (int l=0;l<matrix->size();l++) {
        for (int c=0;c<matrix->at(l).size()-1;c++) {
            myfile << to_string(matrix->at(l)[c]);
            myfile << column_delimiter;
        }
        myfile << to_string((matrix->at(l)).back());
        myfile << line_delimiter;
    }
	myfile.close();
	return true;
}

bool tools::file::write_mat_to_file(string filename_with_path,vector<vector<string>> *matrix, bool append, string column_delimiter, string line_delimiter) {
    ofstream myfile;
    if (append) myfile.open (filename_with_path.c_str(), ios::app);
	else myfile.open (filename_with_path.c_str());
	if (!myfile.is_open()) return false;
    for (int l=0;l<matrix->size();l++) {
        for (int c=0;c<matrix->at(l).size()-1;c++) {
            myfile << matrix->at(l)[c];
            myfile << column_delimiter;
        }
        myfile << (matrix->at(l)).back();
        myfile << line_delimiter;
    }
	myfile.close();
	return true;
}

bool tools::file::write_to_file(string filename_with_path,string *contents, bool append) {
// 	cout << filename_with_path << endl;
  #ifdef __unix__
#else
	tools::str::replace_umlaute_to_windows_compatible(&filename_with_path);
#endif
// 	cout << filename_with_path << endl;
	ofstream myfile;
	if (append) myfile.open (filename_with_path.c_str(), ios::app);
	else myfile.open (filename_with_path.c_str());
	if (!myfile.is_open()) return false;
//     if (!append) myfile << "";
	myfile << *contents;
	myfile.close();
	return true;
}


string tools::file::check_directory_string(string add_directory, string root_dir) {
    /*if empty, then return the working dir*/
	if (add_directory=="" && root_dir=="") return "";
    
    /*if empty, then return the root dir*/
	if (add_directory=="") return tools::file::check_directory_string(root_dir,"");
	
    /*add path_delimiter at end of add_directory, if not exists*/
    if (add_directory.compare(add_directory.size()-PATH_DELIMITER.size(),PATH_DELIMITER.size(),PATH_DELIMITER)!=0) {
        add_directory.append(PATH_DELIMITER);
    }
       
    /*if add_directory is a directory structure or root_dir is "", then return add_directory*/
    if (add_directory.compare(0,PATH_DELIMITER.size(),PATH_DELIMITER)==0 || add_directory.find(":")!=std::string::npos || root_dir=="") 
        return add_directory;
    
    /*add PATH_DELIMITER at end of root_directory, if not exists*/
    if (root_dir.compare(root_dir.size()-PATH_DELIMITER.size(),PATH_DELIMITER.size(),PATH_DELIMITER)!=0) {
        root_dir.append(PATH_DELIMITER);
    }   
    
    /*so add_directory is NOT a directory structure, then return root_dir + add_directory*/
	return root_dir+add_directory;
}

string tools::file::check_directory_string(string directory) {
	if (directory=="") 
	{
		directory.append(".");
		directory.append(PATH_DELIMITER);
		return directory;
	}
	/*add path delimiter at the end if not exist*/
	if (directory.compare(directory.size()-PATH_DELIMITER.size(),PATH_DELIMITER.size(),PATH_DELIMITER)!=0) 
	{
		directory.append(PATH_DELIMITER);
	}
	return directory;
}


#ifdef __unix__
typedef struct stat Stat;

int tools::file::do_mkdir(const char *path, mode_t mode)
{
    Stat            st;
    int             status = 0;

    if (stat(path, &st) != 0)
    {
        /* Directory does not exist. EEXIST for race condition */
        if (mkdir(path, mode) != 0 && errno != EEXIST)
            status = -1;
    }
    else if (!S_ISDIR(st.st_mode))
    {
        errno = ENOTDIR;
        status = -1;
    }

    return(status);
}
/**
** mkpath - ensure all directories in path exist
** Algorithm takes the pessimistic view and works top-down to ensure
** each directory in path exists, rather than optimistically creating
** the last element and working backwards.
*/
int tools::file::mkpath(string pfad, mode_t mode)
{
    const char *path = pfad.c_str();
    char           *pp;
    char           *sp;
    int             status;
    char           *copypath = strdup(path);
    
    status = 0;
    pp = copypath;
    while (status == 0 && (sp = strchr(pp, '/')) != 0)
    {
        if (sp != pp)
        {
            /* Neither root nor double slash in path */
            *sp = '\0';
            status = do_mkdir(copypath, mode);
            *sp = '/';
        }
        pp = sp + 1;
    }
    if (status == 0)
        status = do_mkdir(path, mode);
    free(copypath);
    return (status);
}
#else
int tools::file::mkpath(string path, int delme) {
	tools::str::replace_umlaute_to_windows_compatible(&path);
	string cmd = string("mkdir ") +path + string(" 2>nul");
	system(cmd.c_str() );
	return 1;
}
#endif

bool tools::file::directory_exists(string directory)
{
	struct stat info;

	if( stat( directory.c_str(), &info ) != 0 )
		// 		printf( "cannot access %s\n", pathname );
		return false;
	else if( info.st_mode & S_IFDIR )  // S_ISDIR() doesn't exist on my windows 
// 		printf( "%s is a directory\n", pathname );
		return true;
	else
// 		printf( "%s is no directory\n", pathname );
		return false;

	return true;
}

bool tools::file::file_exists(string filename) {
	ifstream f(filename.c_str());
	if (f.good()) 
	{
		f.close();
		return true;
	} 
	else 
	{
		f.close();
// 		return false;
	}
	return false;
}

vector<string> tools::file::get_file_list_from_dir_name(string dir_name, bool prepend_directory) {
	dir_name = check_directory_string(dir_name);
	vector<string> filelist;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (dir_name.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			if (strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0)
			{
				if (prepend_directory) 	filelist.push_back(dir_name +ent->d_name);
				else filelist.push_back(ent->d_name);
			}
		}
		closedir (dir);
	}
	return filelist;
}
