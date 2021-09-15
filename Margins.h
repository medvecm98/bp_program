#ifndef PROGRAM_MARGINS_H
#define PROGRAM_MARGINS_H
#include "GlobalUsing.h"
#include <random>

struct Margin {
	Margin() = default;
	Margin(const my_string& t, const my_string& c) {
		type = t;
		content = c;
		std::random_device rd("/dev/urandom");
		id = rd();
	}
	Margin(const my_string& t, const my_string& c, unsigned int i) {
		type = t;
		content = c;
		id = i;
	}
	unsigned int id;
	my_string type;
	my_string content;

	/**
	 * Serialize using boost archive.
	 */
	template <class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & id;
		ar & type;
		ar & content;
	}
};


#endif //PROGRAM_MARGINS_H
