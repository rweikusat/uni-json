/*
  uni-json parser bindings for Python

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed
*/

/*  includes */
#define PY_SSIZE_T_CLEAN
#include <Python.h>

/*  routines */
static PyObject *parse_json(PyObject *, PyObject *args)
{
    uint8_t *data;
    Py_ssize_t len;
    int rc;

    rc = PyArg_ParseTuple(args, "s#", &data, &len);
    if (rc) fprintf(stderr, "%.*s\n", (int)len, data);

    Py_RETURN_NONE;
}
