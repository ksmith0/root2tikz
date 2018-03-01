/**
 * @author Karl Smith
 * @date Mar 1, 2018
 */
#ifndef PGFPLOTSPLOT_HPP
#define PGFPLOTSPLOT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include <TH1.h>

class PgfPlotsPlot {
	public:
		PgfPlotsPlot(const TH1* hist, const std::string options="");

		/// Add a node to the plot.
		void AddNode(const std::string nodeLabel, const std::string nodeOptions="");

		const TH1* GetHist() {return hist_;};

		/// Generate the string to create a node for a plot.
		static std::string NodeString(std::string nodeLabel, 
				                        std::string nodeOptions="");

		/// Write out the TikZ addplot command for the given arguments.
		static std::string PlotTH1(const TH1 *hist, const std::string &options="");

		/// Write out the plot.
		void Write(std::streambuf *buf = std::cout.rdbuf());

	private:
		/// Pointer to the hist object.
		const TH1* hist_;

		/// Options for the plot command.
		std::string options_;

		/// Node label and options.
		std::vector< std::pair< std::string, std::string > > nodes_;
};
#endif //PGFPLOTSPLOT_HPP
