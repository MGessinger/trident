cmake_minimum_required(VERSION 3.0)
project(trident LANGUAGES C VERSION 0.1 DESCRIPTION "A functional language to connect one process to multiple pipes.")

# Add Bison and Flex to the project and set their targets
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

BISON_TARGET(parser src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c
                DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.h)
FLEX_TARGET(lexer src/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c)
ADD_FLEX_BISON_DEPENDENCY(lexer parser)

# Set the target and include my own directories
include_directories(src ${CMAKE_CURRENT_BINARY_DIR})

set(trident_src
	${BISON_parser_OUTPUTS}
	${FLEX_lexer_OUTPUTS}
	src/child_process.c
	src/targets.c
	src/command.c
	src/jobs.c
	src/main.c
)

add_executable(trident ${trident_src})
install(TARGETS trident)

if (MSVC)
	add_compile_options(/W4 /WX)
else()
	add_compile_options(-Wall -Wextra -Wpedantic)
endif()
