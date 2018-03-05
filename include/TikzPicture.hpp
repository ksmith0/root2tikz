/**
 * @author Karl Smith
 * @date Mar 1, 2018
 */
#ifndef TIKZPICTURE_HPP
#define TIKZPICTURE_HPP

#include <string>
#include <vector>

#include "PgfPlotsAxis.hpp"

class TikzPicture {
	public:
		TikzPicture(const std::string &options="", const std::string &preamble="");

		/// Add an axis or group plot to this picture.
		void Add(PgfPlotsAxis* obj);

		/// Specify a colorbrewer2 palette to be used.
		void SetPalette(std::string colorbrewer2_palette);

		/// Write the picture and registered axis and plots.
		void Write(const std::string &filename = "");

	private:
		/// The vector of registered axes.
		std::vector< PgfPlotsAxis* > axes_;

		/// Colorbrewer pallete
		std::string colorbrewer2_palette_;

		/// Additional options for the picture argument.
		std::string options_;

		/// Additional preamble material.
		std::string preamble_;
};

#endif //TIKZPICTURE_HPP
