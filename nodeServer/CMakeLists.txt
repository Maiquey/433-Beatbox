# CMakeLists.txt for node webserver
# Used a bit of help from GPT here

add_custom_target(node_server
    COMMAND ${CMAKE_MAKE_PROGRAM}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../nodeServer
    COMMENT "Running NodeJS Makefile"
    VERBATIM
)

# Build nodeJS server before hal and app modules
add_dependencies(hal node_server)