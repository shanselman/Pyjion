import sys
import pytest


# A class with a few attributes for testing the `getattr` and `setattr` builtins.
class F:
    a = 1
    b = 2
    c = 3
    d = 4


def test_existing_attr():
    f = F()
    before = sys.getrefcount(f)
    assert getattr(f, "a") is not None
    assert sys.getrefcount(f) == before


def test_missing_attr():
    f = F()
    before = sys.getrefcount(f)
    with pytest.raises(AttributeError):
        getattr(f, "e")
    assert sys.getrefcount(f) == before


def test_existing_attr():
    f = F()
    before = sys.getrefcount(f)
    assert setattr(f, "a", 10) is None
    assert f.a == 10
    assert before == sys.getrefcount(f)


def test_new_attr():
    f = F()
    before = sys.getrefcount(f)
    assert setattr(f, "e", 5) is None
    assert f.e == 5
    assert before == sys.getrefcount(f)
