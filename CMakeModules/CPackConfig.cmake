cmake_minimum_required(VERSION 3.8.2)

INCLUDE(Version)

##
# Activate (optional) generators
##
# Source package
SET(CPACK_SOURCE_GENERATOR "")
OPTION(CREATE_TGZ "Create a .tgz install file" ON)
IF(${CREATE_TGZ})
  LIST(APPEND CPACK_GENERATOR "TGZ")
ENDIF()

# Self-extracting installer for *nix operating systems
OPTION(CREATE_STGZ "Create .sh install file" ON)
MARK_AS_ADVANCED(CREATE_STGZ)
IF(${CREATE_STGZ})
  LIST(APPEND CPACK_GENERATOR "STGZ")
ENDIF()

# Debian packaging
OPTION(CREATE_DEB "Create .deb install file" OFF)
MARK_AS_ADVANCED(CREATE_DEB)
IF(${CREATE_DEB})
  LIST(APPEND CPACK_GENERATOR "DEB")
  SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${PROCESSOR_ARCHITECTURE})
ENDIF()

# RPM packaging
OPTION(CREATE_RPM "Create .rpm install file" OFF)
MARK_AS_ADVANCED(CREATE_RPM)
IF(${CREATE_RPM})
  LIST(APPEND CPACK_GENERATOR "RPM")
  SET(CPACK_RPM_PACKAGE_LICENSE "CHANGE_ME_LICENSE_SHORT_NAME (e.g. BSD)")
  SET(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
ENDIF()

##
# Common information to all packaging tools
##
SET(CPACK_PREFIX_DIR ${CMAKE_INSTALL_PREFIX})
SET(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
SET(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
SET(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
SET(CPACK_PACKAGE_VENDOR ${VENDOR_NAME})
SET(CPACK_PACKAGE_CONTACT ${VENDOR_EMAIL})
SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
SET(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")

# Long description of the package
SET(CPACK_PACKAGE_DESCRIPTION "CHANGE_ME_WRITE_IN_DESCRIPTION")

# Short description of the package
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CHANGE_ME_SHORT_DESCRIPTION")

SET(CPACK_PACKAGE_GROUP "CHANGE_ME_UPSTREAM_PACKAGE_GROUPING (e.g. Development/Software)")

# Useful descriptions for components
SET(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "CHANGE_ME_WRITE_IN_DISPLAY_NAME")
SET(CPACK_COMPONENT_DOCUMENTATION_NAME "Doxygen documentation")
SET(CPACK_COMPONENT_HEADERS_NAME "C/C++ headers")
SET(CPACK_COMPONENT_CMAKE_NAME "CMake support")
# Set the default components installed in the package
SET(CPACK_COMPONENTS_ALL libraries headers documentation cmake)

# Naming convention for the package, this  is probably ok in most cases
SET(CPACK_SOURCE_PACKAGE_FILE_NAME
  ${CPACK_PACKAGE_NAME}_src_${CPACK_PACKAGE_VERSION}_${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR})

# A standard list of files to ignore.
SET(CPACK_SOURCE_IGNORE_FILES
    "/build"
    "CMakeFiles"
    "/\\\\.dir"
    "/\\\\.git"
    "/\\\\.gitignore$"
    ".*~$"
    "\\\\.bak$"
    "\\\\.swp$"
    "\\\\.orig$"
    "/\\\\.DS_Store$"
    "/Thumbs\\\\.db"
    "/CMakeLists.txt.user$"
    ${CPACK_SOURCE_IGNORE_FILES})

# Ignore build directories that may be in the source tree
FILE(GLOB_RECURSE CACHES "${CMAKE_SOURCE_DIR}/CMakeCache.txt")

# Call to CPACK
INCLUDE(CPack)
