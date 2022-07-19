#ifndef PROGRAM_MARGINS_H
#define PROGRAM_MARGINS_H
#include "GlobalUsing.h"
#include <random>

/**
 * @brief Structure to encapsulate all the data about margin.
 * 
 * Contains author id, margin type and its contents.
 */
struct Margin {
	/**
	 * @brief Construct a new Margin object.
	 * 
	 * Default contructor.
	 */
	Margin() = default;

	/**
	 * @brief Construct a new Margin object.
	 * 
	 * Uses provided arguments when constructing new object.
	 * 
	 * @param t type of margin
	 * @param c contents of margin
	 * @param i id of the author
	 */
	Margin(const my_string& t, const my_string& c, std::size_t i) {
		type = t;
		content = c;
		id = i;
	}

	std::size_t id; //public identifier of the author
	my_string type; //type of margin, denotes what this margin is about
	my_string content; //contents of margin
};


#endif //PROGRAM_MARGINS_H
