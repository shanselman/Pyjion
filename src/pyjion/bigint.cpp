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

#include "bigint.h"
#include "intrins.h"

PyjionBigInt* PyjionBigInt_FromInt64(int64_t value, PyjionBigIntRegister* bigIntRegister) {
    return bigIntRegister->addShort(value);
}

PyjionBigInt* PyjionBigInt_FromPyLong(PyObject* pythonObject, PyjionBigIntRegister* bigIntRegister) {
    PyjionBigInt* result;
    if (!IntegerValue::isBig(pythonObject)) {
        return bigIntRegister->addShort(PyLong_AsLongLong(pythonObject));
    } else {
        auto size = Py_ABS(Py_SIZE(pythonObject));
        result = bigIntRegister->addLong(size);
        result->negative = (Py_SIZE(pythonObject) < 0);
        for (size_t idx = 0; idx < size; idx++) {
            result->digits[idx] = ((PyLongObject*) pythonObject)->ob_digit[idx];
        }
    }
    return result;
}

#define NB_OP(op) \
    PyjionBigInt* result = nullptr; \
    PyObject *leftTmp, *rightTmp, *pyResult; \
    if (left->isShort && !right->isShort) { \
        leftTmp = PyLong_FromLongLong(left->shortVersion); \
        rightTmp = PyjionBigInt_AsPyLong(right); \
    } else if (!left->isShort && right->isShort) { \
        leftTmp = PyjionBigInt_AsPyLong(left); \
        rightTmp = PyLong_FromLongLong(right->shortVersion); \
    } else { \
        leftTmp = PyjionBigInt_AsPyLong(left); \
        rightTmp = PyjionBigInt_AsPyLong(right); \
    } \
    pyResult = PyLong_Type.tp_as_number->op(leftTmp, rightTmp);\
    result = PyjionBigInt_FromPyLong(pyResult, bigIntRegister);\
    Py_XDECREF(pyResult);\
    Py_XDECREF(leftTmp);\
    Py_XDECREF(rightTmp);\
    return result;\

#define NB_OP_LEFT(op) \
    PyObject *leftTmp, *rightTmp, *pyResult; \
    PyjionBigInt* result = nullptr; \
    leftTmp = PyLong_FromLongLong(left); \
    rightTmp = PyjionBigInt_AsPyLong(right); \
    pyResult = PyLong_Type.tp_as_number->op(leftTmp, rightTmp); \
    result = PyjionBigInt_FromPyLong(pyResult, bigIntRegister); \
    Py_XDECREF(pyResult); \
    Py_XDECREF(leftTmp); \
    Py_XDECREF(rightTmp); \
    return result;     \

#define NB_OP_RIGHT(op) \
    PyObject *leftTmp, *rightTmp, *pyResult; \
    PyjionBigInt* result = nullptr; \
    rightTmp = PyLong_FromLongLong(right); \
    leftTmp = PyjionBigInt_AsPyLong(left); \
    pyResult = PyLong_Type.tp_as_number->op(leftTmp, rightTmp); \
    result = PyjionBigInt_FromPyLong(pyResult, bigIntRegister); \
    Py_XDECREF(pyResult); \
    Py_XDECREF(leftTmp); \
    Py_XDECREF(rightTmp); \
    return result; \

PyjionBigInt* PyjionBigInt_Add(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister) {
    if (left->isShort && right->isShort) {
        return PyjionBigInt_FromInt64(left->shortVersion + right->shortVersion, bigIntRegister);
    }
    NB_OP(nb_add);
}

PyjionBigInt* PyjionBigInt_AddInt64Left(int64_t left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister){
    if(right->isShort){
        return PyjionBigInt_FromInt64(left + right->shortVersion, bigIntRegister);
    } else {
        NB_OP_LEFT(nb_add);
    }
}

PyjionBigInt* PyjionBigInt_AddInt64Right(PyjionBigInt* left, int64_t right, PyjionBigIntRegister* bigIntRegister){
    if(left->isShort){
        return PyjionBigInt_FromInt64(left->shortVersion + right, bigIntRegister);
    } else {
        NB_OP_RIGHT(nb_add);
    }
}

PyjionBigInt* PyjionBigInt_Sub(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister) {
    if (left->isShort && right->isShort) {
        return PyjionBigInt_FromInt64(left->shortVersion - right->shortVersion, bigIntRegister);
    }
    NB_OP(nb_subtract);
}

PyjionBigInt* PyjionBigInt_SubInt64Left(int64_t left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister){
    if(right->isShort){
        return PyjionBigInt_FromInt64(left - right->shortVersion, bigIntRegister);
    } else {
        NB_OP_LEFT(nb_subtract);
    }
}

PyjionBigInt* PyjionBigInt_SubInt64Right(PyjionBigInt* left, int64_t right, PyjionBigIntRegister* bigIntRegister){
    if(left->isShort){
        return PyjionBigInt_FromInt64(left->shortVersion - right, bigIntRegister);
    } else {
        NB_OP_RIGHT(nb_subtract);
    }
}

double PyjionBigInt_TrueDivide(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister) {
    if (left->isShort && right->isShort) {
        return PyJit_LongTrueDivide(left->shortVersion, right->shortVersion);
    }

    double result = 0.;
    PyObject *leftTmp, *rightTmp, *pyResult;
    if (left->isShort && !right->isShort) {
        leftTmp = PyLong_FromLongLong(left->shortVersion);
        rightTmp = PyjionBigInt_AsPyLong(right);
    } else if (!left->isShort && right->isShort) {
        leftTmp = PyjionBigInt_AsPyLong(left);
        rightTmp = PyLong_FromLongLong(right->shortVersion);
    } else {
        leftTmp = PyjionBigInt_AsPyLong(left);
        rightTmp = PyjionBigInt_AsPyLong(right);
    }
    pyResult = PyLong_Type.tp_as_number->nb_true_divide(leftTmp, rightTmp);
    result = PyFloat_AsDouble(pyResult);
    Py_XDECREF(pyResult);
    Py_XDECREF(leftTmp);
    Py_XDECREF(rightTmp);
    return result;
}

double PyjionBigInt_TrueDivideInt64Left(int64_t left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister) {
    if (right->isShort) {
        return PyJit_LongTrueDivide(left, right->shortVersion);
    }

    double result = 0.;
    PyObject *leftTmp, *rightTmp, *pyResult;
    leftTmp = PyLong_FromLongLong(left);
    rightTmp = PyjionBigInt_AsPyLong(right);
    pyResult = PyLong_Type.tp_as_number->nb_true_divide(leftTmp, rightTmp);
    result = PyFloat_AsDouble(pyResult);
    Py_XDECREF(pyResult);
    Py_XDECREF(leftTmp);
    Py_XDECREF(rightTmp);
    return result;
}

double PyjionBigInt_TrueDivideInt64Right(PyjionBigInt* left, int64_t right, PyjionBigIntRegister* bigIntRegister) {
    if (left->isShort) {
        return PyJit_LongTrueDivide(left->shortVersion, right);
    }

    double result = 0.;
    PyObject *leftTmp, *rightTmp, *pyResult;
    leftTmp = PyjionBigInt_AsPyLong(left);
    rightTmp = PyLong_FromLongLong(right);
    pyResult = PyLong_Type.tp_as_number->nb_true_divide(leftTmp, rightTmp);
    result = PyFloat_AsDouble(pyResult);
    Py_XDECREF(pyResult);
    Py_XDECREF(leftTmp);
    Py_XDECREF(rightTmp);
    return result;
}

PyjionBigInt* PyjionBigInt_Mod(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister) {
    if (left->isShort && right->isShort) {
        return PyjionBigInt_FromInt64(PyJit_LongMod(left->shortVersion, right->shortVersion), bigIntRegister);
    }
    NB_OP(nb_remainder);
}

PyjionBigInt* PyjionBigInt_ModInt64Left(int64_t left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister){
    if(right->isShort){
        return PyjionBigInt_FromInt64(PyJit_LongMod(left, right->shortVersion), bigIntRegister);
    } else {
        NB_OP_LEFT(nb_remainder);
    }
}

PyjionBigInt* PyjionBigInt_ModInt64Right(PyjionBigInt* left, int64_t right, PyjionBigIntRegister* bigIntRegister){
    if(left->isShort){
        return PyjionBigInt_FromInt64(PyJit_LongMod(left->shortVersion, right), bigIntRegister);
    } else {
        NB_OP_RIGHT(nb_remainder);
    }
}

PyjionBigInt* PyjionBigInt_Multiply(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister) {
    if (left->isShort && right->isShort) {
        auto x = left->shortVersion * right->shortVersion;
        if (left->shortVersion != 0 && x / left->shortVersion != right->shortVersion) {
            // overflow handling
            auto leftTmp = PyLong_FromLongLong(left->shortVersion);
            auto rightTmp = PyLong_FromLongLong(right->shortVersion);
            auto resultTmp = PyLong_Type.tp_as_number->nb_multiply(leftTmp, rightTmp);
            auto result = PyjionBigInt_FromPyLong(resultTmp, bigIntRegister);
            Py_XDECREF(resultTmp);
            Py_XDECREF(leftTmp);
            Py_XDECREF(rightTmp);
            return result;
        } else {
            return PyjionBigInt_FromInt64(x, bigIntRegister);
        }
    }
    NB_OP(nb_multiply);
}

PyjionBigInt* PyjionBigInt_MultiplyInt64Left(int64_t left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister) {
    if (right->isShort) {
        auto x = left * right->shortVersion;
        if (left != 0 && x / left != right->shortVersion) {
            // overflow handling
            auto leftTmp = PyLong_FromLongLong(left);
            auto rightTmp = PyLong_FromLongLong(right->shortVersion);
            auto resultTmp = PyLong_Type.tp_as_number->nb_multiply(leftTmp, rightTmp);
            auto result = PyjionBigInt_FromPyLong(resultTmp, bigIntRegister);
            Py_XDECREF(resultTmp);
            Py_XDECREF(leftTmp);
            Py_XDECREF(rightTmp);
            return result;
        } else {
            return PyjionBigInt_FromInt64(x, bigIntRegister);
        }
    }
    NB_OP_LEFT(nb_multiply);
}

PyjionBigInt* PyjionBigInt_MultiplyInt64Right( PyjionBigInt* left, int64_t right, PyjionBigIntRegister* bigIntRegister) {
    if (left->isShort) {
        auto x = left->shortVersion * right;
        if (left->shortVersion != 0 && x / left->shortVersion != right) {
            // overflow handling
            auto leftTmp = PyLong_FromLongLong(left->shortVersion);
            auto rightTmp = PyLong_FromLongLong(right);
            auto resultTmp = PyLong_Type.tp_as_number->nb_multiply(leftTmp, rightTmp);
            auto result = PyjionBigInt_FromPyLong(resultTmp, bigIntRegister);
            Py_XDECREF(resultTmp);
            Py_XDECREF(leftTmp);
            Py_XDECREF(rightTmp);
            return result;
        } else {
            return PyjionBigInt_FromInt64(x, bigIntRegister);
        }
    }
    NB_OP_RIGHT(nb_multiply);
}

PyjionBigInt* PyjionBigInt_Power(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister) {
    if (left->isShort && right->isShort) {
        auto longResult = PyJit_LongPow(left->shortVersion, right->shortVersion);
        if (left->shortVersion != 0 && longResult != 0)
            return PyjionBigInt_FromInt64(longResult, bigIntRegister);
        else {
            // overflow handling
            auto leftTmp = PyLong_FromLongLong(left->shortVersion);
            auto rightTmp = PyLong_FromLongLong(right->shortVersion);
            auto resultTmp = PyLong_Type.tp_as_number->nb_power(leftTmp, rightTmp, Py_None);
            auto result = PyjionBigInt_FromPyLong(resultTmp, bigIntRegister);
            Py_XDECREF(resultTmp);
            Py_XDECREF(leftTmp);
            Py_XDECREF(rightTmp);
            return result;
        }
    }

    PyObject *leftTmp, *rightTmp, *pyResult;
    if (left->isShort && !right->isShort) {
        leftTmp = PyLong_FromLongLong(left->shortVersion);
        rightTmp = PyjionBigInt_AsPyLong(right);
    } else if (!left->isShort && right->isShort) {
        leftTmp = PyjionBigInt_AsPyLong(left);
        rightTmp = PyLong_FromLongLong(right->shortVersion);
    } else {
        leftTmp = PyjionBigInt_AsPyLong(left);
        rightTmp = PyjionBigInt_AsPyLong(right);
    }
    pyResult = PyLong_Type.tp_as_number->nb_power(leftTmp, rightTmp, Py_None);
    auto result = PyjionBigInt_FromPyLong(pyResult, bigIntRegister);
    Py_XDECREF(pyResult);
    Py_XDECREF(leftTmp);
    Py_XDECREF(rightTmp);
    return result;
}

PyjionBigInt* PyjionBigInt_PowerInt64Left(int64_t left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister) {
    if (right->isShort) {
        auto longResult = PyJit_LongPow(left, right->shortVersion);
        if (left != 0 && longResult != 0)
            return PyjionBigInt_FromInt64(longResult, bigIntRegister);
        else {
            // overflow handling
            auto leftTmp = PyLong_FromLongLong(left);
            auto rightTmp = PyLong_FromLongLong(right->shortVersion);
            auto resultTmp = PyLong_Type.tp_as_number->nb_power(leftTmp, rightTmp, Py_None);
            auto result = PyjionBigInt_FromPyLong(resultTmp, bigIntRegister);
            Py_XDECREF(resultTmp);
            Py_XDECREF(leftTmp);
            Py_XDECREF(rightTmp);
            return result;
        }
    }

    PyObject *leftTmp, *rightTmp, *pyResult;
    leftTmp = PyLong_FromLongLong(left);
    rightTmp = PyjionBigInt_AsPyLong(right);
    pyResult = PyLong_Type.tp_as_number->nb_power(leftTmp, rightTmp, Py_None);
    auto result = PyjionBigInt_FromPyLong(pyResult, bigIntRegister);
    Py_XDECREF(pyResult);
    Py_XDECREF(leftTmp);
    Py_XDECREF(rightTmp);
    return result;
}

PyjionBigInt* PyjionBigInt_PowerInt64Right(PyjionBigInt* left, int64_t right, PyjionBigIntRegister* bigIntRegister) {
    if (left->isShort) {
        auto longResult = PyJit_LongPow(left->shortVersion, right);
        if (left->shortVersion != 0 && longResult != 0)
            return PyjionBigInt_FromInt64(longResult, bigIntRegister);
        else {
            // overflow handling
            auto leftTmp = PyLong_FromLongLong(left->shortVersion);
            auto rightTmp = PyLong_FromLongLong(right);
            auto resultTmp = PyLong_Type.tp_as_number->nb_power(leftTmp, rightTmp, Py_None);
            auto result = PyjionBigInt_FromPyLong(resultTmp, bigIntRegister);
            Py_XDECREF(resultTmp);
            Py_XDECREF(leftTmp);
            Py_XDECREF(rightTmp);
            return result;
        }
    }

    PyObject *leftTmp, *rightTmp, *pyResult;
    leftTmp = PyjionBigInt_AsPyLong(left);
    rightTmp = PyLong_FromLongLong(right);
    pyResult = PyLong_Type.tp_as_number->nb_power(leftTmp, rightTmp, Py_None);
    auto result = PyjionBigInt_FromPyLong(pyResult, bigIntRegister);
    Py_XDECREF(pyResult);
    Py_XDECREF(leftTmp);
    Py_XDECREF(rightTmp);
    return result;
}

PyjionBigInt* PyjionBigInt_FloorDivide(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister) {
    if (left->isShort && right->isShort) {
        return PyjionBigInt_FromInt64(PyJit_LongFloorDivide(left->shortVersion, right->shortVersion), bigIntRegister);
    }

    NB_OP(nb_floor_divide);
}

PyjionBigInt* PyjionBigInt_FloorDivideInt64Left(int64_t left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister){
    if(right->isShort){
        return PyjionBigInt_FromInt64(PyJit_LongFloorDivide(left, right->shortVersion), bigIntRegister);
    } else {
        NB_OP_LEFT(nb_floor_divide);
    }
}

PyjionBigInt* PyjionBigInt_FloorDivideInt64Right(PyjionBigInt* left, int64_t right, PyjionBigIntRegister* bigIntRegister){
    if(left->isShort){
        return PyjionBigInt_FromInt64(PyJit_LongFloorDivide(left->shortVersion, right), bigIntRegister);
    } else {
        NB_OP_RIGHT(nb_floor_divide);
    }
}

PyObject* PyjionBigInt_AsPyLong(PyjionBigInt* i) {
    PyObject* result = nullptr;
    if (i->isShort)
        result = PyLong_FromLongLong(i->shortVersion);
    else {
        auto l = _PyLong_New(i->numDigits);
        for (size_t idx = 0; idx < i->numDigits; idx++) {
            l->ob_digit[idx] = i->digits[idx];
        }
        if (i->negative) {
            Py_SET_SIZE(l, -i->numDigits);
        }
        result = reinterpret_cast<PyObject*>(l);
    }
    return result;
}

int64_t PyjionBigInt::asLong() {
    if (isShort)
        return shortVersion;
    else
        return PyLong_AsLongLong(PyjionBigInt_AsPyLong(this));
}

int32_t PyjionBigInt_RichCompare(PyjionBigInt* left, PyjionBigInt* right, uint32_t op) {
    int result = -1;
    PyObject *leftTmp = nullptr, *rightTmp = nullptr;

    if (left->isShort && right->isShort)
        goto shortCompare;

    if (left->isShort && !right->isShort) {
        leftTmp = PyLong_FromLongLong(left->shortVersion);
        rightTmp = PyjionBigInt_AsPyLong(right);
    } else if (!left->isShort && right->isShort) {
        leftTmp = PyjionBigInt_AsPyLong(left);
        rightTmp = PyLong_FromLongLong(right->shortVersion);
    } else {
        leftTmp = PyjionBigInt_AsPyLong(left);
        rightTmp = PyjionBigInt_AsPyLong(right);
    }
    result = PyLong_Type.tp_richcompare(leftTmp, rightTmp, op) == Py_True ? 1 : 0;
    Py_XDECREF(leftTmp);
    Py_XDECREF(rightTmp);
    goto cleanup;

shortCompare:
    switch (op) {
        case Py_EQ:
            result = left->shortVersion == right->shortVersion;
            break;
        case Py_NE:
            result = left->shortVersion != right->shortVersion;
            break;
        case Py_GE:
            result = left->shortVersion >= right->shortVersion;
            break;
        case Py_LE:
            result = left->shortVersion <= right->shortVersion;
            break;
        case Py_LT:
            result = left->shortVersion < right->shortVersion;
            break;
        case Py_GT:
            result = left->shortVersion > right->shortVersion;
            break;
        default:
            result = -1;
    }

cleanup:
    return result;
}

int32_t PyjionBigInt_RichCompareInt64Left(int64_t left, PyjionBigInt* right, uint32_t op) {
    int result = -1;
    PyObject *leftTmp = nullptr, *rightTmp = nullptr;

    if (!right->isShort) {
        leftTmp = PyLong_FromLongLong(left);
        rightTmp = PyjionBigInt_AsPyLong(right);
        result = PyLong_Type.tp_richcompare(leftTmp, rightTmp, op) == Py_True ? 1 : 0;
        Py_XDECREF(leftTmp);
        Py_XDECREF(rightTmp);
        return result;
    } else {
        switch (op) {
            case Py_EQ:
                result = left == right->shortVersion;
                break;
            case Py_NE:
                result = left != right->shortVersion;
                break;
            case Py_GE:
                result = left >= right->shortVersion;
                break;
            case Py_LE:
                result = left <= right->shortVersion;
                break;
            case Py_LT:
                result = left < right->shortVersion;
                break;
            case Py_GT:
                result = left > right->shortVersion;
                break;
            default:
                result = -1;
        }
        return result;
    }
}

int32_t PyjionBigInt_RichCompareInt64Right(PyjionBigInt* left, int64_t right, uint32_t op){
    int result = -1;
    PyObject *leftTmp = nullptr, *rightTmp = nullptr;

    if (!left->isShort) {
        leftTmp = PyjionBigInt_AsPyLong(left);
        rightTmp = PyLong_FromLongLong(right);
        result = PyLong_Type.tp_richcompare(leftTmp, rightTmp, op) == Py_True ? 1 : 0;
        Py_XDECREF(leftTmp);
        Py_XDECREF(rightTmp);
        return result;
    } else {
        switch (op) {
            case Py_EQ:
                result = left->shortVersion == right;
                break;
            case Py_NE:
                result = left->shortVersion != right;
                break;
            case Py_GE:
                result = left->shortVersion >= right;
                break;
            case Py_LE:
                result = left->shortVersion <= right;
                break;
            case Py_LT:
                result = left->shortVersion < right;
                break;
            case Py_GT:
                result = left->shortVersion > right;
                break;
            default:
                result = -1;
        }
        return result;
    }
}

double PyjionBigInt_AsDouble(PyjionBigInt*i) {
    if (i->isShort){
        return (double)i->shortVersion;
    } else {
        auto l = PyjionBigInt_AsPyLong(i);
        double result = PyLong_AsDouble(l);
        Py_XDECREF(l);
        return result;
    }
}
