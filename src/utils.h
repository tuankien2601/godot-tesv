#include <regex>
#include <godot_cpp/variant/vector3.hpp>

std::string sanitize_string(std::string str)
{
	std::regex pattern("[^a-zA-Z0-9]+");
	return std::regex_replace(str, pattern, "_");
}

