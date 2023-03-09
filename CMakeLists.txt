# Copyright 2023 Justus Zorn

cmake_minimum_required(VERSION 3.23)

project("Anomaly")

add_subdirectory("3rdParty/SDL")
add_subdirectory("3rdParty/netlib")

set(COMMON_SOURCE_FILES
	"Source/Common/Network.cpp"
)

add_executable("Anomaly" "Source/Client/Main.cpp" ${COMMON_SOURCE_FILES})
target_link_libraries("Anomaly" "SDL2-static" "SDL2main" "netlib" )
target_include_directories("Anomaly" PRIVATE "Source")

add_executable("AnomalyServer" "Source/Server/Main.cpp" ${COMMON_SOURCE_FILES})
target_link_libraries("AnomalyServer" "netlib")
target_include_directories("AnomalyServer" PRIVATE "Source")
