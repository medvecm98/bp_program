cmake_minimum_required(VERSION 3.17)
project(news_p2p_sharing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-pedantic -O0 -g)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


include(FindProtobuf)
find_package(Protobuf REQUIRED)
find_package(Boost COMPONENTS system serialization REQUIRED)
find_package(Qt5 COMPONENTS Core Network Widgets REQUIRED)
include_directories( ${Boost_INCLUDE_DIRS} )

find_library(CryptoPP cryptopp)

add_subdirectory(protobuf_source)
add_dependencies(proto_msg proto_art)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_executable(news_p2p_sharing main.cpp newarticleform.h newarticleform.cpp newarticleform.ui
	add_newspaper.h add_newspaper.cpp add_newspaper.ui form.h form.cpp form.ui
	programcontext.h programcontext.cpp mainwindow.cpp mainwindow.h mainwindow.ui Article.cpp Article.h CategoryEnums.h
	StringSplitter.h GlobalUsing.h ArticleDatabase.cpp ArticleDatabase.h Peer.cpp Peer.h IpWrapper.h IpMap.cpp IpMap.h
	NewspaperEntry.cpp NewspaperEntry.h Margins.h Message.cpp Message.h Networking.cpp Networking.h)
target_link_libraries(news_p2p_sharing stdc++fs proto_msg proto_art ${PROTOBUF_LIBRARIES} ${CryptoPP} ${Boost_LIBRARIES}
	Qt5::Widgets Qt5::Network Qt5::Core)
#target_link_libraries(news_p2p_sharing PUBLIC ${CryptoPP})
