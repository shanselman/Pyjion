def test_abs():
    assert abs(1) == 1
    assert abs(-1) == 1
    assert abs(0.5) == 0.5


def test_all():
    assert all([True, False, True]) == False
    assert all([False, False, False]) == False
    assert all([1, 1, 1]) == True
    assert all([True, True, True]) == True


def test_any():
    assert any([True, False, True]) == True
    assert any([False, False, False]) == False
    assert any([1, 1, 1]) == True
    assert any([True, True, True]) == True


def test_ascii():
    assert ascii(1) == "1"
    assert ascii(0.5) == "0.5"
    assert ascii("hello") == "'hello'"


def test_bin():
    assert bin(3) == '0b11'
    assert bin(-10) == '-0b1010'


def test_bool():
    assert bool(1) == True
    assert bool(True) == True
    assert bool(0) == False

def test_bytearray():
    assert bytearray([True, False]) == bytearray(b'\x01\x00')

def test_bytes():
    assert bytes(bytes("hello", 'utf-8')) == b'hello'
    assert bytes(0x2) == b'\x00\x00'
    assert bytes((True, False)) == b'\x01\x00'

def test_callable():
    assert callable(test_callable) == True
    assert callable(1) == False
    assert callable(bin) == True

def test_chr():
    assert chr(91) == '['
    assert chr(True) == '\x01'
    assert chr(23480) == '宸'

def test_classmethod():
    class F:
        @classmethod
        def foo(cls, n):
            return cls

    assert F.foo(1) == F

def test_complex():
    assert complex('1+2j') == (1+2j)
    assert complex('1') == (1+0j)

def test_dict():
    assert dict([(1,0),(2,3),(3,4)]) == {1: 0, 2: 3, 3: 4}

def test_dir():
    assert 'upper' in dir('hello')

def test_divmod():
    assert divmod(1, 5) == (0, 1)
    assert divmod(1, 5.) == (0.0, 1.0)
    assert divmod(1., 5.) == (0.0, 1.0)
    assert divmod(-1., 5.) == (-1.0, 4.0)

def test_enumerate():
    seasons = ['Spring', 'Summer', 'Fall', 'Winter']
    assert list(enumerate(seasons)) == [(0, 'Spring'), (1, 'Summer'), (2, 'Fall'), (3, 'Winter')]
    assert list(enumerate(seasons, start=1)) == [(1, 'Spring'), (2, 'Summer'), (3, 'Fall'), (4, 'Winter')]

def test_filter():
    assert list(filter(lambda x: x % 2 == 0, [1, 2, 3, 4])) == [2, 4]

def test_float():
    assert float(1) == 1.0
    assert float(True) == 1.0
    assert float("2.4") == 2.4