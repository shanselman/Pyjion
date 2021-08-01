import pytest

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

def test_format():
    assert format(1234567890, ',') == '1,234,567,890'
    assert format(1234.567890, '.2f') == '1234.57'
    assert format(1234.567890, '.3f') == '1234.568'

def test_frozenset():
    assert frozenset({0, 2, 2}) == frozenset({0, 2})
    assert frozenset(['this', 'is', 'a', 'test']) == frozenset({'a', 'test', 'this', 'is'})

def test_getattr():
    class F:
        name = 'bob'
    
    f = F()
    assert getattr(f, 'name') == 'bob'
    assert getattr(1, 'denominator') == 1

# dont test globals

def test_hasattr():
    class F:
        name = 'bob'
    
    f = F()
    assert hasattr(f, 'name')
    assert hasattr(1, 'denominator')
    assert not hasattr(f, 'sf')
    assert not hasattr(1, 'visible')

def test_hash():
    class F:
        def __hash__(self):
            return 123456

    f = F()

    assert hash(f) == 123456
    assert hash(1234) == 1234

# Dont test help()

def test_hex():
    assert hex(255) == '0xff'
    assert hex(-42) == '-0x2a'

def test_id():
    assert id(12) == id(12)
    assert id("bob") != id("jim")

# Dont test input

def test_int():
    assert int("123") == 123
    assert int(True) == 1
    assert int(1.4) == 1
    assert int("123", base=5) == 38

def test_isinstance():
    assert isinstance(1, int)
    assert isinstance("hello", (str,))
    class F:
        pass
    f = F()
    assert isinstance(f, F)

def test_issubclass():
    class F:
        pass
    class G(F):
        pass
    assert issubclass(F, F)
    assert issubclass(F, F)
    assert not issubclass(F, G)

def test_iter():
    assert  ''.join([n.upper() for n in iter('hello')]) == 'HELLO'

def test_len():
    assert len([]) == 0
    assert len((1, 2)) == 2
    assert len("test") == 4
    class F:
        def __len__(self):
            return 89
    
    f = F()
    assert len(f) == 89

def test_list():
    assert list([]) == []
    assert list((1,2,3)) == [1, 2, 3]
    assert list("test") == ["t", "e", "s", "t"]

# dont test locals()

def test_map():
    assert list(map(str.upper, "test")) == ['T', 'E', 'S', 'T']
    _v = map(str.upper, "test")
    assert isinstance(_v, map)

def test_max():
    assert max([1,2,3,4]) == 4
    assert max(1, 2, 3, 4) == 4
    pytest.raises(ValueError, max, [])

def test_memoryview():
    v = memoryview(b'abcefg')
    assert v[1] == 98
    assert v[-1]  == 103
    assert type(v[1:4]) == memoryview
    assert bytes(v[1:4]) == b'bce' 

def test_min():
    assert min([1,2,3,4]) == 1
    assert min(1, 2, 3, 4) == 1
    assert min(.1, .2, .3, .4) == .1
    pytest.raises(ValueError, min, [])

def test_next():
    gen = iter("hi")
    assert next(gen) == "h"
    assert next(gen) == "i"
    pytest.raises(StopIteration, next, gen)
    assert next(gen, False) == False

def test_object():
    a = object()
    assert str(a).startswith('<object object at')
    assert type(a) == object

def test_oct():
    assert oct(8) == '0o10'
    assert oct(-56) == '-0o70'

# dont test open, we use it enough

def test_ord():
    assert ord('[') == 91
    assert ord('\x01') == 1
    assert ord('宸') == 23480

def test_pow():
    assert pow(2, 2) == 4
    assert pow(3, -2) == 0.1111111111111111
    assert pow(-3, 0) == 1
    assert pow(.3, .3) == 0.696845301935949
    assert pow(38, -1, mod=97) == 23

# dont test print, it gets used enough

def test_property():
    class C:
        def __init__(self):
            self._x = None

        def getx(self):
            return self._x

        def setx(self, value):
            self._x = value

        def delx(self):
            del self._x

        x = property(getx, setx, delx, "I'm the 'x' property.")
    
    c = C()
    c.x = 1
    assert c.x == 1
    del c.x

    class Parrot:
        def __init__(self):
            self._voltage = 100000

        @property
        def voltage(self):
            """Get the current voltage."""
            return self._voltage
    
    parrot = Parrot()
    assert parrot.voltage == 100000

def test_range():
    assert list(range(3)) == [0, 1, 2]
    assert list(range(2,3)) == [2]
    assert list(range(2,6,2)) == [2, 4]

def test_repr():
    class T:
        def __init__(self, x):
            self.x = x

        def __repr__(self):
            return '{}'.format(self.x)
    
    t = T(1)

    assert repr(t) == '1'
    assert repr(T(42)) == '42'

def test_reversed():
    back = reversed([1,2])
    assert next(back) == 2
    assert next(back) == 1
    assert list(reversed([1,2])) == [2, 1]

def test_round():
    assert round(1) == 1
    assert round(.1) == 0
    assert round(2.645, 2) == 2.65
    assert round(-0.5) == 0

def test_set():
    assert set([1,2,3]) == {1, 2, 3}
    assert set({1, 2, 3}) == {1, 2, 3}
    assert set("banana") == {"a", "n", "b"}

def test_setattr():
    class F:
        name = 'bob'
    
    f = F()
    assert f.name == "bob"
    setattr(f, 'name', "jim")
    assert f.name == "jim"

def test_slice():
    s1 = slice(0, 1)
    assert s1.start == 0
    assert s1.stop == 1
    assert s1.step == None
    s2 = slice(None, None, -1)  # reverse slice
    assert s2.start == None
    assert s2.stop == None
    assert s2.step == -1

def test_sorted():
    assert sorted([1,3,2]) == [1, 2, 3]
    assert sorted("hat") == ['a', 'h', 't']

def test_staticmethod():
    class T:
        @staticmethod
        def f(a, b):
            return a + b
    
    assert T.f(1, 2) == 3
    assert T().f(2, 3) == 5

    class M:
        fake_len = staticmethod(len)
    
    assert M.fake_len([1, 2]) == 2

def test_str():
    class T:
        def __str__(self):
            return "yes"
    
    t = T()
    assert str(t) == "yes"
    assert str(1) == "1"

def test_sum():
    assert sum([1, 2, 3]) == 6
    assert sum((1, 2, 3)) == 6
    assert sum([1, 2, 3], start=1) == 7

def test_super():
    class A:
        def foo(self):
            return 2
    class B(A):
        def foo(self):
            return super().foo()
    a = A()
    b = B()
    assert b.foo() == 2

def test_type():
    assert type("hello") == str
    assert type(1) == int
    assert type(.1) == float
    assert type(object) == type

    # test the type function as well
    SuperInt = type("SuperInt", (int,), dict(a=100))
    assert SuperInt(123) == 123
    assert SuperInt(123).a == 100

def test_vars():
    assert 'Create a new memoryview object' in vars(memoryview)['__doc__'] 

def test_zip():
    x = [1, 2, 3]
    y = [4, 5, 6]
    zipped = zip(x, y)
    assert list(zipped) == [(1, 4), (2, 5), (3, 6)]

# dont test __import__