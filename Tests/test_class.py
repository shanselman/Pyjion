
def test_add():
    class Number:
        def __add__(self, other):
            return 4 + other

        def __radd__(self, other):
            return other + 4

    a = Number()
    assert 3 + a == 7
    assert a + 3 == 7
