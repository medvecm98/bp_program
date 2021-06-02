#include "Message.h"

template<typename T>
my_vector SerializeT(T input) {
	auto a = (uint8_t*) &input;
	my_vector rv;
	for (std::size_t i = 0; i < sizeof(T); i++) {
		rv.push_back(*(a + i));
	}
	return std::move(rv);
}

template<typename T>
void DeserializeT(T& output, my_vector& input) {
	auto a = (uint8_t*) &output;
	for (std::size_t i = sizeof(T); i > 0; i--) {
		*(a + i - 1) = input.back();
		input.pop_back();
	}
}

/**
 * \brief Serializes and moves contents of field into vector.
 *
 * @tparam T Source field type.
 * @param rv Vector to move into.
 * @param t Field to serialize.
 * @return Number of bytes of serialized vector.
 */
template <typename T>
std::size_t move_vector(my_vector& rv, T&& t) {
	auto temp = SerializeT(t);
	rv.insert(rv.end(), std::make_move_iterator(temp.begin()), std::make_move_iterator(temp.end()));
	return rv.size();
}

my_vector Message::Serialize() {
	my_vector rv;
	move_vector(rv, to_);
	move_vector(rv, from_);
	move_vector(rv, msg_context_);
	move_vector(rv, msg_type_);
	return std::move(rv);
}

bool Message::Deserialize(my_vector s) {
	DeserializeT(msg_type_, s);
	DeserializeT(msg_context_, s);
	DeserializeT(from_, s);
	DeserializeT(to_, s);
	return true;
}

my_vector ArticleHeaderMessage::Serialize() {
	std::size_t serialized_size = 0;
	my_vector rv = Message::Serialize();
	move_vector(rv, article_hash_);
	serialized_size = move_vector(rv, level_);
	if (msg_context_ == MessageContext::response) {
		serialized_size = move_vector(rv, article_);
	}

	move_vector(rv, serialized_size);
	return std::move(rv);
}

bool ArticleHeaderMessage::Deserialize(my_vector s) {

	std::size_t serialized_size = 0;
	DeserializeT(serialized_size, s); //gets message length

	if (msg_context_ == MessageContext::response)
		DeserializeT(article_, s);
	DeserializeT(level_, s);
	DeserializeT(article_hash_, s);
	Message::Deserialize(s);
	return true;
}