%include <typemaps.i>
%include <std_string.i>


%fragment("SWIG_ByteArray_FromCharPtrAndSize","header",fragment="SWIG_pchar_descriptor") {
SWIGINTERNINLINE PyObject *
SWIG_ByteArray_FromCharPtrAndSize(const char* carray, size_t size)
{
  if (carray) {
    if (size > INT_MAX) {
      swig_type_info* pchar_descriptor = SWIG_pchar_descriptor();
      return pchar_descriptor ?
        SWIG_InternalNewPointerObj(%const_cast(carray,char *), pchar_descriptor, 0) : SWIG_Py_Void();
    } else {
      return PyByteArray_FromStringAndSize(carray, %numeric_cast(size,int));
    }
  } else {
    return SWIG_Py_Void();
  }
}
}


/* -----------------------------------------------------------------------------
 * cbytearray.i
 *
 * This library file contains macros for manipulating raw C data as bytearray.
 * ----------------------------------------------------------------------------- */

%{
typedef struct SWIGCBYTEARRAY {
    char *data;
    size_t   len;
} SWIGCBYTEARRAY;
%}

/* -----------------------------------------------------------------------------
 * Typemaps for returning binary data
 * ----------------------------------------------------------------------------- */

%typemap(out,noblock=1,fragment="SWIG_ByteArray_FromCharPtrAndSize") SWIGCBYTEARRAY {
  %set_output(SWIG_ByteArray_FromCharPtrAndSize($1.data,$1.len));
}
%typemap(in) (const void *indata, size_t inlen) = (char *STRING, size_t SIZE);


/* -----------------------------------------------------------------------------
 * %cbytearray(TYPE [, NAME]) 
 *
 * Convert raw C data to a bytearray.
 * ----------------------------------------------------------------------------- */

%define %cbytearray(TYPE,NAME...)

%insert("header") {
#ifdef __cplusplus
extern "C"  {
#endif
#if #NAME == ""
static SWIGCBYTEARRAY cbytearray_##TYPE(TYPE *ptr, size_t nelements)
#else
static SWIGCBYTEARRAY cbytearray_##NAME(TYPE *ptr, size_t nelements)
#endif
{
  SWIGCBYTEARRAY d;
  d.data = (char *) ptr;
#if #TYPE != "void"
  d.len  = nelements*sizeof(TYPE);
#else
  d.len  = nelements;
#endif
   return d;
}
#ifdef __cplusplus
}
#endif 
}

#ifdef __cplusplus
extern "C"
#endif
#if #NAME == ""
SWIGCBYTEARRAY cbytearray_##TYPE(TYPE *ptr, size_t nelements = 1);
#else
SWIGCBYTEARRAY cbytearray_##NAME(TYPE *ptr, size_t nelements = 1);
#endif
%enddef

%rename(cbytearray) ::cbytearray_void(void *ptr, size_t nelements = 1);

%cbytearray(void);

