cmake_minimum_required(VERSION 3.26)
project(dz3)

set(CMAKE_CXX_STANDARD 17)

add_executable(dz3 main.cpp
        HashTable.cpp
        HashTable.h
        AddressFunction.h
        Exceptions.h
        Functions.h
        Functions.cpp)
