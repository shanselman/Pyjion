import pyjion
import pytest


@pytest.mark.optimization(level=1)
def test_import():
    def _f():
        print("foo foo")
        return 2

    assert _f() == 2
    inf = pyjion.info(_f)
    assert inf.compiled
    assert inf.optimizations & pyjion.OptimizationFlags.HashedNames
