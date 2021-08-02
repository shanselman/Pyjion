
def test_add():
    class Number:
        def __add__(self, other):
            return 4 + other

        def __radd__(self, other):
            return other + 4

    a = Number()
    assert 3 + a == 7
    assert a + 3 == 7



def test_inheritance():
    class Node(object):
        def __init__(self, a, b, c):
            self.a = a
            self.b = b
            self.c = c

        def add_n(self, n):
            self.a += n

        def __repr__(self):
            value = self.a
            value = repr(value)
            return '%s(tag=%r, value=%s)' % (self.__class__.__name__, self.b, value)

    class ChildNode(Node):
        def __init__(self, a, b, c):
            self.a = a
            self.b = b
            self.c = c

    class GrandchildNode(ChildNode):
        d = 101000

    node = GrandchildNode(101001, 101002, 101003)
    x = repr(node)
    assert x == "GrandchildNode(tag=101002, value=101001)"
    node.add_n(10000)