import statistics
from fractions import Fraction
import pytest


def test_floats():
    a = 2.0
    b = 3.0
    c = 4.0
    c += a * b
    assert c == 10.0
    c /= a + b
    assert c == 2.0
    c %= a % b
    assert c == 0.0


def test_ints():
    a = 2
    b = 3
    c = 4
    c += a * b
    assert c == 10
    c /= a + b
    assert c == 2.0
    c //= a + b
    assert c == 0
    c = 4
    c %= a % b
    assert c == 0


def test_mixed():
    a = 2
    b = 3.0
    c = 4
    c += a * b
    assert c == 10.0
    c /= a + b
    assert c == 2.0
    c = 4
    c %= a % b
    assert c == 0.0


def test_mixed2():
    a = 2.0
    b = 3
    c = 4
    c += a * b
    assert c == 10.0
    c /= a + b
    assert c == 2.0
    c = 4
    c %= a % b
    assert c == 0.0


def test_mixed3():
    a = 2
    b = 3
    c = 4.0
    c += a * b
    assert c == 10.0
    c /= a + b
    assert c == 2.0
    c = 4.0
    c %= a % b
    assert c == 0.0


def test_mixed4():
    a = 2
    b = 3.0
    c = 4.0
    c += a * b
    assert c == 10.0
    c /= a + b
    assert c == 2.0
    c = 4.0
    c %= a % b
    assert c == 0.0

    i = -10
    x = 1234567890.0 * (10.0 ** i)
    assert x == 0.12345678900000001
    i = 0
    x = 1234567890.0 * (10.0 ** i)
    assert x == 1234567890.0
    i = 10
    x = 1234567890.0 * (10.0 ** i)
    assert x == 1.23456789e+19


def test_mixed5():
    a = 2.0
    b = 3
    c = 4.0
    c += a * b
    assert c == 10.0
    c /= a + b
    assert c == 2.0
    c %= a + b
    assert c == 2


def test_mixed6():
    a = 2.0
    b = 3.0
    c = 4
    c += a * b
    assert c == 10.0
    c /= a + b
    assert c == 2.0
    c %= a % b
    assert c == 0.0


def test_modulo():
    a = 1
    b = 2
    c = "boo %s"
    x = c % (a + b)
    assert x == "boo 3"


def test_mean():
    answer = statistics.mean([1, 2, 3, 4, 4])
    assert answer == 2.8
    answer = statistics.mean([1, 2, 3, 4, 4])
    assert answer == 2.8


@pytest.mark.skip(reason='TODO: Fix variance overflows.')
def test_variance():
    data = [0, 0, 1]
    result = statistics.variance(data)
    assert result == 0.33333333333333337


def test_variance2():
    data = [0, 1]
    result = statistics.variance(data)
    assert result == 0.5


@pytest.mark.skip(reason='TODO: Fix variance overflows.')
def test_variance_slow():
    data = [0, 0, 1]
    c = statistics.mean(data)
    assert c == 0.3333333333333333
    T, total, count = statistics._sum((x - c) ** 2 for x in data)
    assert T == float
    assert total == Fraction(3002399751580331, 4503599627370496)


def test_fraction_operations():
    result1 = Fraction(2, 3) + Fraction(7, 5)
    assert result1 == Fraction(31, 15)
    result2 = Fraction(2, 3) - Fraction(7, 5)
    assert result2 == Fraction(-11, 15)
    result3 = Fraction(2, 3) * Fraction(7, 5)
    assert result3 == Fraction(14, 15)
    result4 = Fraction(2, 3) ** Fraction(7, 5)
    assert result4 == 0.5668553336114626
