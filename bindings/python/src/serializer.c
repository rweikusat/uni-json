/*
  uni-json bindings for Python -- serializer

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed
*/

/*  includes */
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "compiler.h"
#include "uni_json_s_binding.h"
#include "uni_json_serializer.h"

/*  routines */
PyObject _hidden_ * json_serialize(PyObject *, PyObject *args)
{
    fputs("Ha!\n", stderr);
    Py_RETURN_NONE;
}
