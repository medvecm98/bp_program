#include <iostream>
#include "Article.h"

int main() {
	Article article = Article("/home/michal/bruh.txt");
	std::cout << article.get_length() << std::endl;
	return 0;
}
