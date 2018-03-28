#include "PgfPlotsAxis.hpp"

PgfPlotsAxis::PgfPlotsAxis(const std::string &options)
	: legend_(nullptr)
{
	options_.Add(options);
}

void PgfPlotsAxis::SetLegend(PgfPlotsLegend *legend) {
	legend_ = legend;
}

/** Add a plot to this axis.
 *
 * \param[in] plot The plot to be added.
 */
void PgfPlotsAxis::AddPlot(PgfPlotsPlot *plot) {
	plots_.push_back(plot);

	const TH1* hist = plot->GetHist();

	if (options_.find("xlabel") == options_.end()) {
		options_["xlabel"] = "{" + GetLatexString(hist->GetXaxis()->GetTitle()) + "}";
	}
	if (options_.find("ylabel") == options_.end()) {
		options_["ylabel"] = "{" + GetLatexString(hist->GetYaxis()->GetTitle()) + "}";
	}

	//Get the axis limits.
	std::string &xmin = options_["xmin"];
	if (xmin == "" || std::stod(xmin) > hist->GetXaxis()->GetXmin()) {
		xmin = std::to_string(hist->GetXaxis()->GetXmin());
	}
	std::string &xmax = options_["xmax"];
	if (xmax == "" || std::stod(xmax) < hist->GetXaxis()->GetXmax()) {
		xmax = std::to_string(hist->GetXaxis()->GetXmax());
	}

	double histYMin, histYMax;
	hist->GetMinimumAndMaximum(histYMin, histYMax);
	std::string &ymin = options_["ymin"];
	if (ymin == "" || std::stod(ymin) > 0.9 * histYMin) {
		ymin = std::to_string(0.9 * histYMin);
	}
	std::string &ymax = options_["ymax"];
	if (ymax == "" || std::stod(ymax) < 1.1 * histYMax) {
		ymax = std::to_string(1.1 * histYMax);
	}
}

/** Set log mode for a given axis.
 *
 * \param[in] axis Axis to set log mode for: x=0, y=1, z=2.
 * \param[in] logMode Value of log mode, true for log plot.
 */
void PgfPlotsAxis::SetLog(const short &axis, const bool &logMode /* = true */) {
	std::string optionName;
	switch (axis) {
		case 0:
			optionName = "xmode";
			break;
		case 1:
			optionName = "ymode";
			break;
		case 2:
			optionName = "zmode";
			break;
		default:
			std::cerr << "ERROR: TikzPlot::SetLog called for invalid axis index: " << axis << "!\n";
			return;
	}

	if (logMode) {
		options_[optionName] = "log";
	} else {
		options_.erase(optionName);
	}
}

/** Write the axis and its registered plots to the specified buffer.
 *
 * \param[in] buf The buffer that the plot should be written into.
 */
void PgfPlotsAxis::Write(std::streambuf *buf) {
	PreprocessOptions();

	std::ostream output(buf);

	output << "\t" << EnvHeader() << "[\n";

	for (auto option : options_) {
		output << "\t\t\t" << option.first << "=" << option.second << ",\n";
	}

	// Write any legend options for the specified legend
	if (legend_) {
		auto legendStyle = legend_->GetStyle();
		if (!legendStyle->empty()) {
			output << "\t\t\tlegend style={\n";
			for (auto option : *legendStyle) {
				output << "\t\t\t\t" << option.first << "=" << option.second << ",\n";
			}
			output << "\t\t\t}\n";
		}
	}

	output << "\t\t]\n\n";

	// Write the optional legend.
	if (legend_) legend_->Write(buf);

	WriteRegisteredItems(buf);

	output << "\t" << EnvFooter() << "\n";
}

/** Write the registered plots to the specified buffer.
 *
 * \param[in] buf The buffer that the plot should be written into.
 */
void PgfPlotsAxis::WriteRegisteredItems(std::streambuf *buf) {
	for (auto plot : plots_) {
		plot->Write(buf);
	}
}


/** Convert ROOT TLatex syntax into proper LaTeX syntax.
 *
 * \param[in] A ROOT TLatex string to be converted.
 * \return A LaTeX formatted string.
 */
std::string PgfPlotsAxis::GetLatexString(std::string str) {
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

