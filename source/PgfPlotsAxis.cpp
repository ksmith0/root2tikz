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

	auto graph = plot->GetGraph();
	auto hist1d = plot->GetHist1d();
	auto hist2d = plot->GetHist2d();

	const TAxis *xAxis, *yAxis, *zAxis = nullptr;
	if (hist1d) {
		xAxis = hist1d->GetXaxis();
		yAxis = hist1d->GetYaxis();
	}
	else if (hist2d) {
		xAxis = hist2d->GetXaxis();
		yAxis = hist2d->GetYaxis();
		zAxis = hist2d->GetZaxis();
	}
	else if (graph) {
		xAxis = graph->GetXaxis();
		yAxis = graph->GetYaxis();
	}

	if (options_.find("xlabel") == options_.end()) {
		options_["xlabel"] = "{" + GetLatexString(xAxis->GetTitle()) + "}";
	}
	if (options_.find("ylabel") == options_.end()) {
		options_["ylabel"] = "{" + GetLatexString(yAxis->GetTitle()) + "}";
	}
	//z-axis label.
	if (hist2d && options_.find("colorbar style") == options_.end()) {
		options_["view"] = "{0}{90}";
		options_["colorbar"] = "true";
		options_["colorbar style"] = "{ylabel={" + GetLatexString(zAxis->GetTitle()) + "}}";
	}

	//Get the x-axis limits.
	std::string &xmin = options_["xmin"];
	if (xmin == "" || std::stod(xmin) > xAxis->GetXmin()) {
		xmin = std::to_string(xAxis->GetXmin());
	}
	std::string &xmax = options_["xmax"];
	if (xmax == "" || std::stod(xmax) < xAxis->GetXmax()) {
		xmax = std::to_string(xAxis->GetXmax());
	}

	//Get the y-axis limits
	double plotYMin = yAxis->GetXmin();
	double plotYMax = yAxis->GetXmax();
	if (hist1d) {
		hist1d->GetMinimumAndMaximum(plotYMin, plotYMax);
	}
	if (hist1d || graph) {
		plotYMin *= 0.9;
		plotYMax *= 1.1;
	}

	std::string &ymin = options_["ymin"];
	if (ymin == "" || std::stod(ymin) > plotYMin) {
		ymin = std::to_string(plotYMin);
	}
	std::string &ymax = options_["ymax"];
	if (ymax == "" || std::stod(ymax) < plotYMax) {
		ymax = std::to_string(plotYMax);
	}

	//Get the z-axis limits
	if (zAxis) {
		double plotZMin = zAxis->GetXmin();
		double plotZMax = zAxis->GetXmax();
		if (hist2d) {
			hist2d->GetMinimumAndMaximum(plotZMin, plotZMax);
		}
		options_["point meta min"] = std::to_string(plotZMin);
		options_["point meta max"] = std::to_string(plotZMax);
		options_["restrict z to domain*"] = std::to_string(plotZMin) + ":"
		                                    + std::to_string(plotZMax);
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

