# - Find numpy python module
# find_package(Numpy)
# This module defines
#   NUMPY_INCLUDE_DIR, numpy C header directory
#   NUMPY_FOUND, if the directory is found or not


if (NUMPY_INCLUDE_DIR)
  message(WARNING, "numpy include dir was found in cache file")
else(NUMPY_INCLUDE_DIR)
  find_package(PythonInterp)
  if (PYTHONINTERP_FOUND)
    exec_program("${PYTHON_EXECUTABLE}" 
      ARGS "-c 'import numpy; print(numpy.get_include())'"
      OUTPUT_VARIABLE NUMPY_INCLUDE_DIR)
      if(IS_DIRECTORY ${NUMPY_INCLUDE_DIR})
        set(NUMPY_FOUND TRUE)
      else(IS_DIRECTORY ${NUMPY_INCLUDE_DIR})
        set(NUMPY_FOUND FALSE)
      endif(IS_DIRECTORY ${NUMPY_INCLUDE_DIR})
  else(PYTHONINTERP_FOUND)
    message("Python interpreter not found, unable to find numpy")
    set(NUMPY_FOUND FALSE)
  endif(PYTHONINTERP_FOUND)
endif (NUMPY_INCLUDE_DIR)
