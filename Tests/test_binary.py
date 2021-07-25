
def test_addition():
    a = 987654
    b = 123456
    c = a + b
    assert c == 1111110

def test_subtraction():
    a = 987654
    b = 123456
    c = a - b
    assert c == 864198

def test_multiplication():
    a = 987
    b = 1001
    c = a * b
    assert c == 987987

def test_division():
    a = 12341234
    b = 10001
    c = a / b
    assert c == 1234

def test_floor_division():
    a = 7777777
    b = 55555
    c = a // b
    assert c == 140

def test_power():
    a = 0.5
    b = -8
    c = a ** b
    assert c == 256

def test_or():
    a = 1999
    b = 2999
    c = a | b
    assert c == 4095

def test_and():
    a = 1999
    b = 2999
    c = a & b
    assert c == 903

