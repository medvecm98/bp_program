cmake_minimum_required(VERSION 3.17)
project(news_p2p_sharing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (UNIX)
	add_compile_options(-pedantic -O0 -g)
endif (UNIX)

# set(CMAKE_CXX_CLANG_TIDY
# 	clang-tidy;
# 	-header-filter=${CMAKE_CURRENT_SOURCE_DIR};
# )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (UNIX)
	include(FindProtobuf)
endif (UNIX)
find_package(Protobuf REQUIRED)
if (WIN32)
	include_directories(${Protobuf_INCLUDE_DIRS})
	find_package(cryptopp CONFIG REQUIRED)
endif (WIN32)
find_package(Qt5 COMPONENTS Core Network Widgets REQUIRED)
find_package(Doxygen REQUIRED dot)

if (UNIX)
	find_library(CryptoPP cryptopp)
endif (UNIX)

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
        RelayedPublicIdentifierAttribute.hpp RelayedPublicIdentifierAttribute.cpp addmargin.h addmargin.cpp addmargin.ui viewmargin.h viewmargin.cpp viewmargin.ui
        editarticle.h editarticle.cpp editarticle.ui newspaperlistview.h newspaperlistview.cpp newspaperlistview.ui)
if (UNIX)
	target_link_libraries(news_p2p_sharing stdc++fs proto_msg proto_art proto_nee proto_per proto_ipm ${PROTOBUF_LIBRARIES} ${CryptoPP} ${Boost_LIBRARIES}
		Qt5::Widgets Qt5::Network Qt5::Core)
endif (UNIX)
if (WIN32)
	target_link_libraries(news_p2p_sharing ${Protobuf_LIBRARIES} proto_msg proto_art proto_nee proto_per proto_ipm Qt5::Widgets Qt5::Network Qt5::Core cryptopp::cryptopp)
endif (WIN32)

# if (DOXYGEN_FOUND)
# 	doxygen_add_docs(news_p2p_sharing_docs
# 		${CMAKE_SOURCE_DIR}
# 		ALL
# 		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
# else (DOXYGEN_FOUND)
# 	message("Doxygen needs to be installed to generate the doxygen documentation.")
# endif (DOXYGEN_FOUND)
