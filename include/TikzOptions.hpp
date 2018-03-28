/**
 * @author Karl Smith
 * @date Mar 5, 2018
 */

#ifndef TIKZOPTIONS_HPP
#define TIKZOPTIONS_HPP

#include <map>
#include <string>
#include <utility>

class TikzOptions : public std::map< std::string, std::string > {
	public:
		TikzOptions(std::string options="");

		/// Add options.
		void Add(const std::string &options);

		bool IsDefined(const std::string &optionName);

		/// Parse an option into name and argument.
		static std::pair< std::string, std::string> ParseOption(const std::string &option);

		/// Remove leading and trailing whitespace.
		static std::string Strip(const std::string &str);

		/// Get a string representation of the options.
		std::string GetString();
};

#endif // TIKZOPTIONS_HPP
