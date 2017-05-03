/**Script to output a 2D TikZ pgfplot from a ROOT TH2.
 *
 * @author Karl Smith
 * @date May 1, 2017
 */

#include <iostream>
#include <fstream>
#include <string>

#include "TH2.h"

/**By default creates a TikZ matrix plot which is typically used to plot values
 * in a matrix, but can be easily adapted to a two-dimensional histogram. A
 * matrix plot requires that all bins are defined, and thus the option
 * "restrict z to domain*" is used so that empty bins are set to the minimum
 * value. If the option "surf" is provided a surface plot is created instead,
 * this type of plot requires data at the corners to be provided and thus plots
 * one less bin than is contained in the input histogram.
 *
 * @param[in] hist The ROOT histogram to be converted to TikZ output.
 * @param[in] outputFilename Optional filename of the TikZ output. If no name
 * 	is provided the output is directed to the standard output.
 * @param[in] options Options for the TikZ output.
 * 	* surf Create a surface plot instead of a matrix plot.
 */
void TikzTh2(TH2* hist, std::string outputFilename="", std::string options="") {
	bool surfPlot = false;
	if (options.find("surf") != std::string::npos) {
		surfPlot = true;
	}

	std::streambuf *buf = std::cout.rdbuf();

	std::ofstream outputStream;
	if (outputFilename != "") {
		outputStream.open(outputFilename);
		buf = outputStream.rdbuf();
	}
	std::ostream output(buf);

	output << 
		"\% Uses `matrix plot` which creates a filled patch at the middle\n"
		"\% of the coordinates provided. The `surf` type plot is similar,\n"
		"\% but expects the values of the corners to be provided. This   \n"
		"\% requires an extra row and column of values are provided. This\n"
		"\% would cause the plot to omit the final row and column of     \n"
		"\% bins. The `matrix plot` does not permit any holes in the     \n"
		"\% data, thus `restrict z to domain*` must be used which sets   \n"
		"\% the z value for a point that exceeds the domain to the limit \n"
		"\% it encountered.\n\n";
		
	output << 
		"\\begin{tikzpicture}\n"
		"\t\\pgfplotsset{compat=1.3} \%Compatible with v1.3 and newer.\n" 
		"\t\\begin{axis}[view={0}{90}, \%Top down view.\n"
			"\t\txlabel={" << hist->GetXaxis()->GetTitle() <<  "},\n"
			"\t\tylabel={" << hist->GetYaxis()->GetTitle() <<  "},\n"
			"\t\tcolorbar, colorbar style={\n"
			"\t\t\t\%To place the label on top of the colorbar use `title`.\n"
			"\t\t\tylabel={" << hist->GetZaxis()->GetTitle() << "}\n"
			"\t\t},\n"
			"\t\txmin=" << hist->GetXaxis()->GetXmin() << ", "
			"xmax=" << hist->GetXaxis()->GetXmax() << ",\n"
			"\t\tymin=" << hist->GetYaxis()->GetXmin() << ", "
			"ymax=" << hist->GetYaxis()->GetXmax() << ",\n"
			"\t\t\%Colorbar limits:\n"
			"\t\tpoint meta min=" << hist->GetMinimum() << ", "
			"point meta max=" << hist->GetMaximum() << ",\n"
			"\t\trestrict z to domain*=" <<
				hist->GetBinContent(hist->GetMinimumBin()) << ":" <<
				hist->GetBinContent(hist->GetMaximumBin()) << "]\n"
		"\t]\n\n";

	output <<
		"\t\\addplot3[";
	if (surfPlot) output << "surf,";
	else output <<	"matrix plot*, \%Similar to `surf`.";
	output << "\n" 
			"\t\tshader = flat corner, \n"
			"\t\t\%Ordering of the coordinate data:\n"
			"\t\tmesh/cols=" << hist->GetNbinsX() << ", "
			"mesh/rows=" << hist->GetNbinsY() << ", "
			"mesh/ordering=rowwise\n"
		"\t]\n"
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
	output << "\t\t};\n";
	
	output <<
		"\t\\end{axis}\n"
		"\\end{tikzpicture}\n";

	if (outputStream.is_open()) {outputStream.close();}
}
