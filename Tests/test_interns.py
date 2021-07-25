"""Test the optimization of intern values (-5 - 256)"""
import pyjion
import io
import pyjion.dis
import contextlib
import pytest


def assertNotOptimized(func) -> None:
    assert not func()
    assert pyjion.info(func)['compiled']
    f = io.StringIO()
    with contextlib.redirect_stdout(f):
        pyjion.dis.dis(func)
    assert "ldarg.1" in f.getvalue()
    assert "METHOD_RICHCMP_TOKEN" in f.getvalue()

def assertOptimized(func) -> None:
    assert not func()
    assert pyjion.info(func)['compiled']
    f = io.StringIO()
    with contextlib.redirect_stdout(f):
        pyjion.dis.dis(func)
    assert "ldarg.1" in f.getvalue()
    assert "MethodTokens.METHOD_RICHCMP_TOKEN" not in f.getvalue()

def test_const_compare():
    def test_f():
        a = 1
        b = 2
        return a == b
    assertOptimized(test_f)

def test_const_compare_big_left():
    def test_f():
        a = 1000
        b = 2
        return a == b

    assertOptimized(test_f)

def test_const_from_builtin():
    def test_f():
        a = 2
        b = int("3")
        return a == b

    assertOptimized(test_f)

def test_const_compare_big_right():
    def test_f():
        a = 1
        b = 2000
        return a == b

    assertOptimized(test_f)

def test_const_compare_big_both():
    def test_f():
        a = 1000
        b = 2000
        return a == b

    assertOptimized(test_f)

def test_const_not_integer():
    def test_f():
        a = 2
        b = "2"
        return a == b

    assertNotOptimized(test_f)

def test_float_compare():
    def test_f():
        a = 2
        b = 1.0
        return a == b

    assertOptimized(test_f)


def test_dict_key():
    def test_f():
        a = {0: 'a'}
        a[0] = 'b'
        return a[0] == 'b'

    assert test_f()
    assert pyjion.info(test_f)['compiled']
    f = io.StringIO()
    with contextlib.redirect_stdout(f):
        pyjion.dis.dis(test_f)
    assert "ldarg.1" in f.getvalue()
    assert "METHOD_STORE_SUBSCR_DICT" in f.getvalue()

def test_dict_key_invalid_index():
    def test_f_subscr():
        a = {0: 'a'}
        return a[1] == 'b'

    with pytest.raises(KeyError):
        test_f_subscr()
    assert pyjion.info(test_f_subscr)['compiled']
    f = io.StringIO()
    with contextlib.redirect_stdout(f):
        pyjion.dis.dis(test_f_subscr)
    assert "ldarg.1" in f.getvalue()
    assert "METHOD_SUBSCR_DICT_HASH" in f.getvalue()

def test_list_key():
    def test_f():
        a = ['a']
        a[0] = 'b'
        return a[0] == 'b'

    assert test_f()
    assert pyjion.info(test_f)['compiled']
    f = io.StringIO()
    with contextlib.redirect_stdout(f):
        pyjion.dis.dis(test_f)
    assert "ldarg.1" in f.getvalue()
    assert "METHOD_STORE_SUBSCR_LIST_I" in f.getvalue()

def test_list_key_builtin():
    def test_f():
        a = list(('a',))
        a[0] = 'b'
        return a[0] == 'b'

    assert test_f()
    assert pyjion.info(test_f)['compiled']
    f = io.StringIO()
    with contextlib.redirect_stdout(f):
        pyjion.dis.dis(test_f)
    assert "ldarg.1" in f.getvalue()
    assert "METHOD_STORE_SUBSCR_LIST_I" in f.getvalue()

def test_list_key_non_const():
    def test_f(b):
        a = ['a']
        a[b] = 'b'
        return a[b] == 'b'

    assert test_f(0)
    assert pyjion.info(test_f)['compiled']
    f = io.StringIO()
    with contextlib.redirect_stdout(f):
        pyjion.dis.dis(test_f)
    assert "ldarg.1" in f.getvalue()
    assert "METHOD_STORE_SUBSCR_LIST_I" not in f.getvalue()
    assert "METHOD_STORE_SUBSCR_LIST" in f.getvalue()

def test_list_from_builtin_key_non_const():
    def test_f(b):
        a = list(('a',))
        a[b] = 'b'
        return a[b] == 'b'

    assert test_f(0)
    assert pyjion.info(test_f)['compiled']
    f = io.StringIO()
    with contextlib.redirect_stdout(f):
        pyjion.dis.dis(test_f)
    assert "ldarg.1" in f.getvalue()
    assert "METHOD_STORE_SUBSCR_LIST_I" not in f.getvalue()
    assert "METHOD_STORE_SUBSCR_LIST" in f.getvalue()

def test_list_key_invalid_index():
    def test_f_subscr():
        l = [0, 1, 2]
        return l[4] == 'b'

    with pytest.raises(IndexError):
        test_f_subscr()
    assert pyjion.info(test_f_subscr)['compiled']
    f = io.StringIO()
    with contextlib.redirect_stdout(f):
        pyjion.dis.dis(test_f_subscr)
    assert "ldarg.1" in f.getvalue()
    assert "METHOD_SUBSCR_LIST_I" in f.getvalue()

def test_unknown_key_string_const():
    def test_f(x):
        x['y'] = 'b'
        return x['y'] == 'b'

    assert test_f({})
    assert pyjion.info(test_f)['compiled']
    f = io.StringIO()
    with contextlib.redirect_stdout(f):
        pyjion.dis.dis(test_f)
    assert "ldarg.1" in f.getvalue()
    assert "METHOD_STORE_SUBSCR_DICT_HASH" in f.getvalue()

def test_unknown_int_string_const():
    def test_f(x):
        x[10] = 'b'
        return x[10] == 'b'

    assert test_f({})
    assert pyjion.info(test_f)['compiled']
    f = io.StringIO()
    with contextlib.redirect_stdout(f):
        pyjion.dis.dis(test_f)
    assert "ldarg.1" in f.getvalue()
    assert "METHOD_SUBSCR_DICT_HASH" in f.getvalue()
