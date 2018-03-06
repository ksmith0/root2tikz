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

		/// Return a pointer to the TikzOptions object.
		TikzOptions* GetOptions() {return &options_;}

		/// Specify the log mode for a given axis.
		void SetLog(const short &axis, const bool &logMode = true);

		/// Write out the axis and registered plots.
		void Write(std::streambuf *buf = std::cout.rdbuf());

	protected:
		/// Options for the axis command.
		TikzOptions options_;

	private:
		/// Vector of registered plots.
		std::vector< PgfPlotsPlot* > plots_;

		/// The LaTeX command starting the environment.
		virtual std::string EnvHeader() {return "\\begin{axis}";};

		/// The LaTeX command ending the environment.
		virtual std::string EnvFooter() {return "\\end{axis}";};

		virtual void PreprocessOptions() {};

		/// Write out all registered items.
		virtual void WriteRegisteredItems(std::streambuf *buf);
};

#endif //PGFPLOTSAXIS_HPP
