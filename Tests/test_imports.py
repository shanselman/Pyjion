import pyjion


def test_import():
    def _f():
        import math
        return math.sqrt(4)

    assert _f() == 2
    info = pyjion.info(_f)
    assert info['compiled']

def test_import_from():
    def _f():
        from math import sqrt
        return sqrt(4)

    assert _f() == 2
    info = pyjion.info(_f)
    assert info['compiled']

def test_import_from_aliased():
    def _f():
        from math import sqrt as square_root
        return square_root(4)

    assert _f() == 2
    info = pyjion.info(_f)
    assert info['compiled']
