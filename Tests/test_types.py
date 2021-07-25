import os
import types


def test_subclassdict():
    """Test that a subclass of dict can be merged into a static dict"""
    out = {**os.environ, 'foo': 'bar'}

    assert 'foo' in out


def test_resolve_bases():

    class A: pass
    class B: pass
    class C:
        def __mro_entries__(self, bases):
            if A in bases:
                return ()
            return (A,)
    c = C()
    assert types.resolve_bases(()) == ()
    assert types.resolve_bases((c,)) == (A,)
    assert types.resolve_bases((C,)) == (C,)
    assert types.resolve_bases((A, C)) == (A, C)
    assert types.resolve_bases((c, A)) == (A,)
    assert types.resolve_bases((A, c)) == (A,)
    x = (A,)
    y = (C,)
    z = (A, C)
    t = (A, C, B)
    for bases in [x, y, z, t]:
        assert types.resolve_bases(bases) is bases
