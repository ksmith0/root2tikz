#include "PgfPlotsGroupPlot.hpp"

#include <stdexcept>
#include <vector>

PgfPlotsGroupPlot::PgfPlotsGroupPlot(unsigned int rows,
												 unsigned int columns, std::string options)
	: PgfPlotsAxis(options), globalAxisLimits_({false})
{
	options_["group style"] = "{rows=" + std::to_string(rows)
									  + ", columns=" + std::to_string(columns) + "}";
	subPlots_.resize(rows * columns, nullptr);
}

/** Add a plot to the specified sub plot.
 *
 * \param[in] plot The plot to be added.
 * \param[in] plotId the sub plot id to which the plot should be added.
 */
void PgfPlotsGroupPlot::AddPlot(PgfPlotsPlot *plot, unsigned int plotId) {
	GetSubPlot(plotId)->AddPlot(plot);
}

/** Return the group plot arrangement, the number of rows anc columns.
 */
const std::pair< unsigned short, unsigned short > PgfPlotsGroupPlot::GetPlotDims() {
	//Get the group style options.
	std::string &groupStyleStr = options_["group style"];
	TikzOptions groupStyle(groupStyleStr.substr(1, groupStyleStr.length() - 2));
	const int rows = std::stoi(groupStyle["rows"]);
	const int columns = std::stoi(groupStyle["columns"]);

	return std::make_pair(rows, columns);
}

/** Get a pointer to the specified sub plot. This method will create the sub
 * plot object if it does not exist.
 *
 * \param[in] plotId the sub plot id to get the pointer for.
 * \return The sub plot pointer.
 */
PgfPlotsGroupSubPlot* PgfPlotsGroupPlot::GetSubPlot(unsigned int plotId) {
	if (plotId >= subPlots_.size())
		throw std::runtime_error("ERROR: Invalid plot id, " + std::to_string(plotId) + "!");

	PgfPlotsGroupSubPlot* &subPlot = subPlots_.at(plotId);

	//If there is no sub plot defined at that position we create it.
	if (! subPlot) subPlot = new PgfPlotsGroupSubPlot();

	return subPlot;
}

void PgfPlotsGroupPlot::PreprocessOptions() {
	ProcessGlobalLimits();
	ProcessAxisLabels();
	//Tick labels should be done after global limits.
	ProcessTickLabels();
}

void PgfPlotsGroupPlot::ProcessTickLabels() {
	auto dimensions = GetPlotDims();
	unsigned short &rows = dimensions.first;
	unsigned short &columns = dimensions.second;

	//Determine column and row labels
	std::vector< std::pair< std::string, std::string > > columnLimits(columns);
	std::vector< std::pair< std::string, std::string > > rowLimits(rows);
	bool identicalColumnLimits = true;
	bool identicalRowLimits = true;

	auto blankLimits = make_pair(std::string(), std::string());

	for (int row=0; row < rows; ++row) {
		for (int column=0; column < columns; ++column) {
			auto subPlot = subPlots_.at(columns * row + column);
			if (!subPlot) {
				if (row == rows - 1) {
					identicalColumnLimits = false;
				}
				if (column == 0) {
					identicalRowLimits = false;
				}
				continue;
			}

			auto plotOptions = subPlot->GetOptions();

			//Handle column labels (x-axis);
			const auto xlimits = std::make_pair(plotOptions->at("xmin"),
													 plotOptions->at("xmax"));
			if (columnLimits[column] == blankLimits) columnLimits[column] = xlimits;
			else if (columnLimits[column] != xlimits) identicalColumnLimits = false;

			//Handle row labels (y-axis);
			const auto ylimits = std::make_pair(plotOptions->at("ymin"),
													 plotOptions->at("ymax"));
			if (rowLimits[row] == blankLimits) rowLimits[row] = ylimits;
			else if (rowLimits[row] != ylimits) identicalRowLimits = false;
		}
	}
	//Set the global tick labels
	if (identicalColumnLimits)  {
		options_.Add("group/xticklabels at=edge bottom");
	}
	if (identicalRowLimits)  {
		options_.Add("group/yticklabels at=edge left");
	}
}

void PgfPlotsGroupPlot::ProcessAxisLabels() {
	auto dimensions = GetPlotDims();
	unsigned short &rows = dimensions.first;
	unsigned short &columns = dimensions.second;

	//Determine column and row labels
	std::vector<std::string> columnLabels(columns);
	std::vector<std::string> rowLabels(rows);
	bool identicalColumnLabels = true;
	bool identicalRowLabels = true;

	for (int row=0; row < rows; ++row) {
		for (int column=0; column < columns; ++column) {
			auto subPlot = subPlots_.at(columns * row + column);
			if (!subPlot) {
				if (row == rows - 1) {
					identicalColumnLabels = false;
				}
				if (column == 0) {
					identicalRowLabels = false;
				}
				continue;
			}

			//Handle column labels (x-axis);
			const auto &xlabel = subPlot->GetOptions()->at("xlabel");
			if (columnLabels[column] == "") columnLabels[column] = xlabel;
			else if (xlabel != "" && columnLabels[column] != xlabel)
				identicalColumnLabels = false;

			//Handle row labels (y-axis);
			const auto &ylabel = subPlot->GetOptions()->at("ylabel");
			if (rowLabels[row] == "") rowLabels[row] = ylabel;
			else if (ylabel != "" && rowLabels[row] != ylabel)
				identicalRowLabels = false;
		}
	}

	//Currently cannot handle different labels for different columns or rows,
	//but this could be corrected in the future.
	for (auto label : columnLabels) {
		if (label != "" && label != columnLabels[0]) identicalColumnLabels = false;
	}
	for (auto label : rowLabels) {
		if (label != "" && label != rowLabels[0]) identicalRowLabels = false;
	}

	//Set the global labels
	if (identicalColumnLabels)  {
		options_.Add("xlabel=" + columnLabels[0]);
		options_.Add("group/xlabels at=edge bottom");
	}
	if (identicalRowLabels) {
		options_.Add("ylabel=" + rowLabels[0]);
		options_.Add("group/ylabels at=edge left");
	}

	//Remove plot specific labels to avoid rendering for identical case.
	for (auto subPlot : subPlots_) {
		if (!subPlot) continue;
		auto plotOptions = subPlot->GetOptions();
		if (identicalColumnLabels) plotOptions->erase("xlabel");
		if (identicalRowLabels) plotOptions->erase("ylabel");
	}

	//Determine if spacing needs to be increased.
	if (!identicalColumnLabels) options_.Add("group/vertical sep=2cm");
	if (!identicalRowLabels) options_.Add("group/horizontal sep=2cm");
}


void PgfPlotsGroupPlot::ProcessGlobalLimits() {
	for (short axis=0; axis<3; axis++) {
		if (!globalAxisLimits_[axis]) continue;

		//Determine the option name
		std::string axisLetter;
		if (axis == 0) axisLetter = "x";
		else if (axis == 1) axisLetter = "y";
		else if (axis == 2) axisLetter = "z";
		std::string minOptName = axisLetter + "min";
		std::string maxOptName = axisLetter + "max";

		std::string	&globalMin = options_[minOptName];
		std::string	&globalMax = options_[maxOptName];

		bool computeLimits = false;
		//No global limits specified so we compute them.
		if (globalMin == "" && globalMax == "") computeLimits = true;

		for (auto subPlot : subPlots_) {
			if (!subPlot) continue;
			auto plotOptions = subPlot->GetOptions();

			if (computeLimits) {
				if (plotOptions->IsDefined(minOptName)) {
					std::string	&min = plotOptions->at(minOptName);

					if (globalMin == ""
							|| (min != "" && std::stod(min) < std::stod(globalMin))) {
						globalMin = min;
					}
				}
				if (plotOptions->IsDefined(maxOptName)) {
					std::string	&max = plotOptions->at(maxOptName);
					if (globalMax == ""
							|| (max != "" && std::stod(max) > std::stod(globalMax))) {
						globalMax = max;
					}
				}
			}

			//Remove subplot limits
			plotOptions->erase(minOptName);
			plotOptions->erase(maxOptName);
		}
	}
}
void PgfPlotsGroupPlot::SetGlobalAxisLimits(short axis, bool limitGlobally) {
	if (axis > 2) {
		throw std::runtime_error("Invalid axis, " + std::to_string(axis) + "!");
	}

	//Handle the negative case recursively.
	if (axis < 0) {
		for (int axis=0; axis < 3; axis++) {
			SetGlobalAxisLimits(axis, limitGlobally);
		}
		return;
	}

	globalAxisLimits_[axis] = limitGlobally;
}

/** Write the registered plots to the specified buffer.
 *
 * \param[in] buf The buffer that the plot should be written into.
 */
void PgfPlotsGroupPlot::WriteRegisteredItems(std::streambuf *buf) {
	for (auto subPlot : subPlots_) {
		if (subPlot) subPlot->Write(buf);
		else WriteSubPlotPlaceHolder(buf);
	}
}

void PgfPlotsGroupPlot::WriteSubPlotPlaceHolder(std::streambuf *buf) {
	std::ostream output(buf);
	output << "\t\t\\nextgroupplot[group/empty plot]\n";
}

