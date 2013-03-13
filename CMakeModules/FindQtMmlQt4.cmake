# Try to find the QtMmlWidget library
# Once done this will define
#
#  MML_FOUND - system has QtMmlWidget
#  MML_INCLUDE_DIR - the QtMmlWidget include directory
#  MML_LIBRARIES - Link these to use QtMmlWidget

find_path(MML_INCLUDE_DIR qtmmlwidget.h
    PATHS $ENV{MML_DIR}/include
          $ENV{MML_DIR}
          ~/Library/Frameworks
          /Library/Frameworks
          /usr/local/include
          /usr/include/
          /sw/include # Fink
          /opt/local/include # DarwinPorts
          /opt/csw/include # Blastwave
          /opt/include
          /usr/freeware/include
)

find_library(MML_LIBRARY
    NAMES mml QtSolutions_MMLWidget-2.4
    PATHS $ENV{MML_DIR}/lib
          $ENV{MML_DIR}
          ~/Library/Frameworks
          /Library/Frameworks
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          /sw/lib
          /opt/local/lib
          /opt/csw/lib
          /opt/lib
          /usr/freeware/lib64
)

if (NOT MML_INCLUDE_DIR)
  message(FATAL_ERROR "qtmmlwidget.h not found!")
endif (NOT MML_INCLUDE_DIR)


if (NOT MML_LIBRARY)
  message(FATAL_ERROR "qtmml library not found!")
endif (NOT MML_LIBRARY)


set(MML_FOUND "NO")
if(MML_LIBRARY AND MML_INCLUDE_DIR)
  set(MML_FOUND "YES")
endif(MML_LIBRARY AND MML_INCLUDE_DIR)


# handle the QUIETLY and REQUIRED arguments and set MML_FOUND to TRUE if
# all listed variables are TRUE

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MML DEFAULT_MSG MML_LIBRARY MML_INCLUDE_DIR)

mark_as_advanced(MML_INCLUDE_DIR MML_LIBRARY)

