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

		void SetGlobalAxisLimits(short axis = -1, bool limitGlobally = true);

		/// Get the sub plot object for a given id.
		PgfPlotsGroupSubPlot* GetSubPlot(unsigned int plotId);

		const std::pair<unsigned short, unsigned short> GetPlotDims();

	private:
		/// The registered sub plots.
		std::vector< PgfPlotsGroupSubPlot* > subPlots_;

		std::array<bool, 3> globalAxisLimits_;

		/// The LaTeX command starting the environment.
		std::string EnvHeader() {return "\\begin{groupplot}";};

		/// The LaTeX command ending the environment.
		std::string EnvFooter() {return "\\end{groupplot}";};

		/// Perform some preprocessing prior to output.
		virtual void PreprocessOptions();

		/// Determine axis label placement.
		void ProcessAxisLabels();

		/// Determine axis label placement.
		void ProcessTickLabels();

		/// Process the axis limits.
		void ProcessGlobalLimits();

		/// Write out the group plot and registered sub plots.
		void WriteRegisteredItems(std::streambuf *buf);
};

#endif // PGFPLOTSGROUPPLOT_HPP
