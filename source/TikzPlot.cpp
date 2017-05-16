/**Script to output ROOT graphics to pgfPlots.
 *
 * @author Karl Smith
 * @date May 2, 2017
 */

#include "TikzPlot.hpp"

#include <fstream>

#include <TH1.h>

TikzPlot::TikzPlot() :
	logMode_({false})
{

}

/**
 * \param[in] hist A pointer to the histogram to be converted to a TikZ object.
 * \param[in] options Plotting options. See PlotTH1 for currently supported options.
 */
void TikzPlot::Add(TH1* hist, const std::string &options) {
	hists_.push_back(std::make_pair(hist, options));
	if (axisTitles_.at(0) == "") axisTitles_.at(0) = hist->GetXaxis()->GetTitle();
	if (axisTitles_.at(1) == "") axisTitles_.at(1) = hist->GetYaxis()->GetTitle();
}

/**
 * \param[in] hist A pointer to the histogram to be converted to a TikZ object.
 * \param[in] options Plotting options.
 */
void TikzPlot::Add(TH2* hist, const std::string &options) {
	if (! hists_.empty()) {
		std::cerr << "ERROR: Multiple TH2 plotting is not currently supported, request ignored!\n";
		return;
	}
	hists_.push_back(std::make_pair(hist, options));
	if (axisTitles_.at(0) == "") axisTitles_.at(0) = hist->GetXaxis()->GetTitle();
	if (axisTitles_.at(1) == "") axisTitles_.at(1) = hist->GetYaxis()->GetTitle();
}



void TikzPlot::SetLog(const short &axis, const bool &logMode /* = true */) {
	if (axis >= 3) {
		std::cerr << "ERROR: TikzPlot::SetLog called for invalid axis index: " << axis << "!\n";
		return;
	}

	logMode_.at(axis) = logMode;
}

/**Creates a TikZ picture using the content from the provided histogram. THe
 * options passed control the output style.
 *
 * \param[in] filename
 */
void TikzPlot::Write(const std::string &filename /* = "" */) {
	std::streambuf *buf = std::cout.rdbuf();

	std::ofstream outputStream;
	if (filename != "") {
		outputStream.open(filename);
		buf = outputStream.rdbuf();
	}
	std::ostream output(buf);

	output << 
		"\\begin{tikzpicture}\n"
		"\t\\begin{axis}[\n"
		"\t\txlabel={" << axisTitles_.at(0) <<  "},\n"
		"\t\tylabel={" << axisTitles_.at(1) <<  "},\n"
		"\t\txmin=" << axisLimits_.at(0).first << ", "
		"xmax=" << axisLimits_.at(0).second << ",\n"
		"\t\tymin=" << axisLimits_.at(1).first << ","
		"ymax=" << axisLimits_.at(1).second << " ";

	//Need this option to filter out points outside the x range.
	//	output << "restrict x to domain*=
	output << "]\n\n";

	for (auto itr : hists_) {
		TH1* &hist = itr.first;
		std::string &options = itr.second;
		PlotTH1(hist, options, buf);
	}

	output <<
		"\t\\end{axis}\n"
		"\\end{tikzpicture}\n";

	if (outputStream.is_open()) {outputStream.close();}
}

/**
 * \param[in] hist Pointer to the histogram to be plotted.
 * \param[in] options The options to use when plotting the histogram. Currently supported:
 *    * ""   - Same as HIST below.
 *    * HIST - Draw just the histogram.
 * 	* E    - Draw error bars, show only the bars no markers or lines.
 * 	* E1   - Draw error bars with small lines at end and show markers.
 * \param[in] buf The streambuf that the output should be directed to. The default is directed to std::cout.
 */
void TikzPlot::PlotTH1(const TH1 *hist, const std::string &options,
		std::streambuf *buf /* = std::cout.rdbuf() */)
{
	bool includeErrors = false; //Include errors, shows only the error bars, no markers. ROOT option E.
	bool errorMarks = false; //Small lines are darwn at end of the error bars and markers are shown. ROOT option E1.
	if (options.find("E") != std::string::npos) includeErrors = true;
	if (options.find("E1") != std::string::npos) errorMarks = true;

	std::ostream output(buf);

	//Setup the plot style
	output << "\t\\addplot+[";

	//If th e error option was indicated
	if (includeErrors) {
		// exclude markers if not requested.
		if (!errorMarks) output << "scatter, mark=none, ";
		//Remove the line connecting the points.
		output << "only marks, ";
		//Setup the error bars.
		output << "error bars/.cd, y dir=both, y explicit, x dir=both, x explicit";
		//Remove the edges (marks) at the end of the error bars.
		if (!errorMarks) output << ", error mark = none";
	}
	//Otherwise we use a const plot iwth no marks to show bins.
	else output << "const plot, no marks";

	output << "]\n";

	//Begin the cooridnate list
	output << "\t\tcoordinates { ";

	//Add an initial coordinate to extend the left edge of the first bin to zero.
	if (!includeErrors) output << "(" << hist->GetBinLowEdge(1) << ",0) ";

	//Loop over every bin and add a corrdinate for it
	for (int xbin=1; xbin <= hist->GetNbinsX(); xbin++) {
		//Suppress the bins containing zero counts to speed up LaTeX rendering.
		if (hist->GetBinContent(xbin) != 0 && hist->GetBinContent(xbin - 1) != 0) {
			output << "(";
			if (includeErrors) output << hist->GetBinCenter(xbin);
			else output << hist->GetBinLowEdge(xbin);
			output << "," << hist->GetBinContent(xbin) << ") ";
			if (includeErrors) output << " +- (" << hist->GetBinWidth(xbin) / 2 <<
				"," << hist->GetBinError(xbin) << ") ";
		}
	}

	//Add a final coordinate to extend the right edge of the last bin to zero.
	if (!includeErrors) {
		output << "(" << hist->GetBinLowEdge(hist->GetNbinsX()) +
			hist->GetBinWidth(hist->GetNbinsX()) << "," << 0 << ") ";
	}

	//Coordinate list trailer.
	output << "};\n\n";
}
