/**Script to output ROOT graphics to pgfPlots.
 *
 * @author Karl Smith
 * @date May 2, 2017
 */

#include "TikzPlot.hpp"

#include <fstream>

#include <TH1.h>

TikzPlot::TikzPlot() :
	logMode_({false}), is2dColor_(false), colorbrewer2_palette_("")
{

}

/**
 * \param[in] hist A pointer to the histogram to be converted to a TikZ object.
 * \param[in] options Plotting options. See PlotTH1 for currently supported options.
 */
void TikzPlot::Add(TH1* hist, const std::string &options /* = "" */) {
	if (!hist) {
		std::cerr << "ERROR: Null histogram pointer ignored!\n";
		return;
	}
	hists_.push_back(std::make_pair(hist, options));
	if (axisTitles_.at(0) == "") axisTitles_.at(0) = hist->GetXaxis()->GetTitle();
	if (axisTitles_.at(1) == "") axisTitles_.at(1) = hist->GetYaxis()->GetTitle();

	//Get the axis limits.
	if (axisLimits_.at(0).first > hist->GetXaxis()->GetXmin()) axisLimits_.at(0).first = hist->GetXaxis()->GetXmin();
	if (axisLimits_.at(0).second < hist->GetXaxis()->GetXmax()) axisLimits_.at(0).second = hist->GetXaxis()->GetXmax();

	double ymin, ymax;
	hist->GetMinimumAndMaximum(ymin, ymax);
	if (axisLimits_.at(1).first > ymin) axisLimits_.at(1).first = 0.9 * ymin;
	if (axisLimits_.at(1).second < ymax) axisLimits_.at(1).second = 1.1 * ymax;
}

/**
 * \param[in] hist A pointer to the histogram to be converted to a TikZ object.
 * \param[in] options Plotting options.
 */
void TikzPlot::Add(TH2* hist, const std::string &options /* = "" */) {
	if (!hist) {
		std::cerr << "ERROR: Null histogram pointer ignored!\n";
		return;
	}
	if (! hists_.empty()) {
		std::cerr << "ERROR: Multiple TH2 plotting is not currently supported, request ignored!\n";
		return;
	}

	//Check if this is a color bar plot.
	if (options.find("COL") != std::string::npos) is2dColor_ = true;

	//Options are not correctly suppported sow we set this to true no matter what.
	is2dColor_ = true;

	//Add the histogram to the map.
	hists_.push_back(std::make_pair(hist, options));

	//Check if the axis titles are set, if not use them form this hist.
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

/** Makes use of the colorbrewer package which uses the palette on
 *  http://colorbrewer2.org
 *
 * \param[in] colorbrewer2_palette Name of the palette to be used.
 */
void TikzPlot::SetPalette(std::string colorbrewer2_palette) {
	std::vector<std::string> valid_set_names = {"Set1", "Set2", "Set3", "Dark2", "Paired", "Pastel1", "Pastel2", "Accent"};

	//Check if the first part of the palette matches known sets.
	bool palette_found = false;
	for (auto name : valid_set_names) {
		if (colorbrewer2_palette.find(name) != std::string::npos) {
			palette_found = true;
			break;
		}
	}
	if (!palette_found) {
		std::cerr << "WARNING: Colorbrewer2 palette '" << colorbrewer2_palette << "' may not exist.\n";
	}
	colorbrewer2_palette_ = colorbrewer2_palette;
}

std::string TikzPlot::GetLatexString(std::string str) {
	//Replace # with \ and wrap in $.
	std::size_t loc = str.find_first_of("#");
	while (loc != std::string::npos) {
		str[loc] = '\\';
		str.insert(loc++, "$");
		loc = str.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", loc+1);
		str.insert(loc, "$");
		loc = str.find_first_of("#", loc);
	}
	//Wrap math commands in $
	loc = str.find_first_of("{");
	while (loc != std::string::npos) {
		loc = str.find_last_of(" \\^_", loc);
		str.insert(loc++, "$");
		loc = str.find_first_of("}", loc+1);
		str.insert(++loc, "$");
		loc = str.find_first_of("{", loc);
	}
	return str;
}

std::string TikzPlot::LogModeOptions() {
	std::vector<std::string> options;
	if (logMode_.at(0)) options.push_back("xmode=log, ");
	if (logMode_.at(1)) options.push_back("ymode=log, ");
	if (logMode_.at(2)) options.push_back("zmode=log, ");

	std::string output = "";
	for (auto item : options) {
		output.append(item);
	}

	return output;
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

	if (colorbrewer2_palette_ != "") {
		output << "\\usepgfplotslibrary{colorbrewer}\n"
			"\\pgfplotsset{cycle list/" << colorbrewer2_palette_ << "}\n";
	}

	output <<
		"\\begin{tikzpicture}\n"
		"\t\\begin{axis}[\n"
		"\t\t" << LogModeOptions() << "\n"
		"\t\txlabel={" << GetLatexString(axisTitles_.at(0)) <<  "},\n"
		"\t\tylabel={" << GetLatexString(axisTitles_.at(1)) <<  "},\n"
		"\t\txmin=" << axisLimits_.at(0).first << ", "
		"xmax=" << axisLimits_.at(0).second << ",\n"
		"\t\tymin=" << axisLimits_.at(1).first << ", "
		"ymax=" << axisLimits_.at(1).second;

	if (is2dColor_) {
		TH1 *hist = hists_.at(0).first;

		output << ",\n"
			"\t\tview={0}{90}, \%Top down view.\n"
			"\t\tcolorbar, colorbar style={\n"
			"\t\t\t\%To place the label on top of the colorbar use `title` instead of `ylabel`.\n"
			"\t\t\tylabel={" << hist->GetZaxis()->GetTitle() << "}\n"
			"\t\t},\n"
			"\t\t\%Colorbar limits:\n"
			"\t\tpoint meta min=" << hist->GetMinimum() << ", "
			"point meta max=" << hist->GetMaximum() << ",\n"
			"\t\trestrict z to domain*=" <<
				hist->GetBinContent(hist->GetMinimumBin()) << ":" <<
				hist->GetBinContent(hist->GetMaximumBin()) << "\n";

		output <<
			"\% Uses `matrix plot` which creates a filled patch at the middle\n"
			"\% of the coordinates provided. The `surf` type plot is similar,\n"
			"\% but expects the values of the corners to be provided. This   \n"
			"\% requires an extra row and column of values are provided. This\n"
			"\% would cause the plot to omit the final row and column of     \n"
			"\% bins. The `matrix plot` does not permit any holes in the     \n"
			"\% data, thus `restrict z to domain*` must be used which sets   \n"
			"\% the z value for a point that exceeds the domain to the limit \n"
			"\% it encountered.\n";
	}

	//Need this option to filter out points outside the x range.
	//	output << "restrict x to domain*=
	output << "]\n\n";

	for (auto itr : hists_) {
		TH1* &hist = itr.first;
		std::string &options = itr.second;
		if (TH2* hist2d = dynamic_cast<TH2*>(hist)) PlotTH2(hist2d, options, buf);
		else PlotTH1(hist, options, buf);
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

/**By default creates a TikZ matrix plot which is typically used to plot values
 * in a matrix, but can be easily adapted to a two-dimensional histogram. A
 * matrix plot requires that all bins are defined, and thus the option
 * "restrict z to domain*" is used so that empty bins are set to the minimum
 * value. If the option "surf" is provided a surface plot is created instead,
 * this type of plot requires data at the corners to be provided and thus plots
 * one less bin than is contained in the input histogram.
 *
 * \param[in] hist Pointer to the histogram to be plotted.
 * \param[in] options The options to use when plotting the histogram. Currently supported:
 *    * ""   - Same as HIST below.
 *    * HIST - Draw just the histogram.
 * 	* E    - Draw error bars, show only the bars no markers or lines.
 * 	* E1   - Draw error bars with small lines at end and show markers.
 * \param[in] buf The streambuf that the output should be directed to. The default is directed to std::cout.
 */
void TikzPlot::PlotTH2(const TH2 *hist, const std::string &options,
		std::streambuf *buf /* = std::cout.rdbuf() */)
{
	bool surfPlot = false;
	if (options.find("surf") != std::string::npos) {
		surfPlot = true;
	}

	std::ostream output(buf);

	output <<
		"\t\\addplot3[";
	if (surfPlot) output << "surf,";
	else output <<	"matrix plot*, \%Similar to `surf`.";
	output << "\n"
			"\t\tshader = flat corner, \n"
			"\t\t\%Ordering of the coordinate data:\n"
			"\t\tmesh/cols=" << hist->GetNbinsX() << ", "
			"mesh/rows=" << hist->GetNbinsY() << ", "
			"mesh/ordering=rowwise]\n"
		"\t\tcoordinates {\n";

	for (int ybin=1; ybin<= hist->GetNbinsY(); ybin++) {
		output << "\t\t\t";
		double yvalue;
		if (surfPlot) yvalue = hist->GetYaxis()->GetBinLowEdge(ybin);
		else yvalue = hist->GetYaxis()->GetBinCenter(ybin);
		for (int xbin=1; xbin<= hist->GetNbinsX(); xbin++) {
			double xvalue;
			if (surfPlot) xvalue = hist->GetXaxis()->GetBinLowEdge(xbin);
			else xvalue = hist->GetXaxis()->GetBinCenter(xbin);
			double weight = hist->GetBinContent(xbin, ybin);
			output << "(" << xvalue << "," << yvalue << "," << weight << ") ";
		}
		output << "\n";
	}

	//Coordinate list trailer.
	output << "\t\t};\n";
}

/**TikZ is capable of using a previously rendered image and placing it within
 * axes that are rendered by TikZ. This can be useful in situations when
 * rendering a histogram with TikZ would be too time consuming, such as a 2D
 * histogram with many bins.
 *
 * \param[in] imageName The name of the output image to be used in TikZ plot.
 * 	Setting this to an empty string disables this feature.
 */
void TikzPlot::SetRootRender(const std::string &imageName) {
	imageName_ = imageName;
}
