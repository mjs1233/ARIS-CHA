find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_MODE_S_DECODE gnuradio-mode_s_decode)

FIND_PATH(
    GR_MODE_S_DECODE_INCLUDE_DIRS
    NAMES gnuradio/mode_s_decode/api.h
    HINTS $ENV{MODE_S_DECODE_DIR}/include
        ${PC_MODE_S_DECODE_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_MODE_S_DECODE_LIBRARIES
    NAMES gnuradio-mode_s_decode
    HINTS $ENV{MODE_S_DECODE_DIR}/lib
        ${PC_MODE_S_DECODE_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-mode_s_decodeTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_MODE_S_DECODE DEFAULT_MSG GR_MODE_S_DECODE_LIBRARIES GR_MODE_S_DECODE_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_MODE_S_DECODE_LIBRARIES GR_MODE_S_DECODE_INCLUDE_DIRS)
