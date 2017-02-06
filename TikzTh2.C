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

void TikzTh2(TH2* hist, std::string outputFilename="") {
	std::streambuf *buf = std::cout.rdbuf();

	std::ofstream outputStream;
	if (outputFilename != "") {
		outputStream.open(outputFilename);
		buf = outputStream.rdbuf();
	}
	std::ostream output(buf);

	output << 
		"\\begin{tikzpicture}\n"
		"\t\\begin{axis}[view={0}{90}, colorbar, \n"
			"\t\txmin=" << hist->GetXaxis()->GetXmin() << ", "
			"xmax=" << hist->GetXaxis()->GetXmax() << ",\n"
			"\t\tymin=" << hist->GetYaxis()->GetXmin() << ", "
			"ymax=" << hist->GetYaxis()->GetXmax() << ",\n"
			"\t\tpoint meta min=" << hist->GetMinimum() << ", "
			"point meta max=" << hist->GetMaximum() << ",\n"
			"\t\txlabel={" << hist->GetXaxis()->GetTitle() <<  "},\n"
			"\t\tylabel={" << hist->GetYaxis()->GetTitle() <<  "},\n"
			"\t\tzlabel={" << hist->GetZaxis()->GetTitle() <<  "}\n"
		"\t]\n";

	output <<
		"\t\\addplot3[surf, "
			"mesh/cols=" << hist->GetNbinsY() << ", "
			"mesh/rows=" << hist->GetNbinsX() << "]\n"
		"\t\tcoordinates {\n";

	for (int xbin=1; xbin<= hist->GetNbinsX(); xbin++) {
		output << "\t\t\t";
		double xvalue = hist->GetXaxis()->GetBinLowEdge(xbin);
		for (int ybin=1; ybin<= hist->GetNbinsY(); ybin++) {
			double yvalue = hist->GetYaxis()->GetBinLowEdge(ybin);
			double zvalue = hist->GetBinContent(xbin, ybin);
			output << "(" << xvalue << "," << yvalue << "," << zvalue << ") ";
		}
		output << "\n";
	}
	output << "\t\t};\n";
	
	output <<
		"\t\\end{axis}\n"
		"\\end{tikzpicture}\n";

	if (outputStream.is_open()) {outputStream.close();}
}
