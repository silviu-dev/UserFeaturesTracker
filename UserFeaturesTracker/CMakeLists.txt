cmake_minimum_required(VERSION 3.14)

project(UserFeaturesTracker VERSION 1.0.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(NoInSourceBuilds)

#======================= INCLUSION OF OpenCV =======================#
find_package( OpenCV REQUIRED )

#=================== INCLUSION OF Project Files ====================#
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")

file(GLOB_RECURSE SOURCES
    "${SOURCE_DIR}/*.cpp"
)

#======================= FETCHING OF Dlib =======================#

include(FetchContent)
FetchContent_Declare(dlib
    GIT_REPOSITORY https://github.com/davisking/dlib.git
    GIT_TAG        v19.24
)
FetchContent_MakeAvailable(dlib)


#=================== SETUP EXECTUABLE ====================#

# Add the executable
add_executable(UserFeaturesTracker ${SOURCES})

# Add the target includes for UserFeaturesTracker 
target_include_directories(UserFeaturesTracker PRIVATE ${INCLUDE_DIR})
target_include_directories(UserFeaturesTracker PRIVATE ${OpenCV_INCLUDE_DIRS})

#===================== LINKING LIBRARIES =======================#
target_link_libraries(UserFeaturesTracker ${OpenCV_LIBS} )

target_link_libraries(UserFeaturesTracker dlib::dlib)