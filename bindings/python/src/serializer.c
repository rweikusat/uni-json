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
#include "work_string.h"

/*  prototypes */
static void output(uint8_t *, size_t, void *);

/*  variables */
static struct uni_json_s_binding binds = {
    .output =		output
};

/*  routines */
static void output(uint8_t *data, size_t len, void *sink)
{
    add_2_work_string(data, len, sink);
}

PyObject _hidden_ * json_serialize(PyObject *, PyObject *args)
{
    PyObject *obj;
    int fmt, rc;

    fmt = UJ_FMT_FAST;
    rc = PyArg_ParseTuple(args, "O|i", &obj, &fmt);
    if (!rc) return NULL;
    switch (fmt) {
    case UJ_FMT_FAST:
    case UJ_FMT_DET:
    case UJ_FMT_PRETTY:
        break;

    default:
        PyErr_SetString(PyExc_ValueError, "wrong value for format argument");
        return NULL;
    }

    return PyUnicode_FromString("");
}
