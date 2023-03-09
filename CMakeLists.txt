# Copyright 2023 Justus Zorn

project("Anomaly")

add_subdirectory("3rdParty/SDL")

add_executable("Anomaly" "Source/Main.cpp" )
target_link_libraries("Anomaly" "SDL2-static" "SDL2main")
