cmake_minimum_required(VERSION 3.17)
project(news_p2p_sharing)

set(CMAKE_CXX_STANDARD 17)

add_executable(news_p2p_sharing main.cpp Article.cpp Article.h CategoryEnums.h StringSplitter.h GlobalUsing.h ArticleDatabase.cpp ArticleDatabase.h Peer.cpp Peer.h IpWrapper.h IpMap.cpp IpMap.h NewspaperEntry.cpp NewspaperEntry.h Margins.h Message.cpp Message.h Networking.cpp Networking.h)