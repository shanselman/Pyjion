"""Test the optimization of intern values (-5 - 256)"""
import pyjion
import pyjion.dis
import pytest


def assertNotOptimized(func, capsys) -> None:
    assert not func()
    assert pyjion.info(func)['compiled']
    pyjion.dis.dis(func)
    captured = capsys.readouterr()

    assert "ldarg.1" in captured.out
    assert "METHOD_RICHCMP_TOKEN" in captured.out


def assertOptimized(func, capsys) -> None:
    assert not func()
    assert pyjion.info(func)['compiled']
    pyjion.dis.dis(func)
    captured = capsys.readouterr()

    assert "ldarg.1" in captured.out
    assert "MethodTokens.METHOD_RICHCMP_TOKEN" not in captured.out


@pytest.mark.optimization(level=1)
def test_const_compare(capsys):
    def _f():
        a = 1
        b = 2
        return a == b

    assertOptimized(_f, capsys)


@pytest.mark.optimization(level=1)
def test_const_compare_big_left(capsys):
    def _f():
        a = 1000
        b = 2
        return a == b

    assertOptimized(_f, capsys)


@pytest.mark.optimization(level=1)
def test_const_from_builtin(capsys):
    def _f():
        a = 2
        b = int("3")
        return a == b

    assertOptimized(_f, capsys)


@pytest.mark.optimization(level=1)
def test_const_compare_big_right(capsys):
    def _f():
        a = 1
        b = 2000
        return a == b

    assertOptimized(_f, capsys)


@pytest.mark.optimization(level=1)
def test_const_compare_big_both(capsys):
    def _f():
        a = 1000
        b = 2000
        return a == b

    assertOptimized(_f, capsys)


@pytest.mark.optimization(level=1)
def test_const_not_integer(capsys):
    def _f():
        a = 2
        b = "2"
        return a == b

    assertNotOptimized(_f, capsys)


@pytest.mark.optimization(level=1)
def test_float_compare(capsys):
    def _f():
        a = 2
        b = 1.0
        return a == b

    assertOptimized(_f, capsys)


@pytest.mark.optimization(level=1)
def test_dict_key(capsys):
    def _f():
        a = {0: 'a'}
        a[0] = 'b'
        return a[0] == 'b'

    assert _f()
    assert pyjion.info(_f)['compiled']
    pyjion.dis.dis(_f)
    captured = capsys.readouterr()
    assert "ldarg.1" in captured.out
    assert "METHOD_STORE_SUBSCR_DICT" in captured.out


@pytest.mark.optimization(level=1)
def test_dict_key_invalid_index(capsys):
    def _f_subscr():
        a = {0: 'a'}
        return a[1] == 'b'

    pytest.raises(KeyError, _f_subscr, )
    assert pyjion.info(_f_subscr)['compiled']
    pyjion.dis.dis(_f_subscr)
    captured = capsys.readouterr()
    assert "ldarg.1" in captured.out
    assert "METHOD_SUBSCR_DICT_HASH" in captured.out


@pytest.mark.optimization(level=1)
def test_list_key(capsys):
    def _f():
        a = ['a']
        a[0] = 'b'
        return a[0] == 'b'

    assert _f()
    assert pyjion.info(_f)['compiled']
    pyjion.dis.dis(_f)
    captured = capsys.readouterr()
    assert "ldarg.1" in captured.out
    assert "METHOD_STORE_SUBSCR_LIST_I" in captured.out


@pytest.mark.optimization(level=1)
def test_list_key_builtin(capsys):
    def _f():
        a = list(('a',))
        a[0] = 'b'
        return a[0] == 'b'

    assert _f()
    assert pyjion.info(_f)['compiled']
    pyjion.dis.dis(_f)
    captured = capsys.readouterr()
    assert "ldarg.1" in captured.out
    assert "METHOD_STORE_SUBSCR_LIST_I" in captured.out


@pytest.mark.optimization(level=1)
def test_list_key_non_const(capsys):
    def _f(b):
        a = ['a']
        a[b] = 'b'
        return a[b] == 'b'

    assert _f(0)
    assert pyjion.info(_f)['compiled']
    pyjion.dis.dis(_f)
    captured = capsys.readouterr()

    assert "ldarg.1" in captured.out
    assert "METHOD_STORE_SUBSCR_LIST_I" not in captured.out
    assert "METHOD_STORE_SUBSCR_LIST" in captured.out


@pytest.mark.optimization(level=1)
def test_list_from_builtin_key_non_const(capsys):
    def _f(b):
        a = list(('a',))
        a[b] = 'b'
        return a[b] == 'b'

    assert _f(0)
    assert pyjion.info(_f)['compiled']
    pyjion.dis.dis(_f)
    captured = capsys.readouterr()

    assert "ldarg.1" in captured.out
    assert "METHOD_STORE_SUBSCR_LIST_I" not in captured.out
    assert "METHOD_STORE_SUBSCR_LIST" in captured.out


@pytest.mark.optimization(level=1)
def test_list_key_invalid_index(capsys):
    def _f_subscr():
        l = [0, 1, 2]
        return l[4] == 'b'

    pytest.raises(IndexError, _f_subscr)
    assert pyjion.info(_f_subscr)['compiled']
    pyjion.dis.dis(_f_subscr)
    captured = capsys.readouterr()

    assert "ldarg.1" in captured.out
    assert "METHOD_SUBSCR_LIST_I" in captured.out


@pytest.mark.optimization(level=1)
def test_unknown_key_string_const(capsys):
    def _f(x):
        x['y'] = 'b'
        return x['y'] == 'b'

    assert _f({})
    assert pyjion.info(_f)['compiled']
    pyjion.dis.dis(_f)
    captured = capsys.readouterr()

    assert "ldarg.1" in captured.out
    assert "METHOD_STORE_SUBSCR_DICT_HASH" in captured.out


@pytest.mark.optimization(level=1)
def test_unknown_int_string_const(capsys):
    def _f(x):
        x[10] = 'b'
        return x[10] == 'b'

    assert _f({})
    assert pyjion.info(_f)['compiled']
    pyjion.dis.dis(_f)
    captured = capsys.readouterr()

    assert "ldarg.1" in captured.out
    assert "METHOD_SUBSCR_DICT_HASH" in captured.out
