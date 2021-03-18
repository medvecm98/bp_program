//
// Created by michal on 17. 3. 2021.
//

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>

#ifndef PROGRAM_STRINGSPLITTER_H
#define PROGRAM_STRINGSPLITTER_H

class StringHelpers {
public:
	static std::vector<std::string> Split(const std::string& f, char delimiter, bool ignore_empty = false) {
		auto ss = std::stringstream(f);
		return Split(ss, delimiter, ignore_empty);
	}

	static std::vector<std::string> Split(std::istream& f, char delimiter, bool ignore_empty = false) {
		auto rv = std::vector<std::string>();

		std::string buffer = "";
		char cbuf = '~';

		while (f.get(cbuf)) {
			if (cbuf == delimiter) {
				if (ignore_empty && buffer.length() == 0) {}
				else {
					rv.push_back(buffer);
					buffer.clear();
				}
			}
			else {
				buffer.push_back(cbuf);
			}
		}

		if (!buffer.empty()) {
			rv.push_back(buffer);
		}

		return rv;
	}

	static std::string RemoveWhitespace(const std::string& source) {
		std::string buffer;
		for (auto&& c : source) {
			if (!std::isspace(c)) {
				buffer.push_back(c);
			}
		}
		return buffer;
	}

	static std::string Trim(const std::string& source) {
		std::string buffer;

		int fit = 0;
		int rit = source.length() - 1;

		while (fit < rit && source[fit] == ' ') {
			fit++;
		}

		if (fit == rit) {
			return "";
		}

		while (rit > 0 && source[rit] == ' ') {
			rit--;
		}

		do {
			buffer.push_back(source[fit]);
			fit++;
		} while (fit <= rit);

		return buffer;
	}
};

#endif //PROGRAM_STRINGSPLITTER_H
