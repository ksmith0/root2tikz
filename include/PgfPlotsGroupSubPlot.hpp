/**
 * @author Karl Smith
 * @date Mar 5, 2018
 */

#ifndef PGFPLOTSSUBGROUPPLOT_HPP
#define PGFPLOTSSUBGROUPPLOT_HPP

#include "PgfPlotsAxis.hpp"

class PgfPlotsGroupSubPlot : public PgfPlotsAxis {
	public:
		PgfPlotsGroupSubPlot(std::string options="");

	private:
		/// The LaTeX command starting the environment.
		std::string EnvHeader() {return "\\nextgroupplot";};

		/// The LaTeX command ending the environment.
		std::string EnvFooter() {return "";};
};

#endif // PGFPLOTSSUBGROUPPLOT_HPP
