cmake_minimum_required(VERSION 2.6)
set(CMAKE_BUILD_TYPE Debug)
PROJECT(test_native)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/build)
set(APPLICATION_SOURCES test_functions.cpp pint_native.cpp)
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "-fpermissive -std=gnu++11 -Wl,-z,noexecstack ${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "-fpermissive -Wl,-z,noexecstack ${CMAKE_C_FLAGS}")
endif()

INCLUDE_DIRECTORIES("/usr/include/pint")
INCLUDE_DIRECTORIES(BEFORE "/home/swang/src/xmr-stak/")

link_directories(/usr/local/lib/pint)
#link_directories(${CMAKE_HOME_DIRECTORY}/src/pintdrv/build/)

add_executable(test_native ${APPLICATION_SOURCES})
#set_target_properties(pint PROPERTIES VERSION "1.0" SOVERSION "0")
target_link_libraries(${PROJECT_NAME} pintapi pthread)

INSTALL(FILES build/test_native DESTINATION ${CMAKE_HOME_DIRECTORY}/bin
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ WORLD_EXECUTE WORLD_READ)