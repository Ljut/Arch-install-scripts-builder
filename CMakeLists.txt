cmake_minimum_required(VERSION 3.30)
project(archinstall C)

set(CMAKE_C_STANDARD 90)

add_executable(archinstall main.c
        stringformat.h
        archinstallcommands.h)

add_custom_command(
        TARGET archinstall POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/configs/commands.txt ${CMAKE_BINARY_DIR}/commands.txt
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/configs/packages.txt ${CMAKE_BINARY_DIR}/packages.txt
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/configs/partitions.txt ${CMAKE_BINARY_DIR}/partitions.txt
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/configs/settings.txt ${CMAKE_BINARY_DIR}/settings.txt
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/configs/users.txt ${CMAKE_BINARY_DIR}/users.txt
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/configs/postinstall.sh ${CMAKE_BINARY_DIR}/postinstall.sh
)
