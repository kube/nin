file(GLOB_RECURSE SOURCES "*.c")
add_library(libnin STATIC ${SOURCES})
target_include_directories(libnin PRIVATE "${CMAKE_SOURCE_DIR}/src" PUBLIC "${CMAKE_SOURCE_DIR}/include")
install(TARGETS libnin ARCHIVE DESTINATION "lib")
set_target_properties(libnin PROPERTIES OUTPUT_NAME nin)