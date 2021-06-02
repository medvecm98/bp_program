#ifndef PROGRAM_NEWSPAPERENTRY_H
#define PROGRAM_NEWSPAPERENTRY_H

#include "GlobalUsing.h"
#include "Article.h"

/**
 * IDs saved as keys.
 */
class NewspaperEntry {
private:
	article_database_container _articles; //downloaded articles by these newspaper
	user_container _authorities; //authorities of these newspaper
};

#endif //PROGRAM_NEWSPAPERENTRY_H
