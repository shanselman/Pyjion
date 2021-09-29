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
    pyjion.enable_debug()
    item.runtest()
    pyjion.disable()
    info = pyjion.info(item.function)
    if not info.compiled:
        warnings.warn("{0} did not compile ({1})".format(item.function, str(info.compile_result)))
    pyjion.disable_graphs()
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
        if "optimization" in item.keywords:
            args = item.get_closest_marker(name="optimization").args
            if len(args) == 0:
                level = 1
            else:
                level = args[0]
            if config.getoption("--opt-level") != level:
                item.add_marker(skip_optimization)
