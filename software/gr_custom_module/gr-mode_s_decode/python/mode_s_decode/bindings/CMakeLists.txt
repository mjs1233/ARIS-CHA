# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT mode_s_decode_sources)
    message(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT mode_s_decode_sources)

########################################################################
# Check for pygccxml
########################################################################
gr_python_check_module_raw("pygccxml" "import pygccxml" PYGCCXML_FOUND)

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND mode_s_decode_python_files
    preamble_correlation_python.cc python_bindings.cc)

gr_pybind_make_oot(mode_s_decode ../../.. gr::mode_s_decode "${mode_s_decode_python_files}")

# copy bindings extension for use in QA test module
add_custom_command(
    TARGET mode_s_decode_python
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:mode_s_decode_python>
            ${PROJECT_BINARY_DIR}/test_modules/gnuradio/mode_s_decode/)

install(
    TARGETS mode_s_decode_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/mode_s_decode
    COMPONENT pythonapi)
