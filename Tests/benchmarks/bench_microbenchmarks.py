def test_floats(n=100000):
    """ Test float/integer arithmetic """
    for y in range(n):
        x = 0.1
        z = y * y + x - y
        x *= z


def test_ints(n=100000):
    """ Test integer arithmetic """
    for y in range(n):
        x = 2
        z = y * y + x - y
        x *= z


def test_bigints(n=100000):
    for _ in range(n):
        x = 200_100_100_100_100_100_100_100_100
        y = 100_100_100_100_100_100_100_100_100
        z = y * y + x - y
        x *= z


def test_function_calls(n=10000):

    def f():
        pass

    for i in range(n):
        f()
        f()
        f()
        f()
        f()
        f()
        f()


def test_builtin_type_calls(n=10000):
    for _ in range(n):
        int(n)
        int(n)
        int(n)
        int(n)
        int(n)
        int(n)
        int(n)


def test_builtin_func_calls(n=10000):
    for n in range(n):
        sum((n, n))
        sum((n, n))
        sum((n, n))
        sum((n, n))
        sum((n, n))
        sum((n, n))
        sum((n, n))


__benchmarks__ = [
    (test_floats, "floatmath_micro", {"level": 2}, 10),
    (test_ints, "intmath_micro", {"level": 2, "pgc": True}, 10),
    (test_bigints, "bigintmath_micro", {"level": 2, "pgc": True}, 10),
    (test_function_calls, "function_call_micro", {"level": 2, "pgc": True}, 10),
    (test_builtin_type_calls, "type_call_micro", {"level": 2, "pgc": True}, 10),
    (test_builtin_func_calls, "builtin_call_micro", {"level": 2, "pgc": True}, 10)
]
