#ifndef PGFPLOTSLEGEND_HPP
#define PGFPLOTSLEGEND_HPP

#include <iostream>
#include <string>
#include <vector>

#include "TikzOptions.hpp"

class PgfPlotsLegend {
	public:
		PgfPlotsLegend();

		/// Add an entry to the legend.
		void AddEntry(const std::string &entry);

		/// Return a pointer to the legend style TikzOptions object.
		TikzOptions* GetStyle() {return &style_;}

		/// Write out the axis and registered plots.
		void Write(std::streambuf *buf = std::cout.rdbuf());

	private:
		std::vector< std::string > legendEntries_;

		/// Options for the legend style.
		TikzOptions style_;


};

#endif //PGFPLOTSLEGEND_HPP
