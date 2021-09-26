cmake_minimum_required(VERSION 2.6)

project(AlicesTools)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

find_package(ROOT REQUIRED)

# include(CMakeFiles/FindROOT.cmake)


execute_process( COMMAND root-config --version
  COMMAND cut -b1-4
  OUTPUT_VARIABLE RVERSION
OUTPUT_STRIP_TRAILING_WHITESPACE )

include_directories(${ROOT_INCLUDE_DIRS} include/)

# if(DEFINED BUILD32)
#   set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-m32")
#   set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-m32")
#   set(CMAKE_LD_FLAGS ${CMAKE_LD_FLAGS} "-m32")
# endif(DEFINED BUILD32)

set(AlicesTools_INCDIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(AlicesTools_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Generate dictionaries for ROOT classes
set(AlicesTools_DICT AlicesToolsDict.cpp)
set(AlicesTools_DICTMOD ${CMAKE_BINARY_DIR}/AlicesToolsDict_rdict.pcm)

set(AlicesTools_INC
  ${AlicesTools_INCDIR}/apclass.h
  )
set(AlicesTools_INCALL
  ${AlicesTools_INC}
  ${AlicesTools_INCDIR}/apclassLinkDef.h
  )
set(AlicesTools_SRC
  ${AlicesTools_SRCDIR}/apclass.cpp
  )
 

if( RVERSION VERSION_GREATER 6.00 )
  # message( "ROOT6 detected" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  add_custom_command(
      OUTPUT ${AlicesTools_DICT}
      COMMAND rootcling -rootbuild -f ${AlicesTools_DICT} -c ${ROOTCINT_FLAGS} ${ROOTCINT_INCDIR} -I${ROOT_INCLUDE_DIRS} -I${AlicesTools_INCDIR} ${AlicesTools_INCALL}
      DEPENDS ${AlicesTools_INC}
    )
else()
  add_custom_command(
      OUTPUT ${AlicesTools_DICT}
      COMMAND rootcint -rootbuild -f ${AlicesTools_DICT} -c ${ROOTCINT_FLAGS} ${ROOTCINT_INCDIR} -I${ROOT_INCLUDE_DIRS} -I${AlicesTools_INCDIR} ${AlicesTools_INCALL}
      DEPENDS ${AlicesTools_INC}
    )
endif()



add_library(AlicesTools SHARED ${AlicesTools_SRC} ${AlicesTools_DICT})
#add_library(AlicesTools SHARED ${AlicesTools_SRCDIR}/AlicesTools.cpp)
target_link_libraries(AlicesTools ${ROOT_LIBRARIES} TMVA)

# install(TARGETS AlicesTools DESTINATION lib)
# install(FILES ${AlicesTools_INC} DESTINATION include)
# if( RVERSION VERSION_GREATER 6.00 )
#   install(FILES ${AlicesTools_DICTMOD} DESTINATION lib)
# endif()
