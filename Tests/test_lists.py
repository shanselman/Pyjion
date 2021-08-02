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
    l = [0, 1, 2, 3, 4]
    o = 0
    for x in l:
        o += x
    assert o == 10


def test_builtin_list():
    l = (0, 1, 2, 3, 4)
    o = 0
    for x in list(l):
        o += x
    assert o == 10


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


def test_list_plus():
    assert [1, 2, 3] + [4, 5, 6] == [1, 2, 3, 4, 5, 6]


def test_list_comprehension():
    zzzs = (1, 2, 3)
    assert [z for z in zzzs] == [1, 2, 3]


def test_list_comprehension_with_generator():
    assert [i for i in range(6)] == [0, 1, 2, 3, 4, 5]


def test_list_indexing():
    l = [4, 3, 2, 1, 0]
    assert l[0] == 4
    i = 2
    l = [4, 3, 2, 1, 0]
    assert l[i] == 2
    l = [4, 3, 2, 1, 0]
    assert l[-1] == 0
    l = [0]
    a = 1
    a -= 1
    assert l[a] == 0
    l = [4, 3, 2, 1, 0]
    assert l[::-1] == [0, 1, 2, 3, 4]

def test_list_unpacking():
    assert [1, *[2], 3, 4] == [1, 2, 3, 4]
    assert [1, *{2}, 3] == [1, 2, 3]