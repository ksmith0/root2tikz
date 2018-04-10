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

#include <TNamed.h>
#include <TGraph.h>
#include <TH1.h>
#include <TH2.h>

#include "TikzOptions.hpp"

class PgfPlotsPlot {
	public:
		PgfPlotsPlot(const TH1* hist, const std::string &rootStyle="",
		             const std::string &options="");
		PgfPlotsPlot(const TGraph* graph, const std::string &rootStyle="",
		             const std::string &options="");

		/// Add a node to the plot.
		void AddNode(const std::string nodeLabel, const std::string nodeOptions="");

		const TNamed* GetObj() {return obj_;};
		const TGraph* GetGraph() {return dynamic_cast<const TGraph*>(obj_);};
		const TH1* GetHist1d();
		const TH2* GetHist2d() {return dynamic_cast<const TH2*>(obj_);};

		/// Return a pointer to the TikzOptions object.
		TikzOptions* GetOptions() {return &options_;}

		/// Generate the string to create a node for a plot.
		static std::string NodeString(std::string nodeLabel,
		                              std::string nodeOptions="");

		/// Write out the TikZ addplot command for the given arguments.
		static std::string PlotTH1(const TH1 *hist, const std::string &rootStyle="",
		                           const std::string &options="");
		static std::string PlotTGraph(const TGraph *graph,
		                              const std::string &rootStyle="",
		                              const std::string &options="");
		static std::string PlotTH2(const TH2 *hist, const std::string &rootStyle="",
		                           const std::string &options="");

		/// Write out the plot.
		void Write(std::streambuf *buf = std::cout.rdbuf());

	private:
		/// Pointer to the hist object.
		const TNamed* obj_;

		/// Options for the plot command.
		TikzOptions options_;

		/// The root style to use.
		std::string rootStyle_;

		/// Node label and options.
		std::vector< std::pair< std::string, std::string > > nodes_;
};
#endif //PGFPLOTSPLOT_HPP
