import pyjion
import pytest


@pytest.mark.graph
def test_graph_dump() -> None:
    def f():
        a = 1
        b = 2
        return a + b
    f()
    assert pyjion.graph(f) is not None
    f()
    assert pyjion.graph(f) is not None
