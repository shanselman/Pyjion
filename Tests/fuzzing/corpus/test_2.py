def f():
    assert {1: 'a', 2: 'b', 3: 'c'} == {1: 'a', 2: 'b', 3: 'c'}

    def g(a, b, c):
        return {'a': a, 'b': b, 'c': c}

    assert g(1, 2, 3) | g(1, 2, 3) == {'a': 1, 'b': 2, 'c': 3}
