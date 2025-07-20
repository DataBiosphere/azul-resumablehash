/*********************************************************************
* Filename:   _md5_ext.c
* Author:     Luke Moore, Hannes Schmidt
* Project:    https://github.com/luke-moore/resumablesha256/
* License:
    This is free and unencumbered software released into the public domain.

    Anyone is free to copy, modify, publish, use, compile, sell, or
    distribute this software, either in source code form or as a compiled
    binary, for any purpose, commercial or non-commercial, and by any
    means.

    In jurisdictions that recognize copyright laws, the author or authors
    of this software dedicate any and all copyright interest in the
    software to the public domain. We make this dedication for the benefit
    of the public at large and to the detriment of our heirs and
    successors. We intend this dedication to be an overt act of
    relinquishment in perpetuity of all present and future rights to this
    software under copyright law.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

    For more information, please refer to <https://unlicense.org>
*********************************************************************/

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "bcon/md5.h"

typedef struct {
    PyObject_HEAD
    MD5_CTX ctx;
} MD5Object;


static PyObject *
resumablehash_md5_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    MD5Object *self;
    self = (MD5Object *)type->tp_alloc(type, 0);
    if (self != NULL) {
        md5_init(&self->ctx);

        PyObject *initial = NULL;
        if (!PyArg_ParseTuple(args, "|O", &initial)) {
            Py_DECREF(self);
            return NULL;
        }
        if (initial) {
            if (!PyBytes_Check(initial)) {
                PyErr_SetString(PyExc_TypeError,
                    "Strings must be encoded before hashing");
                Py_DECREF(self);
                return NULL;
            }

            char *data;
            Py_ssize_t len;
            if (PyBytes_AsStringAndSize(initial, &data, &len) < 0) {
                Py_DECREF(self);
                return NULL;
            }
            md5_update(&self->ctx, (const unsigned char *)data, (size_t)len);
	}
    }
    return (PyObject *)self;
}

static void
resumablehash_md5_dealloc(MD5Object *self)
{
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *
resumablehash_md5_update(MD5Object *self, PyObject *args)
{
    const char *data;
    Py_ssize_t len;

    if (!PyArg_ParseTuple(args, "s#", &data, &len))
        return NULL;
    md5_update(&self->ctx, (const unsigned char *)data, (size_t)len);
    Py_RETURN_NONE;
}

static PyObject *
resumablehash_md5_digest(MD5Object *self, PyObject *Py_UNUSED(ignored))
{
    // Create a copy of the context so that we don’t change the state.
    unsigned char hash[16];
    MD5_CTX temp = self->ctx;
    md5_final(&temp, hash);
    return Py_BuildValue("y#", hash, 16);
}

static PyObject *
resumablehash_md5_hexdigest(MD5Object *self, PyObject *Py_UNUSED(ignored))
{
    unsigned char hash[16];
    char hex_output[65];
    MD5_CTX temp = self->ctx;
    md5_final(&temp, hash);
    for (int i = 0; i < 16; i++) {
        sprintf(hex_output + (i * 2), "%02x", hash[i]);
    }
    hex_output[64] = '\0';
    return Py_BuildValue("s", hex_output);
}

static PyObject *
resumablehash_md5_getstate(MD5Object *self, PyObject *Py_UNUSED(ignored))
{
    return PyBytes_FromStringAndSize(
        (const char *)&self->ctx, sizeof(MD5_CTX));
}

static PyObject *
resumablehash_md5_setstate(MD5Object *self, PyObject *state)
{
    char *buf;
    Py_ssize_t len;
    if (PyBytes_AsStringAndSize(state, &buf, &len) < 0) {
        return NULL;
    }
    if (len != sizeof(MD5_CTX)) {
        PyErr_SetString(PyExc_ValueError, "Invalid state length");
        return NULL;
    }
    memcpy(&self->ctx, buf, sizeof(MD5_CTX));
    Py_RETURN_NONE;
}

static PyObject *
resumablehash_md5_copy(MD5Object *self, PyObject *Py_UNUSED(ignored))
{
    MD5Object *new_obj = PyObject_New(MD5Object, Py_TYPE(self));
    if (new_obj == NULL)
        return NULL;
    memcpy(&new_obj->ctx, &self->ctx, sizeof(MD5_CTX));
    return (PyObject *)new_obj;
}

static PyObject *
resumablehash_md5_get_digest_size(MD5Object *self, void *closure)
{
    return PyLong_FromLong(16);
}

static PyObject *
resumablehash_md5_get_block_size(MD5Object *self, void *closure)
{
    return PyLong_FromLong(64);
}

static PyObject *
resumablehash_md5_get_name(MD5Object *self, void *closure)
{
    return PyUnicode_FromString("md5");
}

static PyGetSetDef resumablehash_md5_getsetters[] = {
    {"digest_size", (getter)resumablehash_md5_get_digest_size, NULL,
        "digest size", NULL},
    {"block_size",  (getter)resumablehash_md5_get_block_size, NULL,
        "block size", NULL},
    {"name", (getter)resumablehash_md5_get_name, NULL,
        "hash name", NULL},
    {NULL}
};

static PyMethodDef resumablehash_md5_methods[] = {
    {"update", (PyCFunction)resumablehash_md5_update, METH_VARARGS,
        "Update the hash with data"},
    {"digest", (PyCFunction)resumablehash_md5_digest, METH_NOARGS,
        "Return the binary digest"},
    {"hexdigest", (PyCFunction)resumablehash_md5_hexdigest, METH_NOARGS,
        "Return the hexadecimal digest"},
    {"__getstate__", (PyCFunction)resumablehash_md5_getstate, METH_NOARGS,
        "Return internal state for pickling"},
    {"__setstate__", (PyCFunction)resumablehash_md5_setstate, METH_O,
        "Restore internal state from pickled data"},
    {"copy", (PyCFunction)resumablehash_md5_copy, METH_NOARGS,
        "Return a copy of the hash object"},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject MD5Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "resumablehash.md5",
    .tp_doc = "Resumable md5 hash objects",
    .tp_basicsize = sizeof(MD5Object),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = resumablehash_md5_new,
    .tp_dealloc = (destructor)resumablehash_md5_dealloc,
    .tp_methods = resumablehash_md5_methods,
    .tp_getset = resumablehash_md5_getsetters,
};

static PyModuleDef resumablehash_md5_module = {
    PyModuleDef_HEAD_INIT,
    "resumablehash._md5_ext",
    "An MD5 implementation whose state can be saved and loaded",
    -1,
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit__md5_ext(void)
{
    PyObject *m;
    if (PyType_Ready(&MD5Type) < 0)
        return NULL;

    m = PyModule_Create(&resumablehash_md5_module);
    if (m == NULL)
        return NULL;

    Py_INCREF(&MD5Type);
    if (PyModule_AddObject(m, "md5", (PyObject *)&MD5Type) < 0) {
        Py_DECREF(&MD5Type);
        Py_DECREF(m);
        return NULL;
    }
    return m;
}
