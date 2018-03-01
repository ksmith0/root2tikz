#include "PgfPlotsAxis.hpp"

PgfPlotsAxis::PgfPlotsAxis(const std::string &options)
	: options_(options), logMode_({false})
{

}

void PgfPlotsAxis::AddPlot(PgfPlotsPlot *plot) {
	plots_.push_back(plot);

	const TH1* hist = plot->GetHist();

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

/** Returns the options passed to a pgfplots axis for log mode.
 *
 *  \return log options for pgfplots axis.
 */
std::string PgfPlotsAxis::LogModeOptions() {
	std::string output = "";
	if (logMode_.at(0)) output.append("xmode=log, ");
	if (logMode_.at(1)) output.append("ymode=log, ");
	if (logMode_.at(2)) output.append("zmode=log, ");

	return output;
}

/** Set log mode for a given axis.
 *
 * \param[in] axis Axis to set log mode for: x=0, y=1, z=2.
 * \param[in] logMode Value of log mode, true for log plot.
 */
void PgfPlotsAxis::SetLog(const short &axis, const bool &logMode /* = true */) {
	if (axis >= 3) {
		std::cerr << "ERROR: TikzPlot::SetLog called for invalid axis index: " << axis << "!\n";
		return;
	}

	logMode_.at(axis) = logMode;
}

void PgfPlotsAxis::Write(std::streambuf *buf) {
	std::ostream output(buf);

	output << "\t\\begin{axis}[\n";

	auto logModeOpts = LogModeOptions();
	if (logModeOpts != "") {
		output << "\t\t\t" << logModeOpts << "\n";
	}

	output <<
		"\t\t\txlabel={" << GetLatexString(axisTitles_.at(0)) <<  "},\n"
		"\t\t\tylabel={" << GetLatexString(axisTitles_.at(1)) <<  "},\n"
		"\t\t\txmin=" << axisLimits_.at(0).first << ", "
		"xmax=" << axisLimits_.at(0).second << ",\n"
		"\t\t\tymin=" << axisLimits_.at(1).first << ", "
		"ymax=" << axisLimits_.at(1).second << ",\n";

	if (options_ != "") {
		output << "\t\t\t" << options_ << "\n";
	}

	output << "\t\t]\n\n";

	for (auto plot : plots_) {
		plot->Write(buf);
	}

	output <<
		"\t\\end{axis}\n";
}
