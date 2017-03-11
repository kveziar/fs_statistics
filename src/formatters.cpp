#include "formatters.h"

QString fs_stat::formatSizeValue(std::size_t size) {
	double d_size = static_cast<double>(size);
	std::uint8_t i{ 0 };

	while ((i < 3) && (d_size >= 1024)) {
		d_size /= 1024;
		++i;
	}

	QString result;
	switch (i) {
		case 0: {
			return result.sprintf("%llu bytes", size);
		}
		case 1: {
			return result.sprintf("%.2lf Kb", d_size);
		}
		case 2: {
			return result.sprintf("%.2lf Mb", d_size);
		}
		default: {
			return result.sprintf("%.2lf Gb", d_size);
		}
	}
}
