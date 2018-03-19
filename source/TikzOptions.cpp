#include "TikzOptions.hpp"

TikzOptions::TikzOptions(std::string options)
	: std::map< std::string, std::string >()
{
	Add(options);
}

/** Add some tikz options, can be a single options or a list of options
 * spearated by commas. Arguments containing commas should be wrapped in curly
 * brackets. Leading and trailing white space will be stripped from the option
 * name and argument.
 *
 * \param[in] options The options to be added.
 */
void TikzOptions::Add(const std::string &options) {
	//If the string is empty do nothing.
	if (options == "") return;

	//Loop over every option assuming the brek is on a comma.
	size_t start = 0;
	size_t end = options.find_first_of(",");
	while (end != std::string::npos) {
		//Check if we are between curly brackets, if so find the next comma and
		//continue without parsing the current broken argument
		if (end > options.find_last_of("{") && end < options.find_first_of("}")) {
			end = options.find_first_of(",", end + 1);
			continue;
		}

		//Emplace the parsed argument into the map.
		this->emplace(ParseOption(options.substr(start, end - start)));

		//Compute the start and end positions of the next option.
		start = end + 1;
		end = options.find_first_of(",", start);
	}
	//Emplace the remaining option.
	this->emplace(ParseOption(options.substr(start)));
}

bool TikzOptions::IsDefined(const std::string &optionName) {
	if (this->find(optionName) != this->end())
		return true;
	return false;
}

/** Parses a tikz option string by separating on the first equal sign.
 *
 * \param[in] option The option to be parsed.
 * \return A pair where the first item is the option name and the second is
 *  	the argument.
 */
std::pair< std::string, std::string> TikzOptions::ParseOption(const std::string &option) {
	size_t equal_loc = option.find_first_of("=");
	std::string optionName = Strip(option.substr(0, equal_loc));
	std::string optionArgument = Strip(option.substr(equal_loc+1));

	return make_pair(optionName, optionArgument);
}

/** Return a string wit all leading and trailing spaces, tabs, and new lines
 * stripped from the input.
 *
 * \param[in] str The input string that should have white space removed.
 * \return A new string stripped of spaces.
 */
std::string TikzOptions::Strip(const std::string &str) {
	size_t begin = str.find_first_not_of(" \t\n");
	size_t end = str.find_last_not_of(" \t\n") + 1;
	return str.substr(begin, end - begin);
}
