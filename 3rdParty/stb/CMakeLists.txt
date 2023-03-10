cmake_minimum_required(VERSION 3.23)

project("stb")

add_library("stb" STATIC "stb.c")
target_include_directories("stb" PUBLIC "include")
