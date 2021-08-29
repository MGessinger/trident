cmake_minimum_required(VERSION 3.0)
project(Multipipe LANGUAGES C VERSION 0.1 DESCRIPTION "A functional language to connect one process to multiple pipes.")

set(multipipe_src
	src/child_process.c
	src/main.c
)

add_executable(multipipe ${multipipe_src})
