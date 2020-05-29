cmake_minimum_required(VERSION 3.14)
project(progetto_prova)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
set(CMAKE_CXX_FLAGS "--coverage")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_PREFIX_PATH ${BOOST_LIBRARYDIR}\\cmake)
set(BOOST_ROOT "C:/boost_1_72_0")

find_package(Boost CONFIG REQUIRED COMPONENTS regex)
find_package(Boost CONFIG REQUIRED COMPONENTS thread)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Sql REQUIRED)

list(APPEND LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::PrintSupport
        Qt5::Sql)
list(REMOVE_DUPLICATES LIBRARIES)
add_executable(progetto_prova main.cpp prova.pro prova.pro.user textedit.cpp textedit.h stacked.h stacked.cpp Client.cpp Client.h message.cpp message.h userpage.cpp userpage.h)

#target_link_libraries(DBMSProject ${Boost_LIBRARIES})  # that didn't work

target_link_libraries(progetto_prova ${LIBRARIES})
target_link_libraries(progetto_prova Boost::thread)
target_link_libraries(progetto_prova Boost::regex)
if(MSVC OR MINGW)
    target_link_libraries(progetto_prova ws2_32 mswsock)
endif()
