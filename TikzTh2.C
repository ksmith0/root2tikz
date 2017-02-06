/**Script to output a 2D TiKz pgfplot from a ROOT TH2.
 *
 * @author Karl Smith
 *
 *
 */

#include <iostream>
#include <fstream>
#include <string>

#include "TH2.h"

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
			"\t\trestrict z to domain*=" << hist->GetBinContent(hist->GetMinimumBin()) << ":" << hist->GetBinContent(hist->GetMaximumBin()) << "]\n"
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
