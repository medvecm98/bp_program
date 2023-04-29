cmake_minimum_required(VERSION 3.17)
project(news_p2p_sharing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# add_compile_options(-pedantic -O0 -g)
# set(CMAKE_CXX_CLANG_TIDY
# 	clang-tidy;
# 	-header-filter=${CMAKE_CURRENT_SOURCE_DIR};
# )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

find_package(cryptopp CONFIG REQUIRED)

find_package(Qt5 COMPONENTS Core Network Widgets REQUIRED)
#find_package(Doxygen REQUIRED dot)

add_subdirectory(protobuf_source)
add_dependencies(proto_msg proto_art)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_executable(news_p2p_sharing main.cpp newarticleform.h newarticleform.cpp newarticleform.ui add_newspaper.h 
	add_newspaper.cpp add_newspaper.ui form.h form.cpp form.ui categoriesform.h categoriesform.cpp categoriesform.ui
	programcontext.h programcontext.cpp mainwindow.cpp
	mainwindow.h mainwindow.ui Article.cpp Article.h StringSplitter.h GlobalUsing.h
	Peer.cpp Peer.h IpWrapper.h IpMap.cpp IpMap.h NewspaperEntry.cpp NewspaperEntry.h Margins.h 
	Message.cpp Message.h Networking.cpp Networking.h StunServer.hpp StunServer.cpp StunMessages.hpp StunMessages.cpp
	StunClient.hpp StunClient.cpp XorMappedAddressAttribute.hpp XorMappedAddressAttribute.cpp
	ErrorCodeAttribute.hpp ErrorCodeAttribute.cpp UnknownAttributesAttribute.hpp
	UnknownAttributesAttribute.cpp TurnAllocation.hpp RequestedTransportAttribute.hpp RequestedTransportAttribute.cpp
	LifetimeAttribute.hpp LifetimeAttribute.cpp XorRelayedAddress.hpp XorRelayedAddress.cpp 
	PublicIdentifierAttribute.hpp PublicIdentifierAttribute.cpp StunTags.hpp CryptoUtils.hpp CryptoUtils.cpp
	DataAttribute.hpp DataAttribute.cpp PublicKeyAttribute.hpp PublicKeyAttribute.cpp LazyWrappers.hpp
	RelayedPublicIdentifierAttribute.hpp RelayedPublicIdentifierAttribute.cpp addmargin.h addmargin.cpp addmargin.ui)
# target_link_libraries(news_p2p_sharing stdc++fs ${Protobuf_LIBRARIES} proto_msg proto_art proto_nee proto_per proto_ipm cryptopp Qt5::Widgets Qt5::Network Qt5::Core)
target_link_libraries(news_p2p_sharing ${Protobuf_LIBRARIES} proto_msg proto_art proto_nee proto_per proto_ipm Qt5::Widgets Qt5::Network Qt5::Core cryptopp::cryptopp)

#if (DOXYGEN_FOUND)
#	doxygen_add_docs(news_p2p_sharing_docs
#		${CMAKE_SOURCE_DIR}
#		ALL
#		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
#else (DOXYGEN_FOUND)
#	message("Doxygen needs to be installed to generate the doxygen documentation.")
#endif (DOXYGEN_FOUND)
