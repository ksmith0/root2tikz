/**
 * @author Karl Smith
 * @date Mar 5, 2018
 */

#ifndef PGFPLOTSGROUPPLOT_HPP
#define PGFPLOTSGROUPPLOT_HPP

#include "PgfPlotsAxis.hpp"
#include "PgfPlotsGroupSubPlot.hpp"

class PgfPlotsGroupPlot : public PgfPlotsAxis {
	public:
		PgfPlotsGroupPlot(unsigned int rows, unsigned int columns, std::string options="");

		/// Add a plot item to the axis.
		void AddPlot(PgfPlotsPlot *plot, unsigned int plotId);

		/// Get the sub plot object for a given id.
		PgfPlotsGroupSubPlot* GetSubPlot(unsigned int plotId);

	private:
		/// The registered sub plots.
		std::vector< PgfPlotsGroupSubPlot* > subPlots_;

		/// The LaTeX command starting the environment.
		std::string EnvHeader() {return "\\begin{groupplot}";};

		/// The LaTeX command ending the environment.
		std::string EnvFooter() {return "\\end{groupplot}";};

		/// Write out the group plot and registered sub plots.
		void WriteRegisteredItems(std::streambuf *buf);
};

#endif // PGFPLOTSGROUPPLOT_HPP