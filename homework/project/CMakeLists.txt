cmake_minimum_required(VERSION 3.15) # 最低版本

project(GraphExtractor) # 项目名称

set(CMAKE_CXX_STANDARD 17) # C++标准

include(FetchContent) # 引入FetchContent模块管理外部依赖

# FetchContent_Declare(
#     nlohmann_json
#     GIT_REPOSITORY https://github.com/nlohmann/json.git
#     GIT_TAG v3.11.2
# )
# FetchContent_MakeAvailable(nlohmann_json)

find_package(nlohmann_json REQUIRED)

add_executable(mytool
    main.cpp
    utils.cpp
    utils.hpp
    graph_extractor.cpp
    graph_extractor.hpp
)

target_link_libraries(mytool nlohmann_json::nlohmann_json) # 链接nlohmann_json库

