#include "Article.h"

void set_dir_path_and_root(QString& dir_path, QString& root_path) {
#ifdef _WIN64
	dir_path = "C:\\Program Files (x86)\\NewsP2PSharing\\Articles";
	root_path = "C:\\";
#elif defined(_WIN32)
	dir_path = "C:\\Program Files\\NewsP2PSharing\\Articles";
	root_path = "C:\\";
#else
	if (const char* env_p = std::getenv("XDG_DATA_HOME")) {
		dir_path = QString(env_p).append("/news_p2p_sharing/Articles/");
	}
	else if (const char* env_p = std::getenv("HOME")) {
		dir_path = QString(env_p).append("/.local/share/news_p2p_sharing/Articles/");
	}
	root_path = "/";
#endif
}

/**
 * @brief Construct a new Article::Article object from protobuf Article.
 * 
 * Used when saving received article as a conversion between format used for transfer and one used for storage.
 * 
 * @param protobuf_article Protobuf Article to convert from.
 */
Article::Article(const np2ps::Article& protobuf_article, const std::string& article_actual) :
	_author_name(protobuf_article.author_name()),
	_author_id(protobuf_article.author_id()),
	_news_name(protobuf_article.news_name()),
	_news_id(protobuf_article.news_id()),
	_main_hash(protobuf_article.main_hash()),
	_heading(protobuf_article.heading()),
	article_present_(false)
{
	//load categories
	if (!protobuf_article.categories().empty()) {
		for(auto it = protobuf_article.categories().begin(); it != protobuf_article.categories().end(); it++) {
			_categories.insert(*it);	
		}
	}

	//load paragraph hashes
	for (auto it = protobuf_article.paragraph_hashes().begin(); it != protobuf_article.paragraph_hashes().end(); it++) {
		_hashes.insert(std::make_pair<int32_t, HashWrapper>((int32_t)it->first, HashWrapper(it->second.hash(), it->second.level())));
	}

	//load margins
	for (auto it = protobuf_article.margins().begin(); it != protobuf_article.margins().end(); it++) {
		for (auto jt = it->second.margins().begin(); jt != it->second.margins().end(); jt++) {
			_margins.insert(std::make_pair<pk_t, Margin>((pk_t)it->first, Margin(jt->type(), jt->content(), jt->id())));
		}
	}

	_notes = "";

	if (!article_actual.empty()) 
	{
		article_present_ = true;
		QString file_name = QString::fromStdString(_heading).replace(' ', '_').append('-').append(QString::number(_main_hash)).append(".md");
		QFile file;

		QString dir_path, root_path;

		set_dir_path_and_root(dir_path, root_path);

		QDir dir(dir_path);
		QDir rdir(root_path);
		if (!dir.exists()) {
			rdir.mkpath(dir.path());
		}

		file_name.prepend(dir_path);

		file.setFileName(file_name);
		file.open(QIODevice::ReadWrite);
		QTextStream qts(&file);
		qts << QString::fromStdString(article_actual);
		file.close();
		_path_to_article_file = file_name.toStdString();
	} 

}

Article::Article(const np2ps::Article& protobuf_article) : Article(protobuf_article, ""){}

/**
 * Calculated hashes for various paragraphs. Will also fill in heading_ member.
 * @param hashes Where to put hashes.
 */
void Article::calculate_hashes(hashes_container& hashes) {
	std::fstream article_file(_path_to_article_file);
	std::string first_line;
	if (article_file.is_open())
		std::cout << "I'm open" << std::endl;

	my_string line, paragraph;
	int h_counter = 0;
	std::regex r("[ ]*#[ ]+"); //heading (in markdown) regex
	while (std::getline(article_file, line)) {
		if (first_line.empty() && !line.empty()) {
			first_line = line;
		}
		if (_heading.empty()) {
			std::smatch m;
			std::regex_search(line, m, r);
			while (!m.ready()) {}
			if (!m.empty()) {
				//_heading = StringHelpers::Trim(line.substr(m.position(1) + m[1].length()));
				_heading = line;
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
	if (line.empty() && !paragraph.empty()) {
		auto new_hash = hashes.insert(hashes_container::value_type(h_counter++, HashWrapper(std::hash<std::string>{}(paragraph), 0)));
		_length += paragraph.length();
		_main_hash += new_hash.first->second.hash;
	}
	if (_heading.empty())
		_heading = first_line;
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
