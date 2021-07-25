import sys


def test_perc_format():
    a = "Hello %s"
    before_ref = sys.getrefcount(a)
    c = a % ("world",)
    assert sys.getrefcount(a) == before_ref, "a ref"
    assert c == "Hello world", "string match"
    b = "w0rld"
    before_ref_b = sys.getrefcount(b)
    before_ref_c = sys.getrefcount(c)
    c += a % (b,)
    assert sys.getrefcount(a) == before_ref, "a leak"
    assert sys.getrefcount(b) == before_ref_b, "b leak"
    assert sys.getrefcount(c) == before_ref_c, "c leak"
    assert c == "Hello worldHello w0rld", "output fail"
    c += a % ("x", )
    assert sys.getrefcount(c) == before_ref_c, "c leak"

def test_add_inplace():
    c = "..."
    a = "Hello "
    b = "world!"
    before_ref = sys.getrefcount(a)
    before_ref_b = sys.getrefcount(b)
    before_ref_c = sys.getrefcount(c)
    c += a + b
    assert sys.getrefcount(a) == before_ref
    assert sys.getrefcount(b) == before_ref_b
    assert sys.getrefcount(c) == before_ref_c - 1
    assert c == "...Hello world!"



def test_perc_format():
    place = "world"
    message = f"Hello {place}!"
    assert message == "Hello world!"


def _is_dunder(name):
    """Returns True if a __dunder__ name, False otherwise."""
    return (len(name) > 4 and
            name[:2] == name[-2:] == '__' and
            name[2] != '_' and
            name[-3] != '_')


def _is_sunder(name):
    """Returns True if a _sunder_ name, False otherwise."""
    return (len(name) > 2 and
            name[0] == name[-1] == '_' and
            name[1:2] != '_' and
            name[-2:-1] != '_')



def test_sunder():
    assert _is_sunder("_hello_")
    assert not _is_sunder("helloooo!!_")
    assert _is_sunder("_hello_")
    assert not _is_sunder("helloooo!!_")

def test_dunder():
    assert _is_dunder("__hello__")
    assert not _is_dunder("_hello_")
    assert _is_dunder("__hello__")
    assert not _is_dunder("_hello_")

