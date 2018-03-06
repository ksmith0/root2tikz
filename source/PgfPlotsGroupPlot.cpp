#include "PgfPlotsGroupPlot.hpp"

#include <stdexcept>

PgfPlotsGroupPlot::PgfPlotsGroupPlot(unsigned int rows, unsigned int columns, std::string options)
	: PgfPlotsAxis(options)
{
	options_["group style"] = "{rows=" + std::to_string(rows) + ", columns=" + std::to_string(columns) + "}";
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

/** Get a pointer to the specified sub plot. This method witll create teh sub
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
	int rows = std::stoi(groupStyle["rows"]);
	int columns = std::stoi(groupStyle["columns"]);

	//Identify if labels are all identical
	bool identicalColumnLabels = true;
	bool identicalRowLabels = true;
	for (int column=0; column < columns; ++column) {
		std::string columnLabel = GetSubPlot(columns * rows - columns + column)->GetOptions()->at("xlabel");
		for (int row=0; row < rows; ++row) {
			std::string rowLabel = GetSubPlot(columns * row)->GetOptions()->at("ylabel");
			auto plotOptions = GetSubPlot(column * rows + row)->GetOptions();
			if (identicalColumnLabels && plotOptions->at("xlabel") != columnLabel)
			  identicalColumnLabels = false;
			if (identicalRowLabels && plotOptions->at("ylabel") != rowLabel)
			  identicalRowLabels = false;
		}
	}
	if (identicalColumnLabels)  {
		options_.Add("xlabel=" + GetSubPlot(0)->GetOptions()->at("xlabel"));
		options_.Add("group/xlabels at=edge bottom");
	}
	if (identicalRowLabels) {
		options_.Add("ylabel=" + GetSubPlot(0)->GetOptions()->at("ylabel"));
		options_.Add("group/ylabels at=edge left");
	}
	//Remove plot specific labels to avoid rendering for identical case.
	for (int column=0; column < columns; ++column) {
		for (int row=0; row < rows; ++row) {
			auto plotOptions = GetSubPlot(column * rows + row)->GetOptions();
			if (identicalColumnLabels) plotOptions->erase("xlabel");
			if (identicalRowLabels) plotOptions->erase("ylabel");
		}
	}

	//Determine if spacing needs to be increased.
	for (int column=1; column < columns && !options_.IsDefined("group/horizontal sep"); ++column) {
		for (int row=0; row < rows; ++row) {
			auto plotOptions = GetSubPlot(column * rows + row)->GetOptions();
			if (plotOptions->IsDefined("ylabel")) {
				options_.Add("group/horizontal sep=2cm");
				break;
			}
		}
	}
	for (int row=1; row < rows && !options_.IsDefined("group/vertical sep"); ++row) {
		for (int column=0; column < columns; ++column) {
			auto plotOptions = GetSubPlot(column * rows + row)->GetOptions();
			if (plotOptions->IsDefined("xlabel")) {
				options_.Add("group/vertical sep=2cm");
				break;
			}
		}
	}
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

