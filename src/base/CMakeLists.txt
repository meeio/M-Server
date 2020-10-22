
file(GLOB ALL_FILES
    "include/*.hh",
    "source/*.cc",
)

message("BASE_LIB: ${ALL_FILES}")

add_library(base_lib ${ALL_FILES})

find_package(spdlog REQUIRED)

target_link_libraries(base_lib PUBLIC spdlog::spdlog)