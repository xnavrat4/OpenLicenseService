cmake_minimum_required(VERSION 3.14)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Network)

add_library( HWIdentifiers	STATIC
   diskinfo.h diskinfo.cpp
   operatingsystem.h operatingsystem.cpp
   hwidentifiers.h hwidentifiers.cpp
   memoryinfo.h memoryinfo.cpp
   processorinfo.h processorinfo.cpp
   motherboardinfo.h motherboardinfo.cpp
)

target_link_libraries(HWIdentifiers
    Qt${QT_VERSION_MAJOR}::Core
    Qt6::Network)

