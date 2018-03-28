#include "PgfPlotsPlot.hpp"

#include <sstream>
#include <stdexcept>

PgfPlotsPlot::PgfPlotsPlot(const TH1* hist, const std::string &rootStyle,
                           const std::string &options)
	: obj_(hist), options_(options), rootStyle_(rootStyle)
{
	if (!hist) {
		throw std::runtime_error("ERROR: Null histogram pointer!");
	}
}

PgfPlotsPlot::PgfPlotsPlot(const TGraph* graph, const std::string &rootStyle,
	                        const std::string &options)
	: obj_(graph), options_(options), rootStyle_(rootStyle)
{
	if (!graph) {
		throw std::runtime_error("ERROR: Null histogram pointer!");
	}
}

void PgfPlotsPlot::AddNode(const std::string nodeLabel,
                           const std::string nodeOptions)
{
	nodes_.push_back(std::make_pair(nodeLabel, nodeOptions));
}

/**
 * \param[in] hist Pointer to the histogram to be plotted.
 * \param[in] rootStyle The options to use when plotting the histogram. Currently
 *  supported:
 *    * ""   - Same as HIST below.
 *    * HIST - Draw just the histogram.
 * 	* E    - Draw error bars, show only the bars no markers or lines.
 * 	* E1   - Draw error bars with small lines at end and show markers.
 * \param[in] options Options to the pgfplots plot command.
 *
 * \return A string containing the pgfplots code to render the plot.
 */
std::string PgfPlotsPlot::PlotTH1(const TH1 *hist,
                                  const std::string &rootStyle,
                                  const std::string &options)
{
	bool includeErrors = false; //Include errors, shows only the error bars, no markers. ROOT option E.
	bool errorMarks = false; //Small lines are darwn at end of the error bars and markers are shown. ROOT option E1.
	if (rootStyle.find("E") != std::string::npos) includeErrors = true;
	if (rootStyle.find("E1") != std::string::npos) errorMarks = true;

	std::stringstream output;

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
		if (errorMarks || (hist->GetBinContent(xbin) != 0 || hist->GetBinContent(xbin - 1) != 0)) {
			output << "(";
			if (includeErrors) output << hist->GetBinCenter(xbin);
			else output << hist->GetBinLowEdge(xbin);
			output << "," << hist->GetBinContent(xbin) << ") ";
			if (includeErrors) output << " +- (" << hist->GetBinWidth(xbin) / 2 <<
				"," << hist->GetBinError(xbin) << ") ";
		}
	}

	//Add a final coordinate to extend the right edge of the last bin to zero.
	if (!includeErrors && hist->GetBinContent(hist->GetNbinsX()) != 0) {
		output << "(" << hist->GetBinLowEdge(hist->GetNbinsX()) +
			hist->GetBinWidth(hist->GetNbinsX()) << "," << 0 << ") ";
	}

	//Coordinate list trailer.
	output << "};\n\n";

	return output.str();
}

/**
 * \param[in] graph Pointer to the graph to be plotted.
 * \param[in] rootStyle The options to use when plotting the graph. Currently
 *  supported:
 *    * "" - Default option is "PL".
 *    * P  - Draw points.
 *    * L  - Draw a connecting line.
 * \param[in] options Options to the pgfplots plot command.
 *
 * \return A string containing the pgfplots code to render the plot.
 */
std::string PgfPlotsPlot::PlotTGraph(const TGraph *graph,
                                     const std::string &rootStyle,
                                     const std::string &options)
{
	std::stringstream output;

	bool marks = false;
	bool lines = false;
	if (rootStyle == "") {
		marks = true;
		lines = true;
	}
	if (rootStyle.find("P") != std::string::npos) marks = true;
	if (rootStyle.find("L") != std::string::npos) lines = true;

	//Setup the plot style
	output << "\t\\addplot+[\n";
	if (!lines) output << "\t\t\tonly marks,\n";
	if (!marks) output << "\t\t\tmark=none,\n";
	output << options;

	output << "\t\t]\n";

	//Begin the cooridnate list
	output << "\t\tcoordinates { ";

	for (int point = 0; point < graph->GetN(); point++) {
		output << "(" << graph->GetX()[point] << ","
			<< graph->GetY()[point] << ") ";
	}

	//Coordinate list trailer.
	output << "};\n\n";

	return output.str();
}

void PgfPlotsPlot::Write(std::streambuf *buf) {
	std::string plot_str;
	auto hist = GetHist();
	auto graph = GetGraph();
	if (hist) plot_str = PlotTH1(hist, rootStyle_, options_.GetString());
	else if (graph) plot_str = PlotTGraph(graph, rootStyle_, options_.GetString());
	size_t loc = plot_str.find_last_of("}") + 1;
	for (auto nodeInfo : nodes_) {
		plot_str.insert(loc, "\n\t\t" + NodeString(nodeInfo.first, nodeInfo.second));
	}

	std::ostream output(buf);
	output << plot_str;
}

std::string PgfPlotsPlot::NodeString(std::string nodeLabel, std::string nodeOptions) {
	std::stringstream output;

	output << "node";
	if (nodeOptions != "") {
		output << "[" << nodeOptions << "]";
	}

	output << " {" << nodeLabel << "}";

	return output.str();
}
