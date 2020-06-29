
#include "parser_methods.images_t.hpp"

images_t::images_t()
{
	/// overwrites parser_methods::tool_name
	tool_name = "";
	/// overwrites parser_methods::file_type_endings
	file_type_endings = {"jpg","jpeg"};
	/// this is not a primary measurement
	set_secondary_measurement();
}


/// Order is important
bool images_t::parse(filename_t* filename_p, string* contents)
{
	filename = filename_p;
// 	file_contents = contents;
// 	copy_to_measurement_p();
	return true;
}

