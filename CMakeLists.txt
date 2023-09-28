cmake_minimum_required(VERSION 3.0.0)
project(cutils VERSION 0.0.1)

set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/release)
set(LIBRARY_OUTPUT_PATH    ${OUTPUT_DIR})
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})

aux_source_directory(src src_dir)

add_library(cutils STATIC    ${src_dir})
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
            ${PROJECT_SOURCE_DIR}/src/portable_thread.h)
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

# aux_source_directory(samples log_src)
add_executable(logging samples/logging.c)
target_link_libraries(logging PRIVATE cutils)