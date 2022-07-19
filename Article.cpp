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

	switch (protobuf_article.type())
	{
	case 0:
		_format = article_format::PlainText;
		break;
	case 1:
		_format = article_format::Markdown;
		break;
	case 4:
		_format = article_format::Html;
		break;
	
	default:
		break;
	}

	if (!article_actual.empty()) //we want to set the path, if there is article downloaded locally
	{
		set_path(article_actual);
		calculate_crypto_hash();
	}
	else {
		article_present_ = false;
		crypto_hash_ = protobuf_article.crypto_hash();
	}

	

}

/**
 * @brief Construct a new Article:: Article object. Constructor is used for header-only articles.
 * 
 * @param protobuf_article Protobuf message to create Article from.
 */
Article::Article(const np2ps::Article& protobuf_article) : Article(protobuf_article, ""){}

/**
 * @brief Forms path and creates a file for article contents.
 * 
 * If formed path doesn't exist, it will be created.
 * 
 * @param article_actual Article contents.
 */
void Article::set_path(const std::string& article_actual) {
	article_present_ = true;

		/* name of the file, without path, with spaces replaced with underscores and with article hash appended */
		QString file_name = QString::fromStdString(_heading).replace(' ', '_').append('-').append(QString::number(_main_hash));

		switch (_format) //set correct file suffix
		{
		case article_format::PlainText:
			file_name.append(".txt");
			break;
		case article_format::Markdown:
			file_name.append(".md");
			break;
		case article_format::Html:
			file_name.append(".html");
			break;
		
		default:
			file_name.append(".txt"); //`.txt` is the default one
			break;
		}

		QFile file;

		QString dir_path, root_path;

		/* sets correct path, some support for MS Windows is provided */
		set_dir_path_and_root(dir_path, root_path);

		QDir dir(dir_path); //directory, where we want to place our article
		QDir rdir(root_path); //root of path of dir variable
		if (!dir.exists()) {
			rdir.mkpath(dir.path()); //create the directory if it doesn't exist
		}

		file_name.prepend(dir_path); //form full path together with file name

		file.setFileName(file_name);
		file.open(QIODevice::ReadWrite);
		QTextStream qts(&file);
		qts << QString::fromStdString(article_actual); //write article's contents into the file
		file.close();
		_path_to_article_file = file_name.toStdString();
}

/**
 * Will fill in heading_ member.
 */
void Article::calculate_hashes() {
	std::fstream article_file(_path_to_article_file);
	std::string line;
	std::stringstream article_builder;

	if (article_file.is_open())
		std::cout << "Opened article file for " << _path_to_article_file << std::endl;

	if (std::getline(article_file, line)) { //read first line for the heading
		article_builder << line; 
		QString qline = QString::fromStdString(line);
		if (qline.contains('#')) { //sets the heading...
			auto i = qline.indexOf('#');
			_heading = qline.mid(i + 1).toStdString(); //...for markdown, where it needs to cut the trailing `#`
		}
		else {
			_heading = line; //...for plaintext
		}
	}

	while (std::getline(article_file, line)) {
		article_builder << line;
	}
}

/**
 * @brief Getter for `_path_to_article_file`. 
 * 
 * @return my_string Path to article file.
 */
my_string Article::get_path_to_file() {
	return _path_to_article_file;
}

/**
 * @brief Checks if article belongs in a category
 * 
 * @param category Category to check for.
 * @return true Article belongs into the category.
 * @return false Article doesn't belong into the category.
 */
bool Article::is_in_category(const std::string& category) const {
	if (_categories.find(category) != _categories.end())
		return true;

	return false;
}

/**
 * @brief Opens file stream for article file.
 * 
 * @param stream Reference to stream, which will be opened with article file.
 */
void Article::open_fstream(std::fstream& stream) {
	stream.open(_path_to_article_file);
}

/**
 * @brief Gets the begin iterator for article categories container.
 * 
 * @return category_container_const_iter Begin iterator for categories container.
 */
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

void Article::calculate_crypto_hash() {
	if (article_present()) {
		CryptoPP::SHA3_256 hash;
		CryptoPP::FileSource(_path_to_article_file.c_str(), true, new CryptoPP::HashFilter(hash, new CryptoPP::StringSink(crypto_hash_)));
	}
}

bool Article::verify(const std::string& article_to_check) {
	bool result;
	CryptoPP::SHA3_256 hash;
	CryptoPP::StringSource(crypto_hash_ + article_to_check, true, 
		new CryptoPP::HashVerificationFilter(hash, 
			new CryptoPP::ArraySink((CryptoPP::byte*)&result, sizeof(result))));
	return result;
}

std::string Article::get_crypto_hash() {
	return crypto_hash_;
}