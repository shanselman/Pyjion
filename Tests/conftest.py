import pytest


def pytest_runtest_call(item: pytest.Item) -> None:
    item.runtest()
