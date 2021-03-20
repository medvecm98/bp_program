#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>
#include "GlobalUsing.h"

#ifndef PROGRAM_STRINGSPLITTER_H
#define PROGRAM_STRINGSPLITTER_H

class StringHelpers {
public:
	/** \brief Splits string with given delimiter.
	 *
	 * May filter out empty entries in resulting vector.
	 * @param f String to split
	 * @param delimiter Delimiter used to do split
	 * @param ignore_empty If empty entries should end up in resulting vector
	 * @return Vector of split string.
	 */
	static std::vector<my_string> Split(const my_string& f, char delimiter,
									 int64_t where_begin = 0, bool ignore_empty = false) {
		auto ss = std::stringstream(f);
		return Split(ss, delimiter, where_begin, ignore_empty);
	}

	/** \brief Splits stream with given delimiter.
	 *
	 * May filter out empty entries in resulting vector.
	 * @param f `istream` to read from
	 * @param delimiter Delimiter used to do split
	 * @param ignore_empty If empty entries should end up in resulting vector
	 * @return Vector of split stream.
	 */
	static std::vector<my_string> Split(std::istream& f, char delimiter,
									 int64_t where_begin = 0, bool ignore_empty = false) {
		//TODO: utilize `where_begin`
		auto rv = std::vector<my_string>();

		my_string buffer = "";
		char cbuf = '~';

		while (f.get(cbuf)) {
			if (cbuf == delimiter) {
				if (ignore_empty && buffer.empty()) {}
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

	/** \brief Removes any whitespace.
	 *
	 * **Any** whitespace from provided string is removed. Uses `std::isspace()`.
	 *
	 * @param source String to remove whitespaces from.
	 * @return String with no whitespaces.
	 */
	static my_string RemoveWhitespace(const my_string& source) {
		my_string buffer;
		for (auto&& c : source) {
			if (!std::isspace(c)) {
				buffer.push_back(c);
			}
		}
		return buffer;
	}

	/** \brief Trims whitespace characters.
	 *
	 * Trims whitespace characters (using `std::isspace()`) from beginning and end.
	 * @param source String to trim.
	 * @return Trimmed string.
	 */
	static my_string Trim(const my_string& source) {
		my_string buffer;

		if (!std::isspace(*source.begin()) && !std::isspace(*source.rbegin())) {
			return source;
		}

		int fit = 0;
		int rit = source.length() - 1;

		while (fit < rit && std::isspace(source[fit])) {
			fit++;
		}

		if (fit == rit) {
			return "";
		}

		while (rit > 0 && std::isspace(source[rit])) {
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
