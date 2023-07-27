#include <iostream>
#include <memory>
#include <set>
#include <map>
#include <string>
#include <filesystem>
#include "csv.hpp"

void tlog(unsigned first, unsigned second) {
	std::cout <<  first << '\t' << second << '\n';
}

int main() {
	const std::string scfile{R"(C:\Users\jilig\Downloads\Suzume.2022.1080p.WEB.Rip.5.1.YTS.M.X\Suzume.2022.1080p.WEBRip.x264.AAC5.1-YTS.MX.origin-Scenes-contant.csv)"};
	std::filesystem::path scfilepath{scfile};
	const std::filesystem::directory_entry scfileEntry{scfilepath};

	csv::CSVReader reader(scfilepath.generic_string());

	for (csv::CSVRow& row : reader) {
		unsigned target{ row["End Frame"].get<unsigned>() - 1 };
		if (target % 2 == 0) {
			unsigned _target{ static_cast<unsigned>(target * 2.5f) };
			tlog(_target, target);
			tlog(_target + 1, target);
			tlog(_target + 2, target + 1);
		}
		else {
			unsigned _target{ static_cast<unsigned>((target + 1) * 2.5f) };
			tlog(_target, target + 1);
			tlog(_target - 1, target + 1);
			tlog(_target - 2, target);
		}
	}
}