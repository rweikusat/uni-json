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

static void on_error(unsigned, size_t, void *);
static void *make_null(void);

/*  variables */
PyDoc_STRVAR(mod_doc, "JSON parser/ serializer");

static struct uni_json_p_binding binds = {
    .on_error =		on_error,

    .make_null =	make_null
};

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
static void on_error(unsigned code, size_t pos, void *)
{
    char buf[1024];

    sprintf(buf, "%s (%u) at %zu",
            uni_json_ec_2_msg(code), code, pos);
    PyErr_SetString(PyExc_ValueError, buf);
}

static void *make_null(void)
{
    Py_RETURN_NONE;
}

static PyObject *parse_json(PyObject *, PyObject *args)
{
    uint8_t *data;
    Py_ssize_t len;
    int rc;

    rc = PyArg_ParseTuple(args, "s#", &data, &len);
    if (!rc) Py_RETURN_NONE;

    return uni_json_parse(data, len, &binds, NULL);
}

PyMODINIT_FUNC PyInit_UniJson(void)
{
    return PyModule_Create(&module);
}
