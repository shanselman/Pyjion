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
#include "intrins.h"
#define MAXINT64 9223372036854775807
// Max int64 is 9_223_372_036_854_775_807

TEST_CASE("Test IntegerValue:isBig()"){
    SECTION("Test small values"){
        auto smallNumbers = GENERATE(0, 1, -1, 255, 10000, -10000, 10000000, -10000000);
        CHECK(!IntegerValue::isBig(PyLong_FromLongLong(smallNumbers)));
    }
    SECTION("Test big values"){
        auto big = GENERATE("9_223_372_036_854_775_807", "-9_223_372_036_854_775_807", "200200200200200200200200200", "-200200200200200200200200200");
        CHECK(IntegerValue::isBig(PyLong_FromUnicodeObject(PyUnicode_FromString(big), 10)));
    }
}

TEST_CASE("Test bigint math") {
    // +=, -= checks are to avoid constant folding
    SECTION("test addition of max int64 with int") {
        auto t = EmissionTest(
                "def f():\n    x = 9_223_372_036_854_775_807\n    return x + 1");
        CHECK(t.returns() == "9223372036854775808");
    }
    SECTION("test addition of max int64 with max int64") {
        auto t = EmissionTest(
                "def f():\n    x = 9_223_372_036_854_775_807\n    return x + 9_223_372_036_854_775_807");
        CHECK(t.returns() == "18446744073709551614");
    }
    SECTION("test addition of big int with big int") {
        auto t = EmissionTest(
                "def f():\n    x = 100_100_100_100_100_100_100_100_100\n    return x + 100_100_100_100_100_100_100_100_100");
        CHECK(t.returns() == "200200200200200200200200200");
    }
    SECTION("test addition of big int with int") {
        auto t = EmissionTest(
                "def f():\n    x = 100_100_100_100_100_100_100_100_100\n    return x + 100");
        CHECK(t.returns() == "100100100100100100100100200");
    }
    SECTION("test addition of -ve big int with int") {
        auto t = EmissionTest(
                "def f():\n    x = -100_100_100_100_100_100_100_100_100\n    return x + 100");
        CHECK(t.returns() == "-100100100100100100100100000");
    }
    SECTION("test subtraction of big with int") {
        auto t = EmissionTest(
                "def f():\n    x = 100_100_100_100_100_100_100_100_100\n    return x - 100");
        CHECK(t.returns() == "100100100100100100100100000");
    }
    SECTION("test power of 2 small ints into a big int") {
        auto t = EmissionTest(
                "def f():\n    x = 100\n    return x ** 100");
        CHECK(t.returns() == "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    }
    SECTION("test float multiply with  big int") {
        auto t = EmissionTest(
                "def f():\n    x = 100_100_100_100_100_100_100_100_100\n    return x * 100.");
        CHECK(t.returns() == "1.001001001001001e+28");
    }
    SECTION("test big int multiply with float") {
        auto t = EmissionTest(
                "def f():\n    x = 100.\n    return x * 100_100_100_100_100_100_100_100_100");
        CHECK(t.returns() == "1.001001001001001e+28");
    }
}

TEST_CASE("Test power function"){
    SECTION("Test overflow"){
        CHECK(PyJit_LongPow(2, 2) == 4);
        CHECK(PyJit_LongPow(2, 3) == 8);
        CHECK(PyJit_LongPow(2, 4) == 16);
        CHECK(PyJit_LongPow(2, 5) == 32);
        CHECK(PyJit_LongPow(2, 6) == 64);
        CHECK(PyJit_LongPow(2, 32) == 4294967296);
        CHECK(PyJit_LongPow(2, 63) == 9223372036854775808);
        CHECK(PyJit_LongPow(2, 65) == 0);
        CHECK(PyJit_LongPow(2, 65) == 0);
    }
}