import pyjion


def test_basic():
    def _f():
        def add_a(z):
            if len(z) < 5:
                z.append('a')
                return add_a(z)
            return z

        return add_a([])

    assert _f() == ['a', 'a', 'a', 'a', 'a']
    info = pyjion.info(_f)
    assert info['compiled']


def test_recursive_listcomp():
    def _f(t):
        print("-")
        if isinstance(t, list):
            return [_f(e) for e in t]
        else:
            return t

    assert _f([[0, 2], 2, 3]) == [[0, 2], 2, 3]
    info = pyjion.info(_f)
    assert info['compiled']
