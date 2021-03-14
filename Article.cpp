#include "Article.h"

Article::Article(std::string path) {
	path_to_article_file = path;
}

void Article::open_fstream() {
	if (!article_file.is_open()) {
		article_file = std::fstream(path_to_article_file);
		file = std::filesystem::directory_entry(std::filesystem::path(path_to_article_file));
	}
}

int get_size() {

}

std::uint64_t Article::get_length() {
	if (!article_file.is_open()) {
		Article::open_fstream();
		length = file.file_size();
	}
	return length;
}
