
#ifndef PARSER_METHODS_IMAGES_T_HPP
#define PARSER_METHODS_IMAGES_T_HPP

#include <set>
#include <string>
#include <map>
#include "parser_methods.hpp"

/*handles parsing of picture files (at the moment just the filename)*/
class images_t: public parser_methods 
{
private:
	
public:	
	/// tries to parse the filename: and overwrites parser_t::parse()
	bool parse(filename_t* filename_p, string* contents);
	images_t();
};

#endif // PARSER_METHODS_IMAGES_T_HPP
