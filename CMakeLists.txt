# Copyright 2023 Justus Zorn

cmake_minimum_required(VERSION 3.23)

project("Anomaly")

add_subdirectory("3rdParty/SDL")
add_subdirectory("3rdParty/netlib")

set(CLIENT_SOURCE_FILES
	"Source/Client/Main.cpp"
	"Source/Client/Renderer.cpp"
)

set(SERVER_SOURCE_FILES
	"Source/Server/Main.cpp"
)

set(COMMON_SOURCE_FILES
	"Source/Common/Network.cpp"
)

add_executable("Anomaly" ${CLIENT_SOURCE_FILES} ${COMMON_SOURCE_FILES})
target_link_libraries("Anomaly" "SDL2-static" "SDL2main" "netlib" )
target_include_directories("Anomaly" PRIVATE "Source")

add_executable("AnomalyServer" ${SERVER_SOURCE_FILES} ${COMMON_SOURCE_FILES})
target_link_libraries("AnomalyServer" "netlib")
target_include_directories("AnomalyServer" PRIVATE "Source")
