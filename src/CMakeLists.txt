cmake_minimum_required(VERSION 3.14)

project(OpenLicenseService LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(MSVC)
    include_directories(OpenLicenseCommunication/win64/includes)
    link_directories(OpenLicenseCommunication/win64)
endif()

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS WebSockets)


add_subdirectory(HWIdentifiers)
add_subdirectory(OpenLicenseCommunication)

set( project_sources
   main.cpp
   filemanager.h filemanager.cpp
   licenseservice.h licenseservice.cpp
   servicecreator.h servicecreator.cpp
   licensevalidator.h licensevalidator.cpp
   registerdevice.h registerdevice.cpp
   reporter.h reporter.cpp
   timemanager.h timemanager.cpp
)

add_executable(OpenLicenseService
    ${project_sources}
)
target_link_libraries(OpenLicenseService
    HWIdentifiers
    OpenLicenseCommunication
    Qt6::WebSockets
)

if(MSVC)
    target_link_libraries(OpenLicenseService
        HWIdentifiers
        OpenLicenseCommunication
        Qt6::WebSockets
        botan
    )
else()
    target_link_libraries(OpenLicenseService
        HWIdentifiers
        OpenLicenseCommunication
        Qt6::WebSockets
    )
endif()

install(TARGETS OpenLicenseService
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
