#include "PgfPlotsGroupPlot.hpp"

#include <stdexcept>
#include <vector>

PgfPlotsGroupPlot::PgfPlotsGroupPlot(unsigned int rows,
												 unsigned int columns, std::string options)
	: PgfPlotsAxis(options)
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
	//Get the group style options.
	std::string &groupStyleStr = options_["group style"];
	TikzOptions groupStyle(groupStyleStr.substr(1, groupStyleStr.length() - 2));
	const int rows = std::stoi(groupStyle["rows"]);
	const int columns = std::stoi(groupStyle["columns"]);

	//Determine column and row labels
	std::vector<std::string> columnLabels(columns);
	std::vector<std::string> rowLabels(rows);
	bool identicalColumnLabels = true;
	bool identicalRowLabels = true;

	for (int row=0; row < rows; ++row) {
		for (int column=0; column < columns; ++column) {
			auto subPlot = subPlots_.at(columns * row + column);
			if (!subPlot) continue;

			const auto &xlabel = subPlot->GetOptions()->at("xlabel");
			const auto &ylabel = subPlot->GetOptions()->at("ylabel");

			//Handle column labels (x-axis);
			if (columnLabels[column] == "") columnLabels[column] = xlabel;
			else if (xlabel != "" && columnLabels[column] != xlabel)
				identicalColumnLabels = false;

			//Handle row labels (y-axis);
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

/** Write the registered plots to the specified buffer.
 *
 * \param[in] buf The buffer that the plot should be written into.
 */
void PgfPlotsGroupPlot::WriteRegisteredItems(std::streambuf *buf) {
	for (auto subPlot : subPlots_) {
		if (subPlot) subPlot->Write(buf);
	}
}

