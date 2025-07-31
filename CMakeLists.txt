# CMakeLists.txt project file for Raylib projects
cmake_minimum_required(VERSION 3.30)
project(chess CXX)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Include the command that downloads libraries
# include(FetchContent)

# define a function for adding git dependencies
# function(include_dependency libName gitURL gitTag)
#     FetchContent_Declare(${libName}
#             GIT_REPOSITORY ${gitURL}
#             GIT_TAG        ${gitTag}
#             GIT_SHALLOW    TRUE
#             GIT_PROGRESS   TRUE
#     )
#     FetchContent_MakeAvailable(${libName})
# endfunction()

# add raylib support
set(LIB1 raylib)
find_package(${LIB1} QUIET)
# if (NOT ${LIB1}_FOUND)
#     message(STATUS "Getting ${LIB1} from Github")
#     include_dependency(${LIB1} https://github.com/raysan5/raylib.git 5.5)
# else()
#     message(STATUS "Using local ${LIB1}")
# endif()
file(GLOB p_SRC
     "src/*.cpp"
)
# add_executable(${PROJECT_NAME} src/main.cpp)
add_executable(${PROJECT_NAME} ${p_SRC})

# set the include directory
target_include_directories(${PROJECT_NAME} PRIVATE ${raylib_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE include)

# link all libraries to the project
target_link_libraries(${PROJECT_NAME} raylib)
# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
