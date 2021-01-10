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

#include "pyjitmath.h"
#include <opcode.h>

#define UNSUPPORTED_MATH_OP(opcode) PyErr_SetString(PyExc_NotImplementedError, "Operation not supported")

using namespace std;

bool isBinaryMathOp(int opcode){
    switch(opcode){
        case BINARY_TRUE_DIVIDE:
        case BINARY_FLOOR_DIVIDE:
        case BINARY_POWER:
        case BINARY_MODULO:
        case BINARY_MATRIX_MULTIPLY:
        case BINARY_MULTIPLY:
        case BINARY_SUBTRACT:
        case BINARY_ADD:
            return true;
    }
    return false;
}

bool isMathOp(int opcode) {
    switch(opcode){
        case BINARY_TRUE_DIVIDE:
        case BINARY_FLOOR_DIVIDE:
        case BINARY_POWER:
        case BINARY_MODULO:
        case BINARY_MATRIX_MULTIPLY:
        case BINARY_MULTIPLY:
        case BINARY_SUBTRACT:
        case BINARY_ADD:
        case INPLACE_POWER:
        case INPLACE_MULTIPLY:
        case INPLACE_MATRIX_MULTIPLY:
        case INPLACE_TRUE_DIVIDE:
        case INPLACE_FLOOR_DIVIDE:
        case INPLACE_MODULO:
        case INPLACE_ADD:
        case INPLACE_SUBTRACT:
            return true;
    }
    return false;
}

bool isInplaceMathOp(int opcode) {
    switch(opcode){
        case INPLACE_POWER:
        case INPLACE_MULTIPLY:
        case INPLACE_MATRIX_MULTIPLY:
        case INPLACE_TRUE_DIVIDE:
        case INPLACE_FLOOR_DIVIDE:
        case INPLACE_MODULO:
        case INPLACE_ADD:
        case INPLACE_SUBTRACT:
            return true;
    }
    return false;
}

PyObject* PyJitMath_TripleBinaryOp(PyObject* c, PyObject* a, PyObject* b, int firstOp, int secondOp) {
    PyObject* res;
    if (PyFloat_CheckExact(a) && PyFloat_CheckExact(b) && PyFloat_CheckExact(c))
        res = PyJitMath_TripleBinaryOpFloatFloatFloat((PyFloatObject*)a, (PyFloatObject*)b, (PyFloatObject*)c, firstOp, secondOp);
    else if (PyLong_CheckExact(a) && PyLong_CheckExact(b) && PyLong_CheckExact(c))
        res =  PyJitMath_TripleBinaryOpIntIntInt(a, b, c, firstOp, secondOp);
    else if (PyLong_CheckExact(a) && PyFloat_CheckExact(b) && PyFloat_CheckExact(c))
        res =  PyJitMath_TripleBinaryOpIntFloatFloat(a, b, c, firstOp, secondOp);
    else if (PyFloat_CheckExact(a) && PyLong_CheckExact(b) && PyLong_CheckExact(c))
        res =  PyJitMath_TripleBinaryOpFloatIntInt(a, b, c, firstOp, secondOp);
    else
        res =  PyJitMath_TripleBinaryOpObjObjObj(a, b, c, firstOp, secondOp);
    // Decref first op
    Py_DECREF(a);
    Py_DECREF(b);
    // Decref second op
    Py_DECREF(c);
    return res;
}

PyObject* PyJitMath_TripleBinaryOpFloatFloatFloat(PyFloatObject* a, PyFloatObject* b, PyFloatObject * c, int firstOp, int secondOp){
    double val_a = PyFloat_AS_DOUBLE(a);
    double val_b = PyFloat_AS_DOUBLE(b);
    double val_c = PyFloat_AS_DOUBLE(c);
    double res;
    switch(firstOp) {
        case BINARY_TRUE_DIVIDE:
            res = val_a / val_b;
            break;
        case BINARY_FLOOR_DIVIDE:
            res = floor(val_a / val_b);
            break;
        case BINARY_POWER:
            res = pow(val_a, val_b);
            break;
        case BINARY_MODULO:
        case BINARY_MATRIX_MULTIPLY:
            UNSUPPORTED_MATH_OP(firstOp);
            return nullptr;
        case BINARY_MULTIPLY:
            res = val_a * val_b;
            break;
        case BINARY_SUBTRACT:
            res = val_a - val_b;
            break;
        case BINARY_ADD:
            res = val_a + val_b;
            break;
    }
    switch (secondOp){
        case BINARY_TRUE_DIVIDE:
            return PyFloat_FromDouble(val_c / res);
        case BINARY_FLOOR_DIVIDE:
            return PyFloat_FromDouble(floor(val_c / res));
        case BINARY_POWER:
            return PyFloat_FromDouble(pow(val_c, res));
        case BINARY_MULTIPLY:
            return PyFloat_FromDouble(val_c * res);
        case BINARY_SUBTRACT:
            return PyFloat_FromDouble(val_c - res);
        case BINARY_ADD:
            return PyFloat_FromDouble(val_c + res);
        case INPLACE_POWER:
            c->ob_fval = pow(c->ob_fval, res);
            return (PyObject*)c;
        case INPLACE_MULTIPLY:
            c->ob_fval *= res;
            return (PyObject*)c;
        case INPLACE_TRUE_DIVIDE:
            c->ob_fval /= res;
            return (PyObject*)c;
        case INPLACE_FLOOR_DIVIDE:
            c->ob_fval = floor(c->ob_fval / res);
            return (PyObject*)c;
        case BINARY_MODULO:
        case BINARY_MATRIX_MULTIPLY:
        case INPLACE_MATRIX_MULTIPLY:
        case INPLACE_MODULO:
            UNSUPPORTED_MATH_OP(secondOp);
            return nullptr;
        case INPLACE_ADD:
            c->ob_fval += res;
            return (PyObject*)c;
        case INPLACE_SUBTRACT:
            c->ob_fval -= res;
            return (PyObject*)c;
    }
    return nullptr;
}

PyObject* PyJitMath_TripleBinaryOpIntIntInt(PyObject* a, PyObject* b, PyObject* c, int firstOp, int secondOp) {
    long val_a = PyLong_AsLong(a);
    long val_b = PyLong_AsLong(b);
    long val_c = PyLong_AsLong(c);
    long res;
    switch(firstOp) {
        case BINARY_TRUE_DIVIDE:
            res = val_a / val_b;
            break;
        case BINARY_FLOOR_DIVIDE:
            res = floor(val_a / val_b);
            break;
        case BINARY_POWER:
            res = pow(val_a, val_b);
            break;
        case BINARY_MODULO:
        case BINARY_MATRIX_MULTIPLY:
            UNSUPPORTED_MATH_OP(firstOp);
            return nullptr;
        case BINARY_MULTIPLY:
            res = val_a * val_b;
            break;
        case BINARY_SUBTRACT:
            res = val_a - val_b;
            break;
        case BINARY_ADD:
            res = val_a + val_b;
            break;
    }
    switch (secondOp){
        case BINARY_TRUE_DIVIDE:
            return PyFloat_FromDouble(val_c / res);
        case BINARY_FLOOR_DIVIDE:
            return PyLong_FromLong(floor(val_c / res));
        case BINARY_POWER:
            return PyLong_FromLong(pow(val_c, res));
        case BINARY_MULTIPLY:
            return PyFloat_FromDouble(val_c * res);
        case BINARY_SUBTRACT:
            return PyFloat_FromDouble(val_c - res);
        case BINARY_ADD:
            return PyFloat_FromDouble(val_c + res);
        case INPLACE_MULTIPLY:
            return PyLong_FromLong(val_c * res);
        case INPLACE_MATRIX_MULTIPLY:
            return PyLong_FromLong(val_c * res);
        case INPLACE_TRUE_DIVIDE:
            return PyFloat_FromDouble(val_c / res);
        case INPLACE_FLOOR_DIVIDE:
            return PyLong_FromLong(floor(val_c / res));
        case BINARY_MODULO:
        case BINARY_MATRIX_MULTIPLY:
        case INPLACE_MODULO:
            PyErr_SetString(PyExc_NotImplementedError, "Operation not supported");
            return nullptr;
        case INPLACE_POWER:
            return PyLong_FromLong(pow(val_c, res));
        case INPLACE_ADD:
            return PyLong_FromLong(val_c + res);
        case INPLACE_SUBTRACT:
            return PyLong_FromLong(val_c - res);
    }
    return nullptr;
}
PyObject* PyJitMath_TripleBinaryOpFloatIntInt(PyObject* a, PyObject* b, PyObject* c, int firstOp, int secondOp) {
    double val_a = PyFloat_AS_DOUBLE(a);
    long val_b = PyLong_AsLong(b);
    long val_c = PyLong_AsLong(c);
    double res;
    switch(firstOp) {
        case BINARY_TRUE_DIVIDE:
            res = val_a / val_b;
            break;
        case BINARY_FLOOR_DIVIDE:
            res = floor(val_a / val_b);
            break;
        case BINARY_POWER:
            res = pow(val_a, val_b);
            break;
        case BINARY_MODULO:
        case BINARY_MATRIX_MULTIPLY:
            UNSUPPORTED_MATH_OP(firstOp);
            return nullptr;
        case BINARY_MULTIPLY:
            res = val_a * val_b;
            break;
        case BINARY_SUBTRACT:
            res = val_a - val_b;
            break;
        case BINARY_ADD:
            res = val_a + val_b;
            break;
    }
    switch (secondOp){
        case BINARY_TRUE_DIVIDE:
            return PyFloat_FromDouble(res / val_c);
        case BINARY_FLOOR_DIVIDE:
            return PyFloat_FromDouble(floor(res / val_c));
        case BINARY_POWER:
            return PyFloat_FromDouble(pow(res, val_c));
        case BINARY_MODULO:
        case BINARY_MATRIX_MULTIPLY:
            PyErr_SetString(PyExc_NotImplementedError, "Operation not supported");
            return nullptr;
        case BINARY_MULTIPLY:
            return PyFloat_FromDouble(res * val_c);
        case BINARY_SUBTRACT:
            return PyFloat_FromDouble(res - val_c);
        case BINARY_ADD:
            return PyFloat_FromDouble(res + val_c);
        case INPLACE_POWER:
            return PyLong_FromLong(pow(res, val_c));
        case INPLACE_MULTIPLY:
            return PyLong_FromLong(res * val_c);
        case INPLACE_MATRIX_MULTIPLY:
            return PyLong_FromLong(res * val_c);
        case INPLACE_TRUE_DIVIDE:
            return PyLong_FromLong(res / val_c);
        case INPLACE_FLOOR_DIVIDE:
            return PyLong_FromLong(floor(res / val_c));
        case INPLACE_MODULO:
            return nullptr;
        case INPLACE_ADD:
            return PyLong_FromLong(res + val_c);
        case INPLACE_SUBTRACT:
            return PyLong_FromLong(res - val_c);
    }
    return nullptr;
}

PyObject* PyJitMath_TripleBinaryOpIntFloatFloat(PyObject* a, PyObject* b, PyObject* c, int firstOp, int secondOp) {
    long val_a = PyLong_AsLong(a); // TODO : Handle overflow gracefully
    double val_b = PyFloat_AS_DOUBLE(b);
    double val_c = PyFloat_AS_DOUBLE(c);
    double res;
    switch(firstOp) {
        case BINARY_TRUE_DIVIDE:
            res = val_a / val_b;
            break;
        case BINARY_FLOOR_DIVIDE:
            res = floor(val_a / val_b);
            break;
        case BINARY_POWER:
            res = pow(val_a, val_b);
            break;
        case BINARY_MODULO:
        case BINARY_MATRIX_MULTIPLY:
            UNSUPPORTED_MATH_OP(firstOp);
            return nullptr;
        case BINARY_MULTIPLY:
            res = val_a * val_b;
            break;
        case BINARY_SUBTRACT:
            res = val_a - val_b;
            break;
        case BINARY_ADD:
            res = val_a + val_b;
            break;
    }
    switch (secondOp){
        case BINARY_TRUE_DIVIDE:
            return PyFloat_FromDouble(res / val_c);
        case BINARY_FLOOR_DIVIDE:
            return PyFloat_FromDouble(floor(res / val_c));
        case BINARY_POWER:
            return PyFloat_FromDouble(pow(res, val_c));
        case BINARY_MULTIPLY:
            return PyFloat_FromDouble(res * val_c);
        case BINARY_SUBTRACT:
            return PyFloat_FromDouble(res - val_c);
        case BINARY_ADD:
            return PyFloat_FromDouble(res + val_c);
        case INPLACE_MULTIPLY:
            return PyLong_FromLong(res * val_c);
        case INPLACE_MATRIX_MULTIPLY:
            return PyLong_FromLong(res * val_c);
        case INPLACE_TRUE_DIVIDE:
            return PyLong_FromLong(res / val_c);
        case INPLACE_FLOOR_DIVIDE:
            return PyLong_FromLong(floor(res / val_c));
        case INPLACE_ADD:
            return PyLong_FromLong(res + val_c);
        case INPLACE_SUBTRACT:
            return PyLong_FromLong(res - val_c);
        case BINARY_MODULO:
        case BINARY_MATRIX_MULTIPLY:
        case INPLACE_MODULO:
        case INPLACE_POWER:
            UNSUPPORTED_MATH_OP(firstOp);
            return nullptr;
    }
    return nullptr;
}

PyObject* PyJitMath_TripleBinaryOpObjObjObj(PyObject* a, PyObject* b, PyObject* c, int firstOp, int secondOp) {
    PyObject* res;
    switch(firstOp) {
        case BINARY_TRUE_DIVIDE:
            res = PyNumber_TrueDivide(a, b);
            break;
        case BINARY_FLOOR_DIVIDE:
            res = PyNumber_FloorDivide(a, b);
            break;
        case BINARY_MODULO:
            UNSUPPORTED_MATH_OP(firstOp);
            return nullptr;
        case BINARY_MATRIX_MULTIPLY:
            res = PyNumber_MatrixMultiply(a, b);
            break;
        case BINARY_MULTIPLY:
            res = PyNumber_Multiply(a, b);
            break;
        case BINARY_SUBTRACT:
            res = PyNumber_Subtract(a, b);
            break;
        case BINARY_ADD:
            res = PyNumber_Add(a, b);
            break;
    }
    switch (secondOp){
        case BINARY_TRUE_DIVIDE:
            return PyNumber_TrueDivide(c, res);
        case BINARY_FLOOR_DIVIDE:
            return PyNumber_FloorDivide(c, res);
        case BINARY_POWER:
            return PyNumber_TrueDivide(c, res);
        case BINARY_MODULO:
            UNSUPPORTED_MATH_OP(firstOp);
            return nullptr;
        case BINARY_MATRIX_MULTIPLY:
            return PyNumber_MatrixMultiply(c, res);
        case BINARY_MULTIPLY:
            return PyNumber_Multiply(c, res);
        case BINARY_SUBTRACT:
            return PyNumber_Subtract(c, res);
        case BINARY_ADD:
            return PyNumber_Add(c, res);
        case INPLACE_POWER:
            UNSUPPORTED_MATH_OP(firstOp);
            return nullptr;
        case INPLACE_MULTIPLY:
            return PyNumber_InPlaceMultiply(c, res);
        case INPLACE_MATRIX_MULTIPLY:
            return PyNumber_InPlaceMatrixMultiply(c, res);
        case INPLACE_TRUE_DIVIDE:
            return PyNumber_InPlaceTrueDivide(c, res);
        case INPLACE_FLOOR_DIVIDE:
            return PyNumber_InPlaceFloorDivide(c, res);
        case INPLACE_MODULO:
            return PyNumber_TrueDivide(c, res);
        case INPLACE_ADD:
            return PyNumber_InPlaceAdd(c, res);
        case INPLACE_SUBTRACT:
            return PyNumber_InPlaceSubtract(c, res);
    }
    return nullptr;
}
