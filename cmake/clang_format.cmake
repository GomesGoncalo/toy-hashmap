# MIT License 
# Copyright (c) 2018-Today Michele Adduci <adduci@tutanota.com>
#
# Clang-Format instructions

find_program(CLANG_FORMAT_BIN NAMES clang-format)

if(CLANG_FORMAT_BIN)
  file(GLOB_RECURSE CPP_SOURCE_FILES *.cpp)
  file(GLOB_RECURSE CPP_HEADER_FILES *.h)

  add_custom_target(
    format-sources
    COMMAND clang-format --style=file -i ${CPP_SOURCE_FILES}
    COMMAND_EXPAND_LISTS VERBATIM)

  add_custom_target(
    format-headers
    COMMAND clang-format --style=file -i ${CPP_HEADER_FILES}
    COMMAND_EXPAND_LISTS VERBATIM)

  add_custom_target(
    format
    DEPENDS format-sources format-headers)
endif()
