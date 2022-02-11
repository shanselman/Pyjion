/*
* The MIT License (MIT)
*
* Copyright (c) Microsoft Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
*/

#include <Python.h>
#include <pyjit.h>
#include <util.h>
#include <string>

bool InitializePyjion() {
    Py_Initialize();
    PyjionUnboxingError = PyErr_NewException("pyjion.PyjionUnboxingError", PyExc_ValueError, nullptr);
#ifdef WINDOWS
    JitInit(L"clrjit.dll");
#else
    JitInit(L"libclrjit.so");
#endif
    g_pyjionSettings.graph = true;
    g_pyjionSettings.debug = DebugMode::Debug;
    g_pyjionSettings.codeObjectSizeLimit = 1000000;
    g_pyjionSettings.exceptionHandling = true;
    setOptimizationLevel(2);
    //Py_Finalize();
    return true;
}

PyCodeObject* CompileCode(const char* code, py_ptr<PyObject> globals, py_ptr<PyObject> locals) {
    auto builtins = PyEval_GetBuiltins();
    PyDict_SetItemString(globals.get(), "__builtins__", builtins);

    PyRun_String(code, Py_file_input, globals.get(), locals.get());
    if (PyErr_Occurred()) {
        PyErr_Clear();
        return nullptr;
    }
    auto func = PyObject_ptr(PyObject_GetItem(locals.get(), PyUnicode_FromString("f")));
    if (func.get() == nullptr){
        return nullptr;
    }
    return (PyCodeObject*) PyObject_GetAttrString(func.get(), "__code__");;
}

PyObject* run(PyjionJittedCode* jittedcode, PyCodeObject* code, py_ptr<PyObject> globals, py_ptr<PyObject> locals) {
    auto sysModule = PyObject_ptr(PyImport_ImportModule("sys"));
    auto builtins = PyEval_GetBuiltins();
    PyDict_SetItemString(globals.get(), "__builtins__", builtins);
    PyDict_SetItemString(globals.get(), "sys", sysModule.get());
    auto profile = new PyjionCodeProfile();
    auto tstate = PyThreadState_Get();

    auto frame = PyFrame_New(tstate, code, globals.get(), locals.get());
    auto prev = _PyInterpreterState_GetEvalFrameFunc(PyInterpreterState_Main());
    _PyInterpreterState_SetEvalFrameFunc(PyInterpreterState_Main(), PyJit_EvalFrame);
    auto res = PyJit_ExecuteAndCompileFrame(jittedcode, frame, tstate, profile);
    assert(frame->f_stackdepth != -1);
    assert(frame->f_lasti >= 0);
    assert(frame->f_lasti * 2 < PyBytes_GET_SIZE(code->co_code));
    _PyInterpreterState_SetEvalFrameFunc(PyInterpreterState_Main(), prev);
    Py_DECREF(frame);
    PyGC_Collect();
    assert(!jittedcode->j_failed);
    assert(jittedcode->j_genericAddr != nullptr);
    delete profile;
    return res;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    static bool Initialized = InitializePyjion();
    auto input = string(reinterpret_cast<const char*>(Data), Size);
    
    auto globals = PyObject_ptr(PyDict_New());
    auto locals = PyObject_ptr(PyDict_New());
    auto co = CompileCode(input.c_str(), globals, locals);
    if (co == nullptr) {
    return 0;
    }
    auto jitted = PyJit_EnsureExtra((PyObject*) co);
    auto res = PyObject_ptr(run(jitted, co, globals, locals));
    if (res.get() == nullptr || PyErr_Occurred()) {
        PyErr_PrintEx(-1);
        PyErr_Clear();
        return 0;
    }

    auto repr = PyUnicode_AsUTF8(PyObject_Repr(res.get()));
    printf("Returned: %s \n", repr);

  return 0;  // Non-zero return values are reserved for future use.
}