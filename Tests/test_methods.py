import pyjion
import sys


def test_dict_keys_optimization():
    def _f():
        l = {'a': 1, 'b': 2}
        k = l.keys()
        return tuple(k)
    assert _f() == ('a', 'b')
    inf = pyjion.info(_f)
    assert inf.compiled
    assert inf.optimizations & pyjion.OptimizationFlags.BuiltinMethods


class RefCountTestCase:

    def test_append_tuples_to_list(self):
        l = list()
        assert sys.getrefcount(l) == 2
        l.append((1, 2, 3))
        assert sys.getrefcount(l) == 2
        l.append((4, 5, 6))
        assert sys.getrefcount(l[0]) == 3
        assert sys.getrefcount(l[1]) == 3

    def test_object_refs(self):
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
        assert sys.getrefcount(node) == 2
        assert sys.getrefcount(node.a) == 3
        assert sys.getrefcount(node.b) == 3
        assert sys.getrefcount(node.c) == 3

        x = repr(node)
        assert x == "GrandchildNode(tag=101002, value=101001)"
        assert sys.getrefcount(node) == 2
        assert sys.getrefcount(node.a) == 3
        assert sys.getrefcount(node.b) == 3
        assert sys.getrefcount(node.c) == 3

        node.add_n(10000)
        assert sys.getrefcount(node) == 2
        assert sys.getrefcount(node.a) == 2
        assert sys.getrefcount(node.b) == 3
        assert sys.getrefcount(node.c) == 3

