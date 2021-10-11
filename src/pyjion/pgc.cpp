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
#include "pyjit.h"
#include "pycomp.h"

PgcStatus nextPgcStatus(PgcStatus status) {
    switch (status) {
        case PgcStatus::Uncompiled:
            return PgcStatus::CompiledWithProbes;
        case PgcStatus::CompiledWithProbes:
        case PgcStatus::Optimized:
        default:
            return PgcStatus::Optimized;
    }
}

PyjionCodeProfile::~PyjionCodeProfile() {
    // Don't decref types so that comparisons can be made to jumps
    //    for (auto &pos: this->stackTypes) {
    //        for(auto &observed: pos.second){
    //            Py_XDECREF(observed.second);
    //        }
    //    }
}

void PyjionCodeProfile::record(size_t opcodePosition, size_t stackPosition, PyObject* value) {
    if (PyGen_CheckExact(value) || PyCoro_CheckExact(value))
        return;
    if (this->stackTypes[opcodePosition][stackPosition] == nullptr) {
        this->stackTypes[opcodePosition][stackPosition] = Py_TYPE(value);
        Py_INCREF(Py_TYPE(value));
    }
    this->stackKinds[opcodePosition][stackPosition] = GetAbstractType(Py_TYPE(value), value);
}

PyTypeObject* PyjionCodeProfile::getType(size_t opcodePosition, size_t stackPosition) {
    return this->stackTypes[opcodePosition][stackPosition];
}

AbstractValueKind PyjionCodeProfile::getKind(size_t opcodePosition, size_t stackPosition) {
    return this->stackKinds[opcodePosition][stackPosition];
}

void capturePgcStackValue(PyjionCodeProfile* profile, PyObject* value, size_t opcodePosition, size_t stackPosition) {
    if (value != nullptr && profile != nullptr) {
        profile->record(opcodePosition, stackPosition, value);
    }
}
