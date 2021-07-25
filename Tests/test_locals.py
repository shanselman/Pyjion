"""Test the optimization of frame locals"""


def test_simple_compare():
    def test_f():
        a = 1
        b = 2
        return a == b

    assert not test_f()


def test_simple_delete():
    def test_f():
        a = 1
        b = 2
        del a
        del b
        return False

    assert not test_f()
