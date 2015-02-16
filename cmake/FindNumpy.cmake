# - Find numpy python module
# find_package(Numpy)
# This module finds numpy C header directory in
#   NUMPY_INCLUDE_DIR
# and sets 
#   NUMPY_NOT_FOUND 
# if the directory is not found


if (NUMPY_INCLUDE_DIR)
  message(WARNING, "numpy include dir was found in cache file")
else(NUMPY_INCLUDE_DIR)
  find_package(PythonInterp)
  if (PYTHONINTERP_FOUND)
    exec_program ("${PYTHON_EXECUTABLE}" 
      ARGS "-c 'import numpy; print(numpy.get_include())'"
      OUTPUT_VARIABLE NUMPY_INCLUDE_DIR
      RETURN_VALUE NUMPY_NOT_FOUND)
  else(PYTHONINTERP_FOUND)
    message("Python interpreter not found, unable to find numpy")
    set(NUMPY_NOT_FOUND TRUE)
  endif(PYTHONINTERP_FOUND)
endif (NUMPY_INCLUDE_DIR)
