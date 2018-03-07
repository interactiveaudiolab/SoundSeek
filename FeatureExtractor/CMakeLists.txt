cmake_minimum_required(VERSION 3.8)
project(FeatureExtractor)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES 
    main.cpp
    ../src/FeatureExtractor.cpp)

set(HEADER_FILES 
    ../src/FeatureExtractor.h
    ../src/FileUtils.h)

find_package (Eigen3 3.3 REQUIRED NO_MODULE)

find_package(Boost 1.62 REQUIRED COMPONENTS filesystem)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(FeatureExtractor ${SOURCE_FILES})
endif()

include_directories(/usr/local/Cellar/essentia/2.1_beta3/include)
link_directories(/usr/local/Cellar/essentia/2.1_beta3/lib)

target_include_directories(FeatureExtractor PRIVATE ${HEADER_FILES})
target_link_libraries(FeatureExtractor ${Boost_LIBRARIES} Eigen3::Eigen essentia)
