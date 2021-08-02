import pyjion


def test_static_sequence():
    def f(x):
        a, b = x
        return a + b

    f([0, 1])
    assert pyjion.info(f).pgc >= 1

    f([0, 1])
    assert pyjion.info(f).pgc == 2


def test_changed_sequence():
    def f(x):
        a, b = x
        return a + b

    r = f([1, 2])
    assert r == 3
    assert pyjion.info(f).pgc >= 1
    r = f((3, 4))
    assert r == 7
    assert pyjion.info(f).pgc == 2
    r = f([3, 4])
    assert r == 7
    assert pyjion.info(f).pgc == 2


def test_recursive_sequence():
    def f(x):
        a, b = x
        if isinstance(x, list):
            f(tuple(x))
        a, b = x
        return a, b

    r = f([1, 2])
    assert r == (1, 2)
    assert pyjion.info(f).pgc == 2
    r = f([3, 4])
    assert r == (3, 4)
    assert pyjion.info(f).pgc == 2
    r = f((3, 4))
    assert r == (3, 4)
    assert pyjion.info(f).pgc == 2
