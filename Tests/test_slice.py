import sys


def test_list_slicing():
    l = [0, 1, 2, 3]
    initial_ref = sys.getrefcount(l)
    assert l[0:1] == [0]
    assert initial_ref == sys.getrefcount(l)

    assert l[:1] == [0]
    assert initial_ref == sys.getrefcount(l)

    assert l[1:] == [1, 2, 3]
    assert initial_ref == sys.getrefcount(l)

    assert l[:] == [0, 1, 2, 3]
    assert initial_ref == sys.getrefcount(l)

    assert l[-2:-1] == [2]
    assert initial_ref == sys.getrefcount(l)

    assert l[-1:-2] == []
    assert initial_ref == sys.getrefcount(l)

    assert l[-1:] == [3]
    assert initial_ref == sys.getrefcount(l)

    assert l[:-1] == [0, 1, 2]
    assert initial_ref == sys.getrefcount(l)

    assert l[0:1:] == [0]
    assert initial_ref == sys.getrefcount(l)

    assert l[0:1:1] == [0]
    assert initial_ref == sys.getrefcount(l)

    assert l[::1] == [0, 1, 2, 3]
    assert initial_ref == sys.getrefcount(l)

    assert l[::-1] == [3, 2, 1, 0]
    assert initial_ref == sys.getrefcount(l)
    n = l[::-1]
    assert sys.getrefcount(n) == 2

    m = l[:]
    assert sys.getrefcount(m) == 2

    assert l[::-2] == [3, 1]
    assert initial_ref == sys.getrefcount(l)

    assert l[::2] == [0, 2]
    assert initial_ref == sys.getrefcount(l)

    assert [0, 1, 2, 3][False:True] == [0]
    assert initial_ref == sys.getrefcount(l)


def test_list_slicing_expressions():
    l = [0, 1, 2, 3]
    x = int(2)  # prevent const rolling
    initial_ref = sys.getrefcount(l)
    assert l[x + 1:0] == []
    assert initial_ref == sys.getrefcount(l)
    assert l[:x - 1] == [0]
    assert initial_ref == sys.getrefcount(l)
    assert l[x:x + 1] == [2]
    assert initial_ref == sys.getrefcount(l)
    assert l[:x - 4] == [0, 1]
    assert initial_ref == sys.getrefcount(l)
    assert l[x::-1] == [2, 1, 0]
    assert initial_ref == sys.getrefcount(l)


def test_string_slicing():
    assert 'The train to Oxford leaves at 3pm'[-1:3:-2] == 'm3t ealdox tnat'
