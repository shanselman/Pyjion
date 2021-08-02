import pyjion
import sys
import pytest


def test_dict_keys_optimization():
    def _f():
        l = {'a': 1, 'b': 2}
        k = l.keys()
        return tuple(k)

    assert _f() == ('a', 'b')
    inf = pyjion.info(_f)
    assert inf.compiled
    assert inf.optimizations & pyjion.OptimizationFlags.BuiltinMethods


def test_object_refs():
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
