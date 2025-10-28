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
static void free_obj(void *);

static void *make_null(void);
static void *make_bool(int);
static void *make_number(uint8_t *, size_t, unsigned);

static void *make_string(void);
static int add_to_string(uint8_t *, size_t, void *);

/*  variables */
PyDoc_STRVAR(mod_doc, "JSON parser/ serializer");

static struct uni_json_p_binding binds = {
    .on_error =		on_error,

    .make_string =	make_string,
    .free_string =	free_obj,
    .add_2_string =	add_to_string,

    .make_null =	make_null,
    .free_null =	free_obj,

    .make_bool =	make_bool,
    .free_bool =	free_obj,

    .make_number =	make_number,
    .free_number =	free_obj
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

static void free_obj(void *obj)
{
    Py_DECREF(obj);
}

static void *make_null(void)
{
    Py_RETURN_NONE;
}

static void *make_bool(int true_false)
{
    PyObject *obj;

    obj = true_false ? Py_True : Py_False;
    Py_INCREF(obj);
    return obj;
}

static void *make_number(uint8_t *data, size_t len, unsigned flags)
{
    PyObject *s_obj, *n_obj;
    char *s;

    if (flags & UJ_NF_INT) {
        s = alloca(len + 1);
        memcpy(s, data, len);
        s[len] = 0;

        return PyLong_FromString(s, NULL, 10);
    }

    s_obj = PyByteArray_FromStringAndSize((char *)data, len);
    n_obj = PyFloat_FromString(s_obj);
    Py_DECREF(s_obj);

    return n_obj;
}

static void *make_string(void)
{

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
