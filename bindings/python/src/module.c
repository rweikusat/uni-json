/*
  uni-json bindings for Python -- "module meta"

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed
*/

/*  includes */
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "uni_json_serializer.h"

/*  prototypes */
PyObject *parse_json(PyObject *, PyObject *);
PyObject *json_serialize(PyObject *, PyObject *);

static int add_consts(PyObject *);

/*  variables */
PyDoc_STRVAR(mod_doc, "uni-json JSON parser/ serializer");

static PyMethodDef meths[] = {
    {"parse_json", parse_json, METH_VARARGS, "Parse a JSON string."},
    {"json_serialize", json_serialize, METH_VARARGS, "Serialize an object to a JSON string."},
    {0}
};

static PyModuleDef_Slot slots[] = {
    {Py_mod_exec, add_consts},
    {0, NULL}
};

static PyModuleDef module = {
    .m_base =		PyModuleDef_HEAD_INIT,
    .m_name =		"UniJson",
    .m_doc =		mod_doc,
    .m_methods =	meths,
    .m_slots =		slots
};

/*  routines */
static int add_consts(PyObject *module)
{
    PyModule_AddObject(module, "UJ_FMT_FAST", PyLong_FromLong(UJ_FMT_FAST));
    return 0;
}

PyMODINIT_FUNC PyInit_UniJson(void)
{
    return PyModuleDef_Init(&module);
}
