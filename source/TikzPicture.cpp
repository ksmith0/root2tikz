#include "TikzPicture.hpp"

#include <fstream>

#include "PgfPlotsGroupPlot.hpp"

TikzPicture::TikzPicture(const std::string &options, const std::string &preamble)
	: options_(options), preamble_(preamble), colorbrewer2_palette_("")
{

}

void TikzPicture::Add(PgfPlotsAxis* obj) {
	axes_.push_back(obj);

	if (dynamic_cast<PgfPlotsGroupPlot*>(obj)) {
		preamble_.append("\n\\usepgfplotslibrary{groupplots}");
	}
}

/** Makes use of the colorbrewer package which uses the palette on
 *  http://colorbrewer2.org
 *
 * \param[in] colorbrewer2_palette Name of the palette to be used.
 */
void TikzPicture::SetPalette(std::string colorbrewer2_palette) {
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

/**Creates a TikZ picture using with the provided axis.
 *
 * \param[in] filename
 */
void TikzPicture::Write(const std::string &filename /* = "" */) {
	std::streambuf *buf = std::cout.rdbuf();

	std::ofstream outputStream;
	if (filename != "") {
		outputStream.open(filename);
		buf = outputStream.rdbuf();
	}
	std::ostream output(buf);

	if (preamble_ != "") {
		output << preamble_ << "\n";
	}

	if (colorbrewer2_palette_ != "") {
		output << "\\usepgfplotslibrary{colorbrewer}\n"
			"\\pgfplotsset{cycle list/" << colorbrewer2_palette_ << "}\n";
	}

	output << "\\begin{tikzpicture}\n";

	for (auto axis : axes_) {
		axis->Write(buf);
	}

	output << "\\end{tikzpicture}\n";

	if (outputStream.is_open()) {outputStream.close();}
}
