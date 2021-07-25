import sys


def test_list_init():
    l = []
    l.append(0)
    assert l == [0]


def test_list_prepopulated():
    l = [0, 1, 2, 3, 4]
    l.append(5)
    assert l == [0, 1, 2, 3, 4, 5]


def test_list_slice():
    l = [0, 1, 2, 3, 4]
    assert l[1:3] == [1, 2]


def test_const_list():
    def _f():
        l = [0, 1, 2, 3, 4]
        o = 0
        for x in l:
            o += x
        return o

    assert _f() == 10


def test_builtin_list():
    def _f():
        l = (0, 1, 2, 3, 4)
        o = 0
        for x in list(l):
            o += x
        return o

    assert _f() == 10


def test_const_list_refcount():
    a = 1
    b = 2
    c = 3
    before_a = sys.getrefcount(a)
    before_b = sys.getrefcount(b)
    before_c = sys.getrefcount(c)
    l = [a, b, c]
    del l
    assert before_a == sys.getrefcount(a)
    assert before_b == sys.getrefcount(b)
    assert before_c == sys.getrefcount(c)
