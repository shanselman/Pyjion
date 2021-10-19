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


TEST_CASE("Test builtins") {
    SECTION("call print()") {
        auto t = EmissionTest("def f(): return print('hello world')");
        CHECK(t.returns() == "None");
    }
    SECTION("call ord()") {
        auto t = EmissionTest("def f(): return ord('a')");
        CHECK(t.returns() == "97");
    }
    SECTION("call int()") {
        auto t = EmissionTest("def f(): return int('97')");
        CHECK(t.returns() == "97");
    }
    SECTION("call min()") {
        auto t = EmissionTest("def f(): return min([100, 101, 97])");
        CHECK(t.returns() == "97");
    }
    SECTION("call min() again") {
        auto t = EmissionTest("def f(): return min(100, 101, 97)");
        CHECK(t.returns() == "97");
    }
    SECTION("call type()") {
        auto t = EmissionTest("def f(): return type(2)");
        CHECK(t.returns() == "<class 'int'>");
    }
    SECTION("call type() more complicatedly?") {
        auto t = EmissionTest("def f(): return isinstance(type(2), type)");
        CHECK(t.returns() == "True");
    }
    SECTION("call max() and map()") {
        auto t = EmissionTest("def f(): args=('a', 'aaa', 'aaaaa'); return max(map(len, args))");
        CHECK(t.returns() == "5");
    }
    SECTION("call chr()") {
        auto t = EmissionTest("def f(): return chr(32) * 10");
        CHECK(t.returns() == "'          '");
    }
    SECTION("call bin()") {
        auto t = EmissionTest("def f(): return bin(2**6)");
        CHECK(t.returns() == "'0b1000000'");
    }
    SECTION("call bin() with a big number!") {
        auto t = EmissionTest("def f(): return bin(2**65)");
        CHECK(t.returns() == "'0b100000000000000000000000000000000000000000000000000000000000000000'");
    }
    SECTION("call reversed() ") {
        auto t = EmissionTest("def f(): return tuple(reversed((1,2,3,4)))");
        CHECK(t.returns() == "(4, 3, 2, 1)");
    }
    SECTION("call bytearray() ") {
        auto t = EmissionTest("def f(): return bytearray((1,2,3,4))");
        CHECK(t.returns() == "bytearray(b'\\x01\\x02\\x03\\x04')");
    }
    SECTION("call bytearray() with dud data ") {
        auto t = EmissionTest("def f(): return bytearray(None)");
        CHECK(t.raises() == PyExc_TypeError);
    }
}