import pytest
import pyjion
import gc


def pytest_runtest_call(item: pytest.Item) -> None:
    pyjion.enable()
    item.runtest()
    pyjion.disable()
    gc.collect()
