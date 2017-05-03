/**Script to output a 1D TikZ histogram from a ROOT TH1
 *
 * @author Karl Smith
 * @date May 2, 2017
 */

#include <fstream>
#include <iostream>
#include <string>

#include <TH1.h>

/**Creates a TikZ picture using the content from the provided histogram. THe
 * options passed control the output style.
 *
 * \param[in] hist A pointer to the histogram to be converted to a TikZ object.
 * \param[in] outputFilename
 * \param[in] options Plotting options. Currently supported:
 *    * ""   - Same as HIST below.
 *    * HIST - Draw just the histogram.
 * 	* E    - Draw error bars, show only the bars no markers or lines.
 * 	* E1   - Draw error bars with small lines at end and show markers.
 */
void TikzTH1(TH1* hist, const std::string &outputFilename="", std::string options="") {
	bool includeErrors = false; //Include errors, shows only the error bars, no markers. ROOT option E.
	bool errorMarks = false; //Small lines are darwn at end of the error bars and markers are shown. ROOT option E1. 
	if (options.find("E") != std::string::npos) includeErrors = true;
	if (options.find("E1") != std::string::npos) errorMarks = true;
	std::streambuf *buf = std::cout.rdbuf();

	std::ofstream outputStream;
	if (outputFilename != "") {
		outputStream.open(outputFilename);
		buf = outputStream.rdbuf();
	}
	std::ostream output(buf);


	output << 
		"\\begin{tikzpicture}\n"
		"\t\\begin{axis}[\n"
			"\t\txlabel={" << hist->GetXaxis()->GetTitle() <<  "},\n"
			"\t\tylabel={" << hist->GetYaxis()->GetTitle() <<  "},\n"
			"\t\txmin=" << hist->GetXaxis()->GetXmin() << ", "
			"xmax=" << hist->GetXaxis()->GetXmax() << ",\n"
			"\t\tymin=" << hist->GetYaxis()->GetXmin() << ",";
	if (hist->GetYaxis()->GetXmax() != 1) 
			output << "ymax=" << hist->GetYaxis()->GetXmax() << " ";

	//Need this option to filter out points outside the x range.
//	output << "restrict x to domain*=
	output << "]\n";

	output << 
		"\t\\addplot+[";
	if (includeErrors) {
		if (!errorMarks) output << "scatter, mark=none, ";
		output << "only marks, error bars/.cd, y dir=both, y explicit, x dir=both, x explicit";
		if (!errorMarks) output << ", error mark = none";
	}
	else output << "const plot, no marks";
	output << "]\n"
		"\t\tcoordinates { ";

	if (!includeErrors) output << "(" << hist->GetBinLowEdge(1) << ",0) ";
	for (int xbin=1; xbin <= hist->GetNbinsX(); xbin++) {
		output << "(";
		if (includeErrors) output << hist->GetBinCenter(xbin);
		else output << hist->GetBinLowEdge(xbin);
		output << "," << hist->GetBinContent(xbin) << ") ";
		if (includeErrors) output << " +- (" << hist->GetBinWidth(xbin) / 2 <<
			"," << hist->GetBinError(xbin) << ") ";
	}
	if (!includeErrors) {
		output << "(" << hist->GetBinLowEdge(hist->GetNbinsX()) +
			hist->GetBinWidth(hist->GetNbinsX()) << "," << 0 << ") ";
	}
	output << "};\n";

	output <<
		"\t\\end{axis}\n"
		"\\end{tikzpicture}\n";

	if (outputStream.is_open()) {outputStream.close();}
}
