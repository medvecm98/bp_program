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
template<class Peer_t, class NewspaperEntry_t>
void Article::initialize_article(const category_container &categories, const std::string& file_path, 
	const Peer_t& me, const NewspaperEntry_t& news_entry )
{

	_path_to_article_file = file_path;

	for (auto&& i : categories) {
		_categories.insert(i);
	}

	_news_name = news_entry.get_name();
	_news_id = news_entry.get_id();
	_author_name = me.get_name();
	_author_id = me.get_public_key();
	_heading = "";

	/* main hash, hashes, length and heading are calculated and found here: */
	calculate_hashes(_hashes);

	if (!categories.empty()) {
		for (auto&& cat : categories) {
			_categories.insert(cat);
		}
	}


}

/**
 * Calculated hashes for various paragraphs. Will also fill in heading_ member.
 * @param hashes Where to put hashes.
 */
void Article::calculate_hashes(hashes_container& hashes) {
	std::fstream article_file(_path_to_article_file);

	my_string line, paragraph;
	int h_counter = 0;
	std::regex r("[ ]*#[ ]+"); //heading (in markdown) regex
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
			auto new_hash = hashes.insert(hashes_container::value_type(h_counter++, HashWrapper(std::hash<std::string>{}(paragraph), 0)));
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

void Article::select_level(my_string& rv, level_t level) {
	std::stringstream rv_stream;
	ParagraphIterator pi(_path_to_article_file, level, _hashes.begin());
	++pi;
	while (pi.get().has_value()) {
		rv_stream << (pi.get().value());
		rv_stream << std::endl;
		++pi;
	}
	rv = std::move(rv_stream.str());
}