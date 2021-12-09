#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "GLider::GLider" for configuration ""
set_property(TARGET GLider::GLider APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(GLider::GLider PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C;CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libGLider.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS GLider::GLider )
list(APPEND _IMPORT_CHECK_FILES_FOR_GLider::GLider "${_IMPORT_PREFIX}/lib/libGLider.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
