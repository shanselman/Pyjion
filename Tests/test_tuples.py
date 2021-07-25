import sys


def test_identical_tuples():
    l = ('0', '1')
    m = ('0', '1')
    assert sys.getrefcount(l) == 4
    assert sys.getrefcount(m) == 4


def test_tuple_prepopulated():
    l = ('0', '1', '2', '3', '4')
    r = sys.getrefcount(l)
    assert sys.getrefcount(l) == r
    assert l == ('0', '1', '2', '3', '4')
    assert sys.getrefcount(l) == r
    for i in range(0, 5):
        assert l[i] == str(i)
    assert sys.getrefcount(l) == r
    assert l[4] == '4'
    assert sys.getrefcount(l) == r


def test_tuple_slice():
    l = ('0', '1', '2', '3', '4')
    r = sys.getrefcount(l)
    assert sys.getrefcount(l) == r
    assert l[1:3] == ('1', '2')
    assert sys.getrefcount(l) == r


def test_tuple_unpack():
    l = ('0', '1', '2', '3', '4')
    r = sys.getrefcount(l)
    assert sys.getrefcount(l) == r
    first, second, *_, last = l
    assert sys.getrefcount(l) == r
    assert first == '0'
    assert second == '1'
    assert last == '4'
    assert sys.getrefcount(l) == r


def test_unknown_type():
    def get_vals():
        return (0, 1, 2, 3)

    res = get_vals()
    r = sys.getrefcount(res)
    assert sys.getrefcount(res) == r
    assert res[0] == 0
    assert res[1] == 1
    assert res[2] == 2
    assert sys.getrefcount(res) == r


def test_tuple_non_const():
    zero = str(0)
    r_zero = sys.getrefcount(zero)
    one = str(1)
    two = str(2)
    three = str(3)
    four = str(4)
    l = (zero, one, two, three, four)
    r = sys.getrefcount(l)
    assert sys.getrefcount(l) == r
    assert sys.getrefcount(zero) == r_zero + 1
    first, second, *_, last = l
    assert sys.getrefcount(l) == r
    assert sys.getrefcount(zero) == r_zero + 2
    assert first == '0'
    assert second == '1'
    assert last == '4'
    assert sys.getrefcount(l) == r
    assert sys.getrefcount(zero) == r_zero + 2
