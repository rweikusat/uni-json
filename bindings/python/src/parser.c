/*
  uni-json parser bindings for Python

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed
*/

/*  includes */
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "uni_json_p_binding.h"
#include "uni_json_parser.h"

/*  prototypes */
static PyObject *parse_json(PyObject *, PyObject *);

/*  variables */
PyDoc_STRVAR(mod_doc, "JSON parser/ serializer");

static struct uni_json_p_binding binds;

static PyMethodDef meths[] = {
    {"parse_json", parse_json, METH_VARARGS, "Parse a JSON string."},
    {0}
};

static PyModuleDef module = {
    .m_base =		PyModuleDef_HEAD_INIT,
    .m_name =		"UniJson",
    .m_doc =		mod_doc,
    .m_methods =	meths,
};

/*  routines */
static PyObject *parse_json(PyObject *, PyObject *args)
{
    uint8_t *data;
    void *obj;
    Py_ssize_t len;
    int rc;

    rc = PyArg_ParseTuple(args, "s#", &data, &len);
    if (!rc) Py_RETURN_NONE;

    obj = uni_json_parse(data, len, &binds, NULL);
    if (!obj) Py_RETURN_NONE;

    return obj;
}

PyMODINIT_FUNC PyInit_UniJson(void)
{
    return PyModule_Create(&module);
}
