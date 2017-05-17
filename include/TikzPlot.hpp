/**
 * @author Karl Smith
 * @date May 3, 2017
 */
#ifndef TIKZPLOT_HPP
#define TIKZPLOT_HPP

#include <array>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <TH1.h>
#include <TH2.h>
#include <TVirtualPad.h>

class TikzPlot{
	public:
		TikzPlot();
		TikzPlot(TVirtualPad *pad);

		/// Add a one-dimensional histogram to the plot.
		void Add(TH1* hist, const std::string &options = "");

		/// Add a two-dimensional histogram to the plot.
		void Add(TH2* hist, const std::string &options = "");

		/// Specify the log mode for a given axis.
		void SetLog(const short &axis, const bool &logMode = true);

		/// Write the TikZ output to the given filename.
		void Write(const std::string &filename = "");

		/// Write out the TikZ addplot command for the given arguments.
		static void PlotTH1(const TH1* hist, const std::string &options,
				std::streambuf *buf = std::cout.rdbuf());

		/// Write out the TikZ addplot command for the given arguments.
		static void PlotTH2(const TH2* hist, const std::string &options,
				std::streambuf *buf = std::cout.rdbuf());

	private:
		/// A vector of histograms and plot options.
		std::vector< std::pair< TH1*, std::string > > hists_;

		/// Axis titles. 
		std::array< std::string, 3 > axisTitles_;

		/// An array of bool indicating if the axis should be in log mode.
		std::array< bool, 3 > logMode_;

		std::array< std::pair< double, double >, 3 > axisLimits_;

		/// A flag indicating that a 2D hist. is to be output in a color scale.
		bool is2dColor_;

};

#endif //TIKZPLOT_HPP
