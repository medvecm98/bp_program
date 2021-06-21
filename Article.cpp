#include "Article.h"

Article::Article(const my_string& path_file) {
	_path_to_article_file = path_file;
}

/**
 * \brief Used to initialize a new article, from "scratch".
 *
 * @tparam T Container of categories.
 * @param path_header
 * @param author_name
 * @param author_id
 * @param categories
 */
template <typename T>
void Article::initialize_article(const my_string & path_header, const my_string & author_name, std::size_t author_id,
								 const T &categories) {
	std::fstream header_file = std::fstream(path_header);

	for (auto&& i : categories) {
		_categories.insert(i);
	}

	_author_name = author_name;
	_author_id = author_id;
	_heading = "";
	calculate_hashes(_hashes);
}

void Article::calculate_hashes(hashes_container& hashes) {
	std::fstream article_file(_path_to_article_file);

	my_string line, paragraph;
	int h_counter = 0;
	std::regex r("[ ]*#[ ]+");
	while (std::getline(article_file, line)) {
		if (_heading.empty()) {
			std::smatch m;
			std::regex_search(line, m, r);
			while (!m.ready()) {}
			if (!m.empty()) {
				_heading = StringHelpers::Trim(line.substr(m.position(1) + m[1].length()));
			}
		}

		if (line.empty() && !paragraph.empty()) {
			auto new_hash = hashes.insert(hashes_container::value_type(h_counter++, HashWrapper(255, std::hash<std::string>{}(paragraph))));
			_length += paragraph.length();
			_main_hash += new_hash.first->second.hash;
		}
		else if (!line.empty()) {
			paragraph += StringHelpers::Trim(line) + ' ';
		}
	}
}

void Article::load_information() {

}

my_string Article::get_path_to_file() {
	return _path_to_article_file;
}

std::uint64_t Article::get_length() {
	return _length;
}

bool Article::is_in_category(const std::string& category) const {
	if (_categories.find(category) != _categories.end())
		return true;

	return false;
}

void Article::open_fstream(std::fstream& stream) {
	stream.open(_path_to_article_file);
}

category_container_const_iter Article::get_categories() {
	return _categories.cbegin();
}
