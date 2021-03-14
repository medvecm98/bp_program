#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

#ifndef PROGRAM_ARTICLE_H
#define PROGRAM_ARTICLE_H

enum article_format {
	PlainText = 0,
	Markdown = 1,
	OrgMode = 2,
	Tex = 3,
	Html = 4
};

using article_category = std::uint8_t;

class Article {
public:
	Article(std::string);
	void open_fstream();
	std::uint64_t get_length();
	article_format get_format();
	article_category get_category();
	std::vector<article_category> get_categories();
	std::vector<article_category> get_all_categories();
	std::string get_path_to_file();
	std::ifstream get_ifstream_file();
	std::ofstream get_ofstream_file();

private:
	std::uint64_t length;
	article_format format;
	article_category category;
	std::vector<article_category> categories;
	std::string path_to_article_file;
	std::fstream article_file;
	std::filesystem::directory_entry file;
};


#endif //PROGRAM_ARTICLE_H
