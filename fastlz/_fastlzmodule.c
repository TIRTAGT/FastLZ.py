#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <limits.h>

#include "vendor/fastlz.h"

static int validate_int_range(Py_ssize_t value, const char* name) {
  if (value > INT_MAX) {
    PyErr_Format(PyExc_OverflowError, "%s is too large", name);
    return 0;
  }
  return 1;
}

static int max_compressed_size_internal(Py_ssize_t input_length, Py_ssize_t* out) {
  if (input_length < 0) {
    PyErr_SetString(PyExc_ValueError, "input length must be non-negative");
    return 0;
  }

  size_t n = (size_t)input_length;
  size_t extra = (n * 5 + 99) / 100;
  size_t bound = n + extra;
  if (bound < 66) {
    bound = 66;
  }

  if (bound > (size_t)PY_SSIZE_T_MAX) {
    PyErr_SetString(PyExc_OverflowError, "calculated output size is too large");
    return 0;
  }

  *out = (Py_ssize_t)bound;
  return 1;
}

static PyObject* py_max_compressed_size(PyObject* self, PyObject* args) {
  Py_ssize_t input_length;
  Py_ssize_t bound;

  if (!PyArg_ParseTuple(args, "n:max_compressed_size", &input_length)) {
    return NULL;
  }

  if (!max_compressed_size_internal(input_length, &bound)) {
    return NULL;
  }

  return PyLong_FromSsize_t(bound);
}

static PyObject* py_compress(PyObject* self, PyObject* args, PyObject* kwargs) {
  static char* kwlist[] = {"data", "level", NULL};

  Py_buffer input = {0};
  int level = 1;
  Py_ssize_t output_size;
  PyObject* output = NULL;
  int compressed_size;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "y*|i:compress", kwlist, &input, &level)) {
    return NULL;
  }

  if (level != 1 && level != 2) {
    PyBuffer_Release(&input);
    PyErr_SetString(PyExc_ValueError, "level must be 1 or 2");
    return NULL;
  }

  if (!validate_int_range(input.len, "input size")) {
    PyBuffer_Release(&input);
    return NULL;
  }

  if (input.len == 0) {
    PyBuffer_Release(&input);
    return PyBytes_FromStringAndSize("", 0);
  }

  if (!max_compressed_size_internal(input.len, &output_size)) {
    PyBuffer_Release(&input);
    return NULL;
  }

  if (!validate_int_range(output_size, "output size")) {
    PyBuffer_Release(&input);
    return NULL;
  }

  output = PyBytes_FromStringAndSize(NULL, output_size);
  if (output == NULL) {
    PyBuffer_Release(&input);
    return NULL;
  }

  compressed_size = fastlz_compress_level(level, input.buf, (int)input.len, PyBytes_AS_STRING(output));
  PyBuffer_Release(&input);

  if (compressed_size <= 0) {
    Py_DECREF(output);
    PyErr_SetString(PyExc_ValueError, "compression failed");
    return NULL;
  }

  if (_PyBytes_Resize(&output, compressed_size) != 0) {
    return NULL;
  }

  return output;
}

static PyObject* py_decompress(PyObject* self, PyObject* args, PyObject* kwargs) {
  static char* kwlist[] = {"data", "maxout", NULL};

  Py_buffer input = {0};
  Py_ssize_t maxout;
  PyObject* output = NULL;
  int decompressed_size;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "y*n:decompress", kwlist, &input, &maxout)) {
    return NULL;
  }

  if (maxout < 0) {
    PyBuffer_Release(&input);
    PyErr_SetString(PyExc_ValueError, "maxout must be non-negative");
    return NULL;
  }

  if (!validate_int_range(input.len, "input size") || !validate_int_range(maxout, "maxout")) {
    PyBuffer_Release(&input);
    return NULL;
  }

  if (maxout == 0) {
    PyBuffer_Release(&input);
    return PyBytes_FromStringAndSize("", 0);
  }

  if (input.len == 0) {
    PyBuffer_Release(&input);
    PyErr_SetString(PyExc_ValueError, "compressed data is empty");
    return NULL;
  }

  output = PyBytes_FromStringAndSize(NULL, maxout);
  if (output == NULL) {
    PyBuffer_Release(&input);
    return NULL;
  }

  decompressed_size = fastlz_decompress(input.buf, (int)input.len, PyBytes_AS_STRING(output), (int)maxout);
  PyBuffer_Release(&input);

  if (decompressed_size <= 0) {
    Py_DECREF(output);
    PyErr_SetString(PyExc_ValueError, "invalid compressed data or insufficient maxout");
    return NULL;
  }

  if (_PyBytes_Resize(&output, decompressed_size) != 0) {
    return NULL;
  }

  return output;
}

static PyMethodDef module_methods[] = {
    {"compress", (PyCFunction)py_compress, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("compress(data, level=1) -> bytes")},
    {"decompress", (PyCFunction)py_decompress, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("decompress(data, maxout) -> bytes")},
    {"max_compressed_size", py_max_compressed_size, METH_VARARGS,
     PyDoc_STR("max_compressed_size(input_length) -> int")},
    {NULL, NULL, 0, NULL},
};

static struct PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    "_fastlz",
    "CPython wrapper around the FastLZ C library.",
    -1,
    module_methods,
};

PyMODINIT_FUNC PyInit__fastlz(void) {
  PyObject* module = PyModule_Create(&module_def);
  if (module == NULL) {
    return NULL;
  }

  if (PyModule_AddStringConstant(module, "__fastlz_version__", FASTLZ_VERSION_STRING) != 0) {
    Py_DECREF(module);
    return NULL;
  }

  if (PyModule_AddIntConstant(module, "FASTLZ_VERSION_MAJOR", FASTLZ_VERSION_MAJOR) != 0 ||
      PyModule_AddIntConstant(module, "FASTLZ_VERSION_MINOR", FASTLZ_VERSION_MINOR) != 0 ||
      PyModule_AddIntConstant(module, "FASTLZ_VERSION_REVISION", FASTLZ_VERSION_REVISION) != 0) {
    Py_DECREF(module);
    return NULL;
  }

  return module;
}
