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

/*  constants */
enum {
    WS_INITIAL =	32
};

/*  types */
struct work_string {
    uint8_t *s, *p, *e;
};

/*  prototypes */
static PyObject *parse_json(PyObject *, PyObject *);

static void on_error(unsigned, size_t, void *);
static void free_obj(void *);

static void *make_null(void);
static void *make_bool(int);
static void *make_number(uint8_t *, size_t, unsigned);

static void *make_work_string(void);
static void free_work_string(void *);
static int add_2_string(uint8_t *, size_t, void *);
static void *finalize_string(void *, uint8_t *, size_t);

/*  variables */
PyDoc_STRVAR(mod_doc, "JSON parser/ serializer");

static struct uni_json_p_binding binds = {
    .on_error =		on_error,

    .make_work_string =	make_work_string,
    .free_work_string =	free_work_string,
    .free_string =	free_obj,
    .add_2_string =	add_2_string,
    .finalize_string =	finalize_string,

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
    if (!s_obj) return NULL;
    n_obj = PyFloat_FromString(s_obj);
    Py_DECREF(s_obj);

    return n_obj;
}

static void *make_work_string(void)
{
    struct work_string *ws;

    ws = malloc(sizeof(*ws));
    if (!ws) return NULL;
    ws->s = ws->p = ws->e = NULL;

    return ws;
}

static int add_2_string(uint8_t *data, size_t len, void *str)
{
    struct work_string *ws;
    uint8_t *tmp;
    size_t in_ws, total;

    ws = str;
    if (!ws->s || ws->e - ws->p < (ptrdiff_t)len) {
        if (ws->s) {
            in_ws = ws->p - ws->s;
            total = ws->e - ws->s;
        } else {
            in_ws = 0;
            total = WS_INITIAL;
        }
        while (total - in_ws < len) total *= 2;

        tmp = realloc(ws->s, total);
        if (!tmp) return 0;
        ws->s = tmp;
        ws->p = tmp + in_ws;
        ws->e = tmp + total;
    }

    memcpy(ws->p, data, len);
    ws->p += len;
    return 1;
}

static void *finalize_string(void *str, uint8_t *data, size_t len)
{
    struct work_string *ws;
    void *obj;
    int rc;

    ws = str;
    if (ws->s) {
        if (len) {
            rc = add_2_string(data, len, ws);
            if (!rc) return NULL;
        }

        obj = PyUnicode_FromStringAndSize((char *)ws->s, ws->p - ws->s);
    } else {
        obj = PyUnicode_FromStringAndSize((char *)data, len);
    }

    free_work_string(ws);
    return obj;
}

static void free_work_string(void *str)
{
    struct work_string *ws;

    ws = str;
    if (ws->s) free(ws->s);
    free(ws);
}

static PyObject *parse_json(PyObject *, PyObject *args)
{
    uint8_t *data;
    Py_ssize_t len;
    int rc;

    rc = PyArg_ParseTuple(args, "s#", &data, &len);
    if (!rc) NULL;

    return uni_json_parse(data, len, &binds, NULL);
}

PyMODINIT_FUNC PyInit_UniJson(void)
{
    return PyModule_Create(&module);
}
