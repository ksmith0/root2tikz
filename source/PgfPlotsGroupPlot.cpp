#include "PgfPlotsGroupPlot.hpp"

#include <stdexcept>

PgfPlotsGroupPlot::PgfPlotsGroupPlot(unsigned int rows, unsigned int columns, std::string options)
	: PgfPlotsAxis(options)
{
	options_["group style"] = "{group size=" + std::to_string(rows) + " by " + std::to_string(columns) + "}";
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
 * \param[in] plotId the sub plot id to get teh pointer for.
 * \return The sub plot pointer.
 */
PgfPlotsGroupSubPlot* PgfPlotsGroupPlot::GetSubPlot(unsigned int plotId) {
	if (plotId >= subPlots_.size())
		throw std::runtime_error("ERROR: Invalid plot id, " + std::to_string(plotId) + "!");

	PgfPlotsGroupSubPlot **subPlot = &subPlots_.at(plotId);

	//If there is no sub plot defined at that position we create it.
	if (! *subPlot) *subPlot = new PgfPlotsGroupSubPlot();

	return *subPlot;
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

