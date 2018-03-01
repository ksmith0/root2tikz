/**
 * @author Karl Smith
 * @date Mar 1, 2018
 */

#ifndef PGFPLOTSAXIS_HPP
#define PGFPLOTSAXIS_HPP

#include <array>
#include <string>
#include <vector>
#include <utility>

#include "PgfPlotsPlot.hpp"

class PgfPlotsAxis {
	public:
		PgfPlotsAxis(const std::string &options="");

		/// Add a plot item to the axis.
		void AddPlot(PgfPlotsPlot *plot);

		/// Converts a ROOT TLatex string to a valid LaTeX string.
		static std::string GetLatexString(std::string str);

		/// Specify the log mode for a given axis.
		void SetLog(const short &axis, const bool &logMode = true);

		/// Write out the axis and registered plots.
		void Write(std::streambuf *buf = std::cout.rdbuf());

	private:
		/// Generate the tikz plot options for log mode.
		std::string LogModeOptions();

		/// Vector of registered plots.
		std::vector< PgfPlotsPlot* > plots_;

		/// Options for the axis command.
		std::string options_;

		/// Axis titles; x=0, y=1, z=2.
		std::array< std::string, 3 > axisTitles_;

		/// An array of bool indicating if the axis should be in log mode.
		std::array< bool, 3 > logMode_;

		/// The axis limits: x=0, y=1, z=2; first is min, second is max.
		std::array< std::pair< double, double >, 3 > axisLimits_;

};

#endif //PGFPLOTSAXIS_HPP
