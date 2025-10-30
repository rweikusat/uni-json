/*
  uni-json bindings for Python -- serializer

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed
*/

/*  includes */
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "compiler.h"
#include "uni_json_s_binding.h"
#include "uni_json_serializer.h"
#include "uni_json_types.h"
#include "work_string.h"

/*  types */
struct aiter {
    void *ary;
    size_t len, pos;
};

/*  prototypes */
static void output(uint8_t *, size_t, void *);
static int type_of(void *);

static void *start_array_traversal(void *ary);
static void end_array_traversal(void *aiter);
static void *next_value(void *aiter);

static void get_num_data(void *, struct uj_data *);
static void free_num_data(struct uj_data *);

static void get_string_data(void *, struct uj_data *);
static int get_bool_value(void *);

/*  variables */
static struct uni_json_s_binding binds = {
    .output =			output,
    .type_of =			type_of,

    .start_array_traversal =	start_array_traversal,
    .end_array_traversal =	end_array_traversal,
    .next_value =		next_value,

    .get_num_data =	get_num_data,
    .free_num_data =	free_num_data,

    .get_string_data =	get_string_data,
    .get_bool_value =	get_bool_value
};

/*  routines */
static void output(uint8_t *data, size_t len, void *sink)
{
    add_2_work_string(data, len, sink);
}

static int type_of(void *obj)
{
    PyTypeObject *tp;
    double d;

    if ((PyObject *)obj == Py_None) return UJ_T_NULL;

    tp = Py_TYPE(obj);
    if (tp == &PyBool_Type) return UJ_T_BOOL;
    if (tp == &PyLong_Type) return UJ_T_NUM;
    if (tp == &PyUnicode_Type) return UJ_T_STR;
    if (tp == &PyList_Type) return UJ_T_ARY;
    if (tp == &PyDict_Type) return UJ_T_OBJ;

    if (tp == &PyFloat_Type) {
        d = PyFloat_AsDouble(obj);
        switch (fpclassify(d)) {
        case FP_NAN:
        case FP_INFINITE:
            return UJ_T_UNK;
        }

        return UJ_T_NUM;
    }

    return UJ_T_UNK;
}

static void *start_array_traversal(void *ary)
{
    struct aiter *aiter;

    aiter = malloc(sizeof(*aiter));
    aiter->len = PyList_GET_SIZE(ary);
    aiter->pos = 0;
}

static void end_array_traversal(void *aiter)
{
    free(aiter);
}

static void *next_value(void *aiter)
{
    struct aiter *ai;
    void *v;

    ai = aiter;
    if (ai->pos == ai->len) return NULL;

    v = PyList_GET_ITEM(ai->ary, ai->pos);
    ++ai->pos;

    return v;
}

static void get_num_data(void *num, struct uj_data *ndata)
{
    PyObject *str;
    Py_ssize_t len;
    char const *s;

    str = PyObject_Str(num);
    s = PyUnicode_AsUTF8AndSize(str, &len);

    ndata->s = malloc(len);
    memcpy(ndata->s, s, len);
    ndata->len = len;

    Py_DECREF(str);
}

static void free_num_data(struct uj_data *ndata)
{
    free(ndata->s);
}

static void get_string_data(void *str, struct uj_data *sdata)
{
    Py_ssize_t len;

    sdata->s = (uint8_t *)PyUnicode_AsUTF8AndSize(str, &len);
    sdata->len = len;
}

static int get_bool_value(void *boolean)
{
    return (PyObject *)boolean == Py_True;
}

PyObject _hidden_ * json_serialize(PyObject *, PyObject *args)
{
    PyObject *obj;
    struct work_string *ws;
    int fmt, rc;

    fmt = UJ_FMT_FAST;
    rc = PyArg_ParseTuple(args, "O|i", &obj, &fmt);
    if (!rc) return NULL;
    switch (fmt) {
    case UJ_FMT_FAST:
    case UJ_FMT_DET:
    case UJ_FMT_PRETTY:
        break;

    default:
        PyErr_SetString(PyExc_ValueError, "wrong value for format argument");
        return NULL;
    }

    ws = make_work_string();
    if (!ws) {
        PyErr_SetString(PyExc_MemoryError, "failed to create work string");
        return NULL;
    }

    uni_json_serialize(obj, ws, &binds, fmt);
    obj = PyUnicode_FromStringAndSize((char *)ws->s, ws->p - ws->s);
    free_work_string(ws);

    return obj;
}
