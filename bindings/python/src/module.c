/*
  uni-json bindings for Python -- "module meta"

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed
*/

/*  includes */
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "uni_json_parser.h"
#include "uni_json_serializer.h"
#include "serializer.h"
#include "MODULE.h"
#include "PARSE_JSON.h"
#include "JSON_SER.h"

/*  types */
struct a_const {
    char *n;
    int v;
};

/*  prototypes */
PyObject *ec_2_msg(PyObject *, PyObject *);
PyObject *parse_json(PyObject *, PyObject *);
PyObject *json_serialize(PyObject *, PyObject *);

static int add_consts(PyObject *);

/*  variables */
static struct a_const consts[] = {
#define ac_(x) { .n = #x, .v = x }

    ac_(UJ_E_INV),
    ac_(UJ_E_NO_VAL),
    ac_(UJ_E_INV_LIT),
    ac_(UJ_E_GARBAGE),
    ac_(UJ_E_EOS),
    ac_(UJ_E_INV_IN),
    ac_(UJ_E_ADD),
    ac_(UJ_E_LEADZ),
    ac_(UJ_E_NO_DGS),
    ac_(UJ_E_INV_CHAR),
    ac_(UJ_E_INV_UTF8),
    ac_(UJ_E_INV_ESC),
    ac_(UJ_E_INV_KEY),
    ac_(UJ_E_NO_KEY),
    ac_(UJ_E_TOO_DEEP),
    ac_(UJ_E_MAKE),

    ac_(UJ_FMT_FAST),
    ac_(UJ_FMT_DET),
    ac_(UJ_FMT_PRETTY),

    ac_(UNK_TYPE_ERR),
    ac_(NONSTR_KEY_ERR),
    ac_(STRINGIFY_NONSTR_KEYS),

    {0}

#undef ac_
};

PyDoc_STRVAR(mod_doc, MODULE);

static PyMethodDef meths[] = {
    {"ec_2_msg", ec_2_msg, METH_VARARGS, "Map a parser error code to a standard text message."},
    {"parse_json", parse_json, METH_VARARGS, PARSE_JSON},
    {"json_serialize", json_serialize, METH_VARARGS, JSON_SER},
    {0}
};

static PyModuleDef_Slot slots[] = {
    {Py_mod_exec, add_consts},
    {0}
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
    struct a_const *ac;
    PyObject *obj;
    int rc;

    ac = consts;
    while (ac->n) {
        obj = PyLong_FromLong(ac->v);
        if (!obj) {
            PyErr_SetString(PyExc_MemoryError, "failed to create Long");
            return -1;
        }

        rc = PyModule_AddObjectRef(module, ac->n, obj);
        Py_DECREF(obj);
        if (rc) return rc;

        ++ac;
    }

    return 0;
}

PyMODINIT_FUNC PyInit_UniJson(void)
{
    return PyModuleDef_Init(&module);
}
