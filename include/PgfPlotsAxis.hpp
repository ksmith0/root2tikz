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

#include "TikzOptions.hpp"
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
		/// Vector of registered plots.
		std::vector< PgfPlotsPlot* > plots_;

		/// Options for the axis command.
		TikzOptions options_;

		/// The LaTeX command starting the environment.
		std::string EnvHeader() {return "\\begin{axis}";};

		/// The LaTeX command ending the environment.
		std::string EnvFooter() {return "\\end{axis}";};
};

#endif //PGFPLOTSAXIS_HPP
