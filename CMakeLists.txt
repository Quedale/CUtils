cmake_minimum_required(VERSION 3.18.1)
project(cutils VERSION 0.0.1)

option(ENABLE_SHARED "Build libraries as shared by default" OFF)

if (ENABLE_SHARED)
  set(LIB_BUILD_TYPE SHARED)
else ()
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.dylib;.so;")
  set(LIB_BUILD_TYPE STATIC)
endif ()

set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/release)
set(LIBRARY_OUTPUT_PATH    ${OUTPUT_DIR})
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})

aux_source_directory(src src_dir)

add_library(cutils ${LIB_BUILD_TYPE}  ${src_dir})
install(TARGETS cutils EXPORT cutils DESTINATION "${CMAKE_INSTALL_LIBDIR}")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX}/bin)
set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include)

find_package(Threads REQUIRED)

if(Threads_FOUND)
  message("Linking thread")
  target_link_libraries(cutils PRIVATE Threads::Threads)
else()
  message(FATAL_ERROR  "Threads not found...")
endif()


#Defining headers to export
set(headers ${PROJECT_SOURCE_DIR}/src/clist_ts.h
            ${PROJECT_SOURCE_DIR}/src/clist.h
            ${PROJECT_SOURCE_DIR}/src/clogger.h
            ${PROJECT_SOURCE_DIR}/src/cobject.h
            ${PROJECT_SOURCE_DIR}/src/portable_thread.h
            ${PROJECT_SOURCE_DIR}/src/url_parser.h
            ${PROJECT_SOURCE_DIR}/src/cstring_utils.h)
install(FILES ${headers} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

#Setting pkgconfig file file variables
set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${CMAKE_INSTALL_BINDIR})
set(libdir ${CMAKE_INSTALL_LIBDIR})
set(includedir ${CMAKE_INSTALL_INCLUDEDIR})

if(NOT WIN32)
  target_compile_options(cutils PRIVATE "SHELL: -fPIC")

  #Create pkgconfig file
  configure_file(cutils.pc.in cutils.pc @ONLY) 
  install(FILES ${PROJECT_BINARY_DIR}/cutils.pc DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
endif()

if(ANDROID)
  #See https://github.com/llvm/llvm-project/issues/76375
  target_compile_options(cutils PRIVATE "-Wno-gnu-zero-variadic-macro-arguments")
  find_library(LOG_LIB log REQUIRED)
  target_link_libraries(cutils PRIVATE log)
else()
  target_compile_options(cutils PRIVATE "SHELL: -Wall -Wextra -Wpedantic -Wno-unused-parameter")
endif()

# aux_source_directory(samples log_src)
add_executable(logging samples/logging.c)
target_link_libraries(logging PRIVATE cutils)

add_executable(urlparserdemo samples/url_parser_demo.c)
target_link_libraries(urlparserdemo PRIVATE cutils)

target_include_directories (cutils PUBLIC "${CMAKE_CURRENT_LIST_DIR}/src")