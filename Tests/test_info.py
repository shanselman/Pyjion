import pyjion


def test_once():
    def test_f():
        a = 1
        b = 2
        c = 3
        d = 4
        return a+b+c+d

    assert test_f() == 10
    info = pyjion.info(test_f)

    assert info['compiled']
    assert not info['failed']
    assert info['run_count'] >= 1

def test_never():
    def test_f():
        a = 1
        b = 2
        c = 3
        d = 4
        return a+b+c+d
    info = pyjion.info(test_f)

    assert not info['compiled']
    assert not info['failed']
    assert info['run_count'] == 0

def test_twice():
    def test_f():
        a = 1
        b = 2
        c = 3
        d = 4
        return a+b+c+d

    assert test_f() == 10
    assert test_f() == 10
    info = pyjion.info(test_f)

    assert info['compiled']
    assert not info['failed']
    assert info['run_count'] >= 2

