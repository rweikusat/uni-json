/*
  uni-json parser bindings for Python

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed
*/

/*  includes */
#define PY_SSIZE_T_CLEAN
#include <Python.h>

/*  prototypes */
static PyObject *parse_json(PyObject *, PyObject *);

/*  variables */
PyDoc_STRVAR(mod_doc, "JSON parser/ serializer");

static PyMethodDef meths[] = {
    {"parse_json", parse_json, METH_VARGS, "Parse a JSON string."},
    {0}
};

static PyModuleDef module = {
    .m_base =		PyModuleDef_HEAD_INIT,
    .m_name =		"UniJson",
    .m_doc =		mod_doc,
    .m_methods =	&meths,
};

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

PyMODINIT_FUNC PyInit_UniJson(void)
{
    return PyModule_Create(&module);
}
