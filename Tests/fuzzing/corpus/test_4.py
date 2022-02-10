def f():
    def _f():
        from math import sqrt
        return sqrt(4)

    assert _f() == 2