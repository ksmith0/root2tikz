#include "PgfPlotsLegend.hpp"

PgfPlotsLegend::PgfPlotsLegend() {

}

void PgfPlotsLegend::AddEntry(const std::string &entry) {
	legendEntries_.push_back(entry);
}

void PgfPlotsLegend::Write(std::streambuf *buf) {
	std::ostream output(buf);

	output << "\t\\legend{\n";

	for (auto entry : legendEntries_) {
		output << "\t\t{" << entry << "},\n";
	}

	output << "\t}\n";

}
