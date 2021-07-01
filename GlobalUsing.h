#ifndef PROGRAM_GLOBALUSING_H
#define PROGRAM_GLOBALUSING_H

#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <optional>
#include <variant>

using pk_t = std::size_t;

using my_string = std::string;
using hash_t = std::size_t;
using level_t = std::uint32_t;
using user_container = std::unordered_set<pk_t>;
using user_container_citer = user_container::const_iterator;
using user_level_map = std::unordered_map<pk_t, level_t>;
using user_level_map_citer = user_level_map::const_iterator;

using optional_my_string = std::optional<my_string>;

using user_variant = std::variant<user_container_citer, user_level_map_citer>;

#endif //PROGRAM_GLOBALUSING_H
