# Copyright 2023 Justus Zorn

cmake_minimum_required(VERSION 3.22)

project("Anomaly")

if(MSVC)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(ENET_STATIC ON)

add_subdirectory("3rdParty/enet")
add_subdirectory("3rdParty/glad")
add_subdirectory("3rdParty/SDL")
add_subdirectory("3rdParty/stb")

set(CLIENT_SOURCE_FILES
	"Source/Client/Client.cpp"
	"Source/Client/Main.cpp"
	"Source/Renderer/Input.cpp"
	"Source/Renderer/Renderer.cpp"
	"Source/Renderer/Shader.cpp"
	"Source/Renderer/Window.cpp"
)

set(SERVER_SOURCE_FILES
	"Source/Server/ContentManager.cpp"
	"Source/Server/Main.cpp"
	"Source/Server/Server.cpp"
)

if (ANDROID)
	add_library("main" SHARED ${CLIENT_SOURCE_FILES})
	target_link_libraries("main" "enet_static" "glad" "SDL2" "stb")
	target_include_directories("main" PRIVATE "Source")
	target_compile_definitions("main" PRIVATE "ANOMALY_MOBILE")
else()
	if (WIN32)
		add_executable("Anomaly" ${CLIENT_SOURCE_FILES} "Content/appicon.rc")
		add_executable("AnomalyServer" ${SERVER_SOURCE_FILES} "Content/appicon.rc")
	else()
		add_executable("Anomaly" ${CLIENT_SOURCE_FILES})
		add_executable("AnomalyServer" ${SERVER_SOURCE_FILES})
	endif()
	target_link_libraries("Anomaly" "enet_static" "glad" "SDL2main" "SDL2-static" "stb")
	target_include_directories("Anomaly" PRIVATE "Source")

	target_link_libraries("AnomalyServer" "enet_static" "stb")
	target_include_directories("AnomalyServer" PRIVATE "Source")
	target_compile_features("AnomalyServer" PRIVATE cxx_std_17)
endif()
