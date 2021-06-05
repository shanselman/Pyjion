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

#include <catch2/catch.hpp>
#include "testing_util.h"
#include <Python.h>
#include <pyjit.h>


TEST_CASE("test BINARY PGC"){
    SECTION("test simple") {
        auto t = PgcProfilingTest(
                "def f():\n  a = 1\n  b = 2.0\n  c=3\n  return a + b + c\n"
        );
        CHECK(t.pgcStatus() == PgcStatus::Uncompiled);
        CHECK(t.returns() == "6.0");
        CHECK(t.pgcStatus() == PgcStatus::CompiledWithProbes);
        CHECK(t.profileEquals(16, 0, &PyFloat_Type)); // right
        CHECK(t.profileEquals(16, 1, &PyLong_Type)); // left
        CHECK(t.profileEquals(20, 0, &PyLong_Type)); // right
        //CHECK(t.profileEquals(20, 1, &PyFloat_Type)); // left
        CHECK(t.returns() == "6.0");
        CHECK(t.pgcStatus() == PgcStatus::Optimized);
    };

    SECTION("test consistent types") {
        auto t = PgcProfilingTest(
                "def f():\n"
                "  a = 1000\n"
                "  b = 2.0\n"
                "  c = 2000\n"
                "  d = 3.0\n"
                "  def add(left,right):\n"
                "     return left + right\n"
                "  v = add(a, b) + add(c, d) + add(a, b)\n"
                "  return v\n"
        );
        CHECK(t.pgcStatus() == PgcStatus::Uncompiled);
        CHECK(t.returns() == "4007.0");
        CHECK(t.pgcStatus() == PgcStatus::CompiledWithProbes);
        CHECK(t.returns() == "4007.0");
        CHECK(t.pgcStatus() == PgcStatus::Optimized);
    };

    SECTION("test changing types") {
        auto t = PgcProfilingTest(
                "def f():\n"
                "  a = 1000\n"
                "  b = 2.0\n"
                "  c = 'cheese'\n"
                "  d = ' shop'\n"
                "  def add(left,right):\n"
                "     return left + right\n"
                "  v = str(add(a, b)) + add(c, d)\n"
                "  return a,b,c,d\n"
        );
        CHECK(t.pgcStatus() == PgcStatus::Uncompiled);
        CHECK(t.raises() == PyExc_ValueError);
        CHECK(t.pgcStatus() == PgcStatus::CompiledWithProbes);
        CHECK(t.raises() == PyExc_ValueError);
        CHECK(t.pgcStatus() == PgcStatus::Optimized);
    };

    SECTION("test changing types for COMPARE_OP") {
        auto t = PgcProfilingTest(
                "def f():\n"
                "  a = 1000\n"
                "  b = 2.0\n"
                "  c = 'cheese'\n"
                "  d = ' shop'\n"
                "  def equal(left,right):\n"
                "     return left == right\n"
                "  return equal(a,b), equal (c,d), equal(a, d)\n"
        );
        CHECK(t.pgcStatus() == PgcStatus::Uncompiled);
        CHECK(t.raises() == PyExc_ValueError);
        CHECK(t.pgcStatus() == PgcStatus::CompiledWithProbes);
        CHECK(t.raises() == PyExc_ValueError);
        CHECK(t.pgcStatus() == PgcStatus::Optimized);
    };
}

TEST_CASE("test UNPACK_SEQUENCE PGC") {
    SECTION("test simple") {
        auto t = PgcProfilingTest(
                "def f():\n  a, b, c = ['a', 'b', 'c']\n  return a, b, c"
        );
        CHECK(t.pgcStatus() == PgcStatus::Uncompiled);
        CHECK(t.returns() == "('a', 'b', 'c')");
        CHECK(t.pgcStatus() == PgcStatus::CompiledWithProbes);
        CHECK(t.profileEquals(6, 0, &PyList_Type));
        CHECK(t.returns() == "('a', 'b', 'c')");
        CHECK(t.pgcStatus() == PgcStatus::Optimized);
    };
    SECTION("test for_iter stacked"){
        auto t = PgcProfilingTest(
                "def f():\n"
                "  x = [(1,2), (3,4)]\n"
                "  results = []\n"
                "  for i, j in x:\n"
                "    results.append(i); results.append(j)\n"
                "  return results\n"
        );
        CHECK(t.pgcStatus() == PgcStatus::Uncompiled);
        CHECK(t.returns() == "[1, 2, 3, 4]");
        CHECK(t.pgcStatus() == PgcStatus::CompiledWithProbes);
        CHECK(t.profileEquals(18, 0, &PyTuple_Type));
        CHECK(t.returns() == "[1, 2, 3, 4]");
        CHECK(t.pgcStatus() == PgcStatus::Optimized);
    }

    SECTION("test changed types"){
        auto t = PgcProfilingTest(
                "def f():\n"
                "  results = []\n"
                "  def x(it):\n"
                "    a, b = it\n"
                "    return int(a) + int(b)\n"
                "  return x((1,2)) + x([3, 4]) + x('56')\n"
        );
        CHECK(t.pgcStatus() == PgcStatus::Uncompiled);
        CHECK(t.returns() == "21");
        CHECK(t.pgcStatus() == PgcStatus::CompiledWithProbes);
        CHECK(t.returns() == "21");
        CHECK(t.pgcStatus() == PgcStatus::Optimized);
    }
}


TEST_CASE("test CALL_FUNCTION PGC") {
    SECTION("test callable type object") {
        auto t = PgcProfilingTest(
                "def f():\n  return int('2000')"
        );
        CHECK(t.pgcStatus() == PgcStatus::Uncompiled);
        CHECK(t.returns() == "2000");
        CHECK(t.pgcStatus() == PgcStatus::CompiledWithProbes);
        CHECK(t.profileEquals(4, 0, &PyUnicode_Type));
        CHECK(t.profileEquals(4, 1, &PyType_Type));
        CHECK(t.returns() == "2000");
        CHECK(t.pgcStatus() == PgcStatus::Optimized);
    };

    SECTION("test builtin function") {
        auto t = PgcProfilingTest(
                "def f():\n  return len('2000')"
        );
        CHECK(t.pgcStatus() == PgcStatus::Uncompiled);
        CHECK(t.returns() == "4");
        CHECK(t.pgcStatus() == PgcStatus::CompiledWithProbes);
        CHECK(t.profileEquals(4, 0, &PyUnicode_Type));
        CHECK(t.profileEquals(4, 1, &PyCFunction_Type));
        CHECK(t.returns() == "4");
        CHECK(t.pgcStatus() == PgcStatus::Optimized);
    };

    SECTION("test python function") {
        auto t = PgcProfilingTest(
                "def f():\n"
                "  def half(x):\n"
                "     return x/2\n"
                "  return half(2000)"
        );
        CHECK(t.pgcStatus() == PgcStatus::Uncompiled);
        CHECK(t.returns() == "1000.0");
        CHECK(t.pgcStatus() == PgcStatus::CompiledWithProbes);
        CHECK(t.profileEquals(12, 0, &PyLong_Type));
        CHECK(t.profileEquals(12, 1, &PyFunction_Type));
        CHECK(t.returns() == "1000.0");
        CHECK(t.pgcStatus() == PgcStatus::Optimized);
    };

    SECTION("test changing callable") {
        auto t = PgcProfilingTest(
                "def f():\n"
                "  def half(x):\n"
                "     return x/2\n"
                "  def result_of(x, a):\n"
                "     return x(a)\n"
                "  r1 = result_of(len, 'hello')\n"
                "  result_of(len, 'hello')\n"
                "  r2 = result_of(float, 1000)\n"
                "  return r1, r2"
        );
        CHECK(t.pgcStatus() == PgcStatus::Uncompiled);
        CHECK(t.returns() == "(5, 1000.0)");
        CHECK(t.pgcStatus() == PgcStatus::CompiledWithProbes);
        CHECK(t.returns() == "(5, 1000.0)");
        CHECK(t.pgcStatus() == PgcStatus::Optimized);
        CHECK(t.returns() == "(5, 1000.0)");
        CHECK(t.pgcStatus() == PgcStatus::Optimized);
    };
}

TEST_CASE("test STORE_SUBSCR PGC") {
    SECTION("test list index") {
        auto t = PgcProfilingTest(
                "def f():\n"
                "  text = list('hello')\n"
                "  text[0] = 'H'\n"
                "  return text"
        );
        CHECK(t.pgcStatus() == PgcStatus::Uncompiled);
        CHECK(t.returns() == "['H', 'e', 'l', 'l', 'o']");
        CHECK(t.pgcStatus() == PgcStatus::CompiledWithProbes);
        CHECK(t.profileEquals(4, 0, &PyUnicode_Type));
        CHECK(t.profileEquals(4, 1, &PyType_Type));
        CHECK(t.profileEquals(14, 2, &PyUnicode_Type));
        CHECK(t.profileEquals(14, 1, &PyList_Type));
        CHECK(t.profileEquals(14, 0, &PyLong_Type));
        CHECK(t.returns() == "['H', 'e', 'l', 'l', 'o']");
        CHECK(t.pgcStatus() == PgcStatus::Optimized);
    };

    SECTION("test inplace operation") {
        auto t = PgcProfilingTest(
                "def f():\n"
                "  text = [0,1,2,3,4]\n"
                "  text[0] += 2\n"
                "  return text"
        );
        CHECK(t.pgcStatus() == PgcStatus::Uncompiled);
        CHECK(t.returns() == "[2, 1, 2, 3, 4]");
        CHECK(t.pgcStatus() == PgcStatus::CompiledWithProbes);
        CHECK(t.returns() == "[2, 1, 2, 3, 4]");
        CHECK(t.pgcStatus() == PgcStatus::Optimized);
    };

    SECTION("test complex inplace operation") {
        auto t = PgcProfilingTest(
                "def f():\n"
                "  text = [0,1,2,3,4]\n"
                "  n = 2\n"
                "  text[0] += 2 ** n\n"
                "  return text"
        );
        CHECK(t.pgcStatus() == PgcStatus::Uncompiled);
        CHECK(t.returns() == "[4, 1, 2, 3, 4]");
        CHECK(t.pgcStatus() == PgcStatus::CompiledWithProbes);
        CHECK(t.returns() == "[4, 1, 2, 3, 4]");
        CHECK(t.pgcStatus() == PgcStatus::Optimized);
    };

    SECTION("test complex inplace operation with floats") {
        auto t = PgcProfilingTest(
                "def f():\n"
                "  text = [0.1,1.32,2.4,3.55,4.5]\n"
                "  n = 2.00\n"
                "  text[0] += 2. ** n\n"
                "  return text"
        );
        CHECK(t.pgcStatus() == PgcStatus::Uncompiled);
        CHECK(t.returns() == "[4.1, 1.32, 2.4, 3.55, 4.5]");
        CHECK(t.pgcStatus() == PgcStatus::CompiledWithProbes);
        CHECK(t.returns() == "[4.1, 1.32, 2.4, 3.55, 4.5]");
        CHECK(t.pgcStatus() == PgcStatus::Optimized);
    };

    SECTION("test known builtin return type compare_op") {
        auto t = PgcProfilingTest(
                "def f():\n"
                "  test = [0.1,1.32,2.4,3.55,4.5]\n"
                "  if len(test) > 3:\n"
                "    return True\n"
                "  else:\n"
                "    return False\n"
        );
        CHECK(t.pgcStatus() == PgcStatus::Uncompiled);
        CHECK(t.returns() == "True");
        CHECK(t.pgcStatus() == PgcStatus::CompiledWithProbes);
        CHECK(t.returns() == "True");
        CHECK(t.pgcStatus() == PgcStatus::Optimized);
    };
}