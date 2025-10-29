/*
  uni-json bindings for Python -- "module meta"

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed
*/

/*  includes */
#define PY_SSIZE_T_CLEAN
#include <Python.h>

/*  prototypes */
PyObject *parse_json(PyObject *, PyObject *);
PyObject *json_serialize(PyObject *, PyObject *);

/*  variables */
PyDoc_STRVAR(mod_doc, "uni-json JSON parser/ serializer");

static PyMethodDef meths[] = {
    {"parse_json", parse_json, METH_VARARGS, "Parse a JSON string."},
    {"json_serialize", json_serialize, METH_VARARGS, "Serialize an object to a JSON string."},
    {0}
};

static PyModuleDef module = {
    .m_base =		PyModuleDef_HEAD_INIT,
    .m_name =		"UniJson",
    .m_doc =		mod_doc,
    .m_methods =	meths,
};

/*  routines */
PyMODINIT_FUNC PyInit_UniJson(void)
{
    return PyModule_Create(&module);
}
