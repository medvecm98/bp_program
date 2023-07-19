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
	 */
	Margin(const my_string& t, const my_string& c) {
		type = t;
		content = c;

		std::random_device rd;
		std::mt19937_64 gen(rd());
    	std::uniform_int_distribution<pk_t> dist;
		id = dist(gen);
	}

	/**
	 * @brief Construct a new Margin object.
	 * 
	 * Uses provided arguments when constructing new object.
	 * 
	 * @param t type of margin
	 * @param c contents of margin
	 * @param i ID of the margin
	 */
	Margin(const my_string& t, const my_string& c, pk_t i) {
		type = t;
		content = c;

		id = i;
	}

	std::size_t id; //public identifier of the margin
	my_string type; //type of margin, denotes what this margin is about
	my_string content; //contents of margin
};


#endif //PROGRAM_MARGINS_H
