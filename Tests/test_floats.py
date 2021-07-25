import sys
INF = float("inf")
NAN = float("nan")


def test_binary_add():
    a = 4.0
    b = 2.5
    c = -33.0099

    # TEST BINARY_ADD
    assert a + b == 6.5
    assert a + b == b + a

def test_binary_mul():
    a = 4.0
    b = 2.5
    c = -33.0099
    d = 10.000000000001
    e = 1.099999922
    assert d * e == 10.9999992200011
    assert d * e * d * e == 120.99998284002483
    assert a * b == 10.0

def test_binary_floor_divide():
    a = 4.0
    b = 2.5
    c = -33.0099
    assert a // b == 1.0

def test_binary_true_divide():
    a = 4.0
    b = 2.5
    c = -33.0099
    assert a / b == 1.6

def test_binary_subtract():
    a = 4.0
    b = 2.5
    c = -33.0099
    assert a - b == 1.5
    assert a - b == -(b - a)

def test_binary_mod():
    a = 4.0
    b = 2.5
    # TEST BINARY_MOD
    assert a % b == 1.5
    assert b % a == 2.5



def test_format_specials():
    # Test formatting of nans and infs.
    initial_ref = sys.getrefcount(INF)
    initial_nan_ref = sys.getrefcount(NAN)

    def test(fmt, value, expected):
        # Test with both % and format().
        assert fmt % value == expected, fmt
        fmt = fmt[1:] # strip off the %
        assert format(value, fmt) == expected, fmt

    for fmt in ['%e', '%f', '%g', '%.0e', '%.6f', '%.20g',
                '%#e', '%#f', '%#g', '%#.20e', '%#.15f', '%#.3g']:
        fmt_ref = sys.getrefcount(fmt)

        pfmt = '%+' + fmt[1:]
        assert sys.getrefcount(fmt) == fmt_ref, "format ref leak on slice 1"
        pfmt_ref = sys.getrefcount(pfmt)
        sfmt = '% ' + fmt[1:]
        sfmt_ref = sys.getrefcount(sfmt)
        assert sys.getrefcount(fmt) == fmt_ref, "format ref leak on slice 2"
        test(fmt, INF, 'inf')
        assert sys.getrefcount(INF) == initial_ref, "INF ref leak on first call"
        assert sys.getrefcount(fmt) == fmt_ref, "format ref leak on first call"
        test(fmt, -INF, '-inf')
        assert sys.getrefcount(INF) == initial_ref
        assert sys.getrefcount(fmt) == fmt_ref
        test(fmt, NAN, 'nan')
        assert sys.getrefcount(NAN) == initial_nan_ref
        assert sys.getrefcount(fmt) == fmt_ref
        test(fmt, -NAN, 'nan')
        assert sys.getrefcount(NAN) == initial_nan_ref
        assert sys.getrefcount(fmt) == fmt_ref
        # When asking for a sign, it's always provided. nans are
        #  always positive.
        test(pfmt, INF, '+inf')
        assert sys.getrefcount(INF) == initial_ref
        assert sys.getrefcount(pfmt) == pfmt_ref
        test(pfmt, -INF, '-inf')
        assert sys.getrefcount(INF) == initial_ref
        assert sys.getrefcount(pfmt) == pfmt_ref
        test(pfmt, NAN, '+nan')
        assert sys.getrefcount(NAN) == initial_nan_ref
        assert sys.getrefcount(pfmt) == pfmt_ref
        test(pfmt, -NAN, '+nan')
        assert sys.getrefcount(NAN) == initial_nan_ref
        assert sys.getrefcount(pfmt) == pfmt_ref
        # When using ' ' for a sign code, only infs can be negative.
        #  Others have a space.
        test(sfmt, INF, ' inf')
        assert sys.getrefcount(INF) == initial_ref
        assert sys.getrefcount(sfmt) == sfmt_ref

        test(sfmt, -INF, '-inf')
        assert sys.getrefcount(INF) == initial_ref
        assert sys.getrefcount(sfmt) == sfmt_ref

        test(sfmt, NAN, ' nan')
        assert sys.getrefcount(NAN) == initial_nan_ref
        assert sys.getrefcount(sfmt) == sfmt_ref

        test(sfmt, -NAN, ' nan')
        assert sys.getrefcount(NAN) == initial_nan_ref
        assert sys.getrefcount(sfmt) == sfmt_ref
