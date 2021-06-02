//
// Created by michal on 18. 3. 2021.
//

#include "gtest/gtest.h"
#include "StringSplitter.h"

TEST(TrimTest, Regular) {
	ASSERT_EQ(StringHelpers::Trim(" aha   "), "aha");
	ASSERT_EQ(StringHelpers::Trim(" aha"), "aha");
	ASSERT_EQ(StringHelpers::Trim("aha  "), "aha");

	ASSERT_EQ(StringHelpers::Trim("a ha"), "a ha");
	ASSERT_EQ(StringHelpers::Trim("   ah a    "), "ah a");
	ASSERT_EQ(StringHelpers::Trim("   a  ha  "), "a  ha");

	ASSERT_EQ(StringHelpers::Trim("   \t   \t\t\t\t\t\n\n\n\n\n\n   \t "), "");
}

TEST(SplitTest, Regular) {
	std::vector<std::string> sv = std::vector<std::string>();
	sv.push_back("hello");
	sv.push_back("there");
	sv.push_back("general");
	sv.push_back("kenobi");
	ASSERT_EQ(StringHelpers::Split("hello:there:general:kenobi", ':'), sv);
}

TEST(SplitTest, OneWord) {
	auto single_word_v = std::vector<std::string>();
	single_word_v.push_back("kapral");
	ASSERT_EQ(StringHelpers::Split("kapral:", ':'), single_word_v);
	ASSERT_EQ(StringHelpers::Split(":kapral", ':', 0, true), single_word_v);
}

TEST(RemoveWhiteSpace, Regular) {
	ASSERT_EQ(StringHelpers::RemoveWhitespace(std::string("A L O H A")), "ALOHA");
	ASSERT_EQ(StringHelpers::RemoveWhitespace(std::string("     A L O H A")), "ALOHA");
	ASSERT_EQ(StringHelpers::RemoveWhitespace(std::string("A L OH A       ")), "ALOHA");
	ASSERT_EQ(StringHelpers::RemoveWhitespace(std::string("ALO           H A")), "ALOHA");
	ASSERT_EQ(StringHelpers::RemoveWhitespace(std::string("             ALO       H A")), "ALOHA");
}