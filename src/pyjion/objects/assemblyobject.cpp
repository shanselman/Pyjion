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
        self->asDict = PyDict_New();
        if (self->asDict == NULL) {
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

            auto publicTypeDefs = self->decoder->GetPublicClasses();
            for (const auto&def: publicTypeDefs){
                auto t = PyJitAssemblyType_new(self->decoder, def);
                if (t == nullptr){
                    return -1;
                }
                Py_XINCREF(t); // This now belongs to this assembly. Keep a reference.
                PyDict_SetItemString(self->asDict, self->decoder->GetString(def.Name).c_str(), t);
            }

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

static PyObject*
generic_meth(PyObject *self, PyTypeObject *cls, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames) {
    return Py_None;
}

static PyObject*
generic_static_meth(void *null, PyObject *const *args, Py_ssize_t nargs) {
    return Py_None;
}

static PyObject *
generic_repr(PyObject *self)
{
    return PyUnicode_FromFormat("<%s >", self->ob_type->tp_name);
}

PyObject *
PyJitAssemblyType_repr(PyjionAssemblyTypeObject *self)
{
    return PyUnicode_FromFormat("<%U name='%U' namespace='%U'>", self->name, self->name, self->namespace_);
}

static unordered_map<uint16_t, std::string> typeNames;
static unordered_map<uint16_t, std::string> fieldNames;
static unordered_map<uint16_t, std::string> methodNames;

PyObject * PyJitAssemblyType_new(PEDecoder* decoder, TypeDefRow typeDef){
    auto publicMethods = decoder->GetClassMethods(typeDef);
    auto publicFields = decoder->GetClassFields(typeDef);

    PyMemberDef *members;
    PyMethodDef *methods;
    PyTypeObject *type;
    PyType_Slot slots[3];
    PyType_Spec spec;

    /* Initialize MemberDefs */
    members = PyMem_NEW(PyMemberDef, publicFields.size() + 1);
    if (members == nullptr) {
        PyErr_NoMemory();
        return nullptr;
    }
    for (size_t i = 0 ; i < publicFields.size(); i++) {
        fieldNames[publicFields[i].Name] = decoder->GetString(publicFields[i].Name);
        members[i].name = fieldNames[publicFields[i].Name].c_str();
        members[i].offset = 0;
        members[i].doc = "Reflected type specification";
        members[i].flags = 0;
        members[i].type = T_OBJECT;
    }
    members[publicFields.size()] = {nullptr};

    methods = PyMem_NEW(PyMethodDef, publicMethods.size() + 1);
    if (methods == nullptr) {
        PyErr_NoMemory();
        return nullptr;
    }
    for (size_t i = 0 ; i < publicMethods.size(); i++) {
        methodNames[publicMethods[i].Name] = decoder->GetString(publicMethods[i].Name);
        methods[i].ml_name = methodNames[publicMethods[i].Name].c_str();
        if (IsMdStatic(publicMethods[i].Flags)) {
            methods[i].ml_flags = METH_FASTCALL | METH_STATIC;
            methods[i].ml_meth = reinterpret_cast<PyCFunction>(generic_static_meth);
        } else {
            methods[i].ml_flags = METH_METHOD | METH_FASTCALL | METH_KEYWORDS;
            methods[i].ml_meth = reinterpret_cast<PyCFunction>(generic_meth);
        }
        methods[i].ml_doc = "Reflected type specification";

    }
    methods[publicMethods.size()] = {nullptr};

    /* Initialize Slots */
    slots[0] = (PyType_Slot){Py_tp_members, members};
    slots[1] = (PyType_Slot){Py_tp_methods, methods};
    slots[2] = (PyType_Slot){0, 0};

    /* Initialize Spec */
    /* The name in this PyType_Spec is statically allocated so it is */
    /* expected that it'll outlive the PyType_Spec */
    typeNames[typeDef.Name] = std::string("pyjion.").append(decoder->GetString(typeDef.Name));
    spec.name = typeNames[typeDef.Name].c_str();
    spec.basicsize = sizeof(PyjionAssemblyTypeObject);
    spec.itemsize = 0;
    spec.flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HEAPTYPE;
    spec.slots = slots;

    type = (PyTypeObject *)PyType_FromSpecWithBases(&spec, (PyObject*)&PyjionAssemblyType_Type);
    PyMem_Free(members);
    if (type == nullptr) {
        PyErr_Print();
        return nullptr;
    }
    Py_INCREF(type);
    auto newObject = PyObject_New(PyjionAssemblyTypeObject, type);
    newObject->name = PyUnicode_FromString(decoder->GetString(typeDef.Name).c_str());
    newObject->namespace_ = PyUnicode_FromString(decoder->GetString(typeDef.Namespace).c_str());
    newObject->flags = typeDef.Flags;
    return (PyObject*)newObject;
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
        PyObject_GenericGetAttr,              /* tp_getattro */
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
        offsetof(PyjionAssemblyObject, asDict),      /* tp_dictoffset */
        (initproc) PyJitAssembly_init,                   /* tp_init */
        PyType_GenericAlloc,                        /* tp_alloc */
        PyJitAssembly_new,                          /* tp_new */
        0,                            /* tp_free */
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
        (reprfunc) PyJitAssemblyType_repr,        /* tp_repr */
        0,                                    /* tp_as_number */
        0,                                    /* tp_as_sequence */
        0,                                    /* tp_as_mapping */
        0,                                    /* tp_hash */
        0,                                    /* tp_call */
        0,                                    /* tp_str */
        0,              /* tp_getattro */
        0,                                    /* tp_setattro */
        0,                                    /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                   /* tp_flags */
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