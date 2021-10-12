import pyjion
import timeit
from statistics import fmean


def test_floats(n=10000):
    for y in range(n):
        x = 0.1
        z = y * y + x - y
        x *= z


def test_ints(n=10000):
    for y in range(n):
        x = 2
        z = y * y + x - y
        x *= z


__benchmarks__ = [(test_floats, "floatmath_micro", {"level": 2}), (test_ints, "intmath_micro", {"level": 2})]