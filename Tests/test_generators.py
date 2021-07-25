def test_single_yield():
    def gen():
        x = 1
        yield x

    g = gen()
    assert next(g) == 1


def test_double_yield():
    def gen():
        x = 1
        yield x
        yield 2

    g = gen()
    assert next(g) == 1
    assert next(g) == 2


def test_conditional_yield():
    def gen():
        x = 1
        if x == 1:
            yield x
        else:
            yield 2

    g = gen()
    assert next(g) == 1


def test_yields_from_iterator():
    def gen():
        yield 1
        yield 2
        yield 3

    g = gen()
    result = list(g)
    assert result == [1, 2, 3]


def test_yields_from_range_gen():
    def gen():
        for n in range(10):
            yield f'{n}!'

    result = []
    for x in gen():
        result.append(x)
    assert result == ['0!', '1!', '2!', '3!', '4!', '5!', '6!', '7!', '8!', '9!']


def test_yields_from_range_gen_listcomp():
    def gen():
        for n in range(10):
            yield f'{n}!'

    result = [x for x in gen()]
    assert result == ['0!', '1!', '2!', '3!', '4!', '5!', '6!', '7!', '8!', '9!']
