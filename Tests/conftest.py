import pytest
import pyjion
import gc


def pytest_runtest_call(item: pytest.Item) -> None:
    pyjion.enable()
    pyjion.set_optimization_level(int(item.config.option.opt_level))
    for mark in item.iter_markers():
        if mark.name == "graph":
            pyjion.enable_graphs()
        elif mark.name == "profile":
            pyjion.enable_profiling()
        elif mark.name == "tracing":
            pyjion.enable_tracing()
    pyjion.enable_debug()

    item.runtest()

    pyjion.disable()
    pyjion.disable_graphs()
    pyjion.disable_profiling()
    pyjion.disable_tracing()
    gc.collect()


def pytest_addoption(parser):
    parser.addoption('--opt-level', action='store', type=int, default=1,
                     help='Optimization level')
    parser.addoption(
        "--external", action="store_true", default=False, help="run external tests"
    )


def pytest_generate_tests(metafunc):
    metafunc.fixturenames.append('repeat_n')
    metafunc.parametrize('repeat_n', (0, 1, 2))


def pytest_collection_modifyitems(config, items):
    if config.getoption("--external"):
        # --external given in cli: do not skip external tests
        return
    skip_external = pytest.mark.skip(reason="need --external option to run")
    for item in items:
        if "external" in item.keywords:
            item.add_marker(skip_external)