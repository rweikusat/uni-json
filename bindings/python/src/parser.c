/*
  uni-json bindings for Python -- parser

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed
*/

/*  includes */
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "compiler.h"
#include "uni_json_p_binding.h"
#include "uni_json_parser.h"
#include "work_string.h"

/*  prototypes */
static void on_error(unsigned, size_t, void *);
static void free_obj(void *);

static void *make_null(void);
static void *make_bool(int);
static void *make_number(uint8_t *, size_t, unsigned);

static void *do_make_work_string(void);
static void *finalize_string(void *, uint8_t *, size_t);

static void *make_array(void);
static int add_2_array(void *, void *);

static void *make_object(void);
static int add_2_object(void *, void *, void *);

/*  variables */
static struct uni_json_p_binding binds = {
    .on_error =		on_error,

    .make_object =	make_object,
    .free_object =	free_obj,
    .add_2_object =	add_2_object,

    .make_array =	make_array,
    .free_array =	free_obj,
    .add_2_array =	add_2_array,

    .make_work_string =	do_make_work_string,
    .free_work_string =	free_work_string,
    .free_string =	free_obj,
    .add_2_string =	add_2_work_string,
    .finalize_string =	finalize_string,

    .make_null =	make_null,
    .free_null =	free_obj,

    .make_bool =	make_bool,
    .free_bool =	free_obj,

    .make_number =	make_number,
    .free_number =	free_obj
};


/*  routines */
/**  general */
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

/**  simple types */
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

/**  string */
static void *do_make_work_string(void)
{
    void *p;

    p = make_work_string();
    if (!p) PyErr_SetString(PyExc_MemoryError, "failed to create work string");
    return p;
}

static void *finalize_string(void *str, uint8_t *data, size_t len)
{
    struct work_string *ws;
    void *obj;
    int rc;

    obj = NULL;
    ws = str;
    if (ws->s) {
        if (len) {
            rc = add_2_work_string(data, len, ws);
            if (rc == -1) {
                PyErr_SetString(PyExc_MemoryError, "failed to expand work string");
                goto out;
            }
        }

        data = ws->s;
        len = ws->p - ws->s;
    }

    obj = PyUnicode_FromStringAndSize((char *)data, len);

out:
    free_work_string(ws);
    return obj;
}

/**  array */
static void *make_array(void)
{
    return PyList_New(0);
}

static int add_2_array(void *obj, void *ary)
{
    int rc;

    rc = PyList_Append(ary, obj);
    if (rc == -1) {
        PyErr_Clear();
        return -1;
    }

    return 1;
}

/**  object */
static void *make_object(void)
{
    return PyDict_New();
}

static int add_2_object(void *k, void *v, void *obj)
{
    int rc;

    rc = PyDict_SetItem(obj, k, v);
    if (rc == -1) return -1;

    Py_DECREF(k);
    Py_DECREF(v);

    return 1;
}

/**  entry point */
PyObject _hidden_ *parse_json(PyObject *, PyObject *args)
{
    uint8_t *data;
    Py_ssize_t len;
    int rc;

    rc = PyArg_ParseTuple(args, "s#", &data, &len);
    if (!rc) NULL;

    return uni_json_parse(data, len, &binds, NULL);
}
