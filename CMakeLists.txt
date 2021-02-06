cmake_minimum_required(VERSION 3.10)

#Set up project
project(recty VERSION 1.0)

#Set executable for server
add_executable(recty_server   
    src/server.cpp
)

#Set executable for client
add_executable(recty
    src/client.cpp
)

#Set module path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

#Find libraries
find_package(Threads)
find_package(SFML REQUIRED audio network graphics window system)

add_subdirectory(deps/imgui)
add_subdirectory(deps/imgui_sfml)
add_subdirectory(deps/enet)
add_subdirectory(deps/lua)

function(setup_target TARGET)
    #Set C++17
    target_compile_features(${TARGET} PUBLIC cxx_std_17)
    set_target_properties(${TARGET} PROPERTIES CXX_EXTENSIONS OFF)

    #Set flags
    if(MSVC)
        target_compile_options(${TARGET} PRIVATE 
            /W4 /WX)
    else()
        target_compile_options(${TARGET} PRIVATE 
            -Wall -Wextra -pedantic -Werror)
    endif()

    target_include_directories(
        ${TARGET}
        PRIVATE
        deps
    )
endfunction(setup_target)

setup_target(recty_server)
setup_target(recty)

#Finally
target_link_libraries(recty_server
    enet
    lua
    Threads::Threads 
    ${SFML_LIBRARIES} 
    ${SFML_DEPENDENCIES}
)

target_link_libraries(recty
    enet
    lua
    imgui
    imgui_sfml
    Threads::Threads 
    ${SFML_LIBRARIES} 
    ${SFML_DEPENDENCIES}
)