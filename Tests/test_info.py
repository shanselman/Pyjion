import pyjion


def test_once():
    def _f():
        a = 1
        b = 2
        c = 3
        d = 4
        return a + b + c + d

    assert _f() == 10
    info = pyjion.info(_f)

    assert info.compiled
    assert not info.failed
    assert info.run_count >= 1


def test_never():
    def _f():
        a = 1
        b = 2
        c = 3
        d = 4
        return a + b + c + d

    info = pyjion.info(_f)

    assert not info.compiled
    assert not info.failed
    assert info.run_count == 0


def test_twice():
    def _f():
        a = 1
        b = 2
        c = 3
        d = 4
        return a + b + c + d

    assert _f() == 10
    assert _f() == 10
    info = pyjion.info(_f)

    assert info.compiled
    assert not info.failed
    assert info.run_count >= 2
