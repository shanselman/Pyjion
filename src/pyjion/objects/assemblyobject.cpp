//
// Created by Anthony Shaw on 27/10/21.
//

#include "assemblyobject.h"


PyObject *
PyJitAssembly_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyjionAssemblyObject *self;
    self = (PyjionAssemblyObject *) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->module_name = PyUnicode_FromString("");
        if (self->module_name == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->version = PyUnicode_FromString("");
        if (self->version == NULL) {
            Py_DECREF(self);
            return NULL;
        }
    }
    return (PyObject *) self;
}

int PyJitAssembly_init(PyjionAssemblyObject *self, PyObject *args, PyObject *kwargs){
    PyObject *_namespace = nullptr, *oldModuleName,  *oldVersion;

    if (!PyArg_ParseTuple(args, "O", &_namespace))
        return -1;

    if (_namespace) {
        try {
            self->decoder = new PEDecoder(PyUnicode_AsUTF8(_namespace));
            PyObject* module_name = PyUnicode_FromString(self->decoder->GetModuleName().c_str());
            PyObject* version = PyUnicode_FromString(self->decoder->GetVersion());
            oldModuleName = self->module_name;
            oldVersion = self->version;
            Py_INCREF(module_name);
            Py_INCREF(version);
            self->module_name = module_name;
            self->version = version;
            Py_XDECREF(oldVersion);
            Py_XDECREF(oldModuleName);
        } catch (InvalidImageException &e){
            PyErr_SetString(PyExc_ValueError, e.what());
            return -1;
        }
    }
    return 0;
}

void PyJitAssembly_dealloc(PyjionAssemblyObject * self){
    delete self->decoder;
    self->decoder = nullptr;
    Py_TYPE(self)->tp_free((PyObject *) self);
}

PyObject *
PyJitAssembly_repr(PyjionAssemblyObject *self)
{
    return PyUnicode_FromFormat("<Assembly module='%U' compilerId='%s' version='%U'>", self->module_name, self->decoder->GetCompilerID(), self->version);
}

PyObject * PyJitAssemblyType_new(PEDecoder* decoder, TypeDefRow typeDef){
    PyjionAssemblyTypeObject* obj = PyObject_New(PyjionAssemblyTypeObject, &PyjionAssemblyType_Type);
    obj->name = PyUnicode_FromString(decoder->GetString(typeDef.Name).c_str());
    obj->namespace_ = PyUnicode_FromString(decoder->GetString(typeDef.Namespace).c_str());
    return reinterpret_cast<PyObject*>(obj);
}

PyObject *
PyJitAssembly_getattro(PyjionAssemblyObject* self, PyObject* name){
    auto publicTypeDefs = self->decoder->GetPublicClasses();
    for (const auto&def: publicTypeDefs){
        if (PyUnicode_CompareWithASCIIString(name, self->decoder->GetString(def.Name).c_str()) == 0){
            auto t = PyJitAssemblyType_new(self->decoder, def);
            Py_INCREF(t); // This now belongs to this assembly. Keep a reference.
            return t;
        }
    }
    return PyObject_GenericGetAttr(reinterpret_cast<PyObject*>(self), name);
}

PyTypeObject PyjionAssembly_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0) "pyjion.Assembly", /* tp_name */
        sizeof(PyjionAssemblyObject),                                 /* tp_basicsize */
        0,                                                              /* tp_itemsize */
        /* methods */
        (destructor) PyJitAssembly_dealloc,            /* tp_dealloc */
        0,                                    /* tp_vectorcall_offset */
        0,                                    /* tp_getattr */
        0,                                    /* tp_setattr */
        0,                                    /* tp_as_async */
        (reprfunc) PyJitAssembly_repr,        /* tp_repr */
        0,                                    /* tp_as_number */
        0,                                    /* tp_as_sequence */
        0,                                    /* tp_as_mapping */
        0,                                    /* tp_hash */
        0,                                    /* tp_call */
        0,                                    /* tp_str */
        reinterpret_cast<getattrofunc>(PyJitAssembly_getattro),              /* tp_getattro */
        0,                                    /* tp_setattro */
        0,                                    /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                   /* tp_flags */
        0,                                    /* tp_doc */
        0,                                    /* tp_traverse */
        0,                                    /* tp_clear */
        0,                                    /* tp_richcompare */
        0,                                    /* tp_weaklistoffset */
        0,                                    /* tp_iter */
        0,                                    /* tp_iternext */
        PyJitAssembly_methods,                /* tp_methods */
        PyJitAssembly_members,                                    /* tp_members */
        0,                                          /* tp_getset */
        0,                                          /* tp_base */
        0,                                          /* tp_dict */
        0,                                          /* tp_descr_get */
        0,                                          /* tp_descr_set */
        0,      /* tp_dictoffset */
        (initproc) PyJitAssembly_init,                   /* tp_init */
        PyType_GenericAlloc,                        /* tp_alloc */
        PyJitAssembly_new,                          /* tp_new */
        PyObject_GC_Del,                            /* tp_free */
};

PyTypeObject PyjionAssemblyType_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0) "pyjion.AssemblyType", /* tp_name */
        sizeof(PyjionAssemblyTypeObject),                                 /* tp_basicsize */
        0,                                                              /* tp_itemsize */
        /* methods */
        0,            /* tp_dealloc */
        0,                                    /* tp_vectorcall_offset */
        0,                                    /* tp_getattr */
        0,                                    /* tp_setattr */
        0,                                    /* tp_as_async */
        0,        /* tp_repr */
        0,                                    /* tp_as_number */
        0,                                    /* tp_as_sequence */
        0,                                    /* tp_as_mapping */
        0,                                    /* tp_hash */
        0,                                    /* tp_call */
        0,                                    /* tp_str */
        0,              /* tp_getattro */
        0,                                    /* tp_setattro */
        0,                                    /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                   /* tp_flags */
        0,                                    /* tp_doc */
        0,                                    /* tp_traverse */
        0,                                    /* tp_clear */
        0,                                    /* tp_richcompare */
        0,                                    /* tp_weaklistoffset */
        0,                                    /* tp_iter */
        0,                                    /* tp_iternext */
        0,                /* tp_methods */
        PyJitAssemblyType_members,                                    /* tp_members */
        0,                                          /* tp_getset */
        0,                                          /* tp_base */
        0,                                          /* tp_dict */
        0,                                          /* tp_descr_get */
        0,                                          /* tp_descr_set */
        0,      /* tp_dictoffset */
        0,                   /* tp_init */
        PyType_GenericAlloc,                        /* tp_alloc */
        0,                          /* tp_new */
        PyObject_GC_Del,                            /* tp_free */
};