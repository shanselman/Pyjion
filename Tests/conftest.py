import pytest
import pyjion
import gc
import warnings


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
    info = pyjion.info(item.function)
    if not info.compiled:
        warnings.warn("{0} did not compile ({1})".format(item.function, str(info.compile_result)))
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
    skip_external = pytest.mark.skip(reason="need --external option to run")
    skip_optimization = pytest.mark.skip(reason="test designed for higher optimization levels")
    for item in items:
        if "external" in item.keywords and not config.getoption("--external"):
            item.add_marker(skip_external)
        if "optimization" in item.keywords and config.getoption("--opt-level") == 0:
            item.add_marker(skip_optimization)
