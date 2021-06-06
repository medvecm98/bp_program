cmake_minimum_required(VERSION 3.17)
project(news_p2p_sharing)

set(CMAKE_CXX_STANDARD 17)

include(FindProtobuf)
find_package(Protobuf REQUIRED)

add_subdirectory(protobuf_source)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_executable(news_p2p_sharing main.cpp Article.cpp Article.h CategoryEnums.h StringSplitter.h GlobalUsing.h ArticleDatabase.cpp ArticleDatabase.h Peer.cpp Peer.h IpWrapper.h IpMap.cpp IpMap.h NewspaperEntry.cpp NewspaperEntry.h Margins.h Message.cpp Message.h Networking.cpp Networking.h)
target_link_libraries(news_p2p_sharing proto_msg)
target_link_libraries(news_p2p_sharing proto_art)