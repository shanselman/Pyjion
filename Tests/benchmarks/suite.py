"""
A benchmark suite for Pyjion
"""
import timeit
import warnings

from contextlib import redirect_stdout

import pyjion
import pyjion.dis
import pathlib
import sys
from statistics import fmean
from rich.console import Console
from rich.table import Table
from rich.text import Text
import cProfile

REPEAT = 5
TIMES = 5

if __name__ == "__main__":
    table = Table(title=f"Pyjion Benchmark Suite, repeat={REPEAT}, number={TIMES}")

    table.add_column("Benchmark", justify="right", style="cyan", no_wrap=True)
    table.add_column("Repeat", style="magenta")
    table.add_column("Min", style="magenta", width=10)
    table.add_column("Max", style="magenta", width=10)
    table.add_column("Mean", style="magenta", width=10)
    table.add_column("Min (+)", style="blue", width=10)
    table.add_column("Max (+)", style="blue", width=10)
    table.add_column("Mean (+)", style="blue", width=10)
    table.add_column("Delta", justify="right", style="green", width=10)

    graphs_out = pathlib.Path(__file__).parent / 'graphs'
    if not graphs_out.exists():
        graphs_out.mkdir()
    profiles_out = pathlib.Path(__file__).parent / 'profiles'
    if not profiles_out.exists():
        profiles_out.mkdir()

    for f in pathlib.Path(__file__).parent.glob("bench_*.py"):
        if len(sys.argv) > 1 and f.stem != f"bench_{sys.argv[1]}":
            continue
        i = __import__(f.stem, globals(), locals(), )
        if hasattr(i, "__benchmarks__"):
            for benchmark in i.__benchmarks__:
                func, desc, settings, repeat = benchmark
                with cProfile.Profile() as pr:
                    without_result = timeit.repeat(func, repeat=repeat, number=repeat)

                pr.dump_stats(profiles_out / f'{f.stem}_{desc}_without.pstat')

                with cProfile.Profile() as pr:
                    pyjion.enable()
                    pyjion.config(**settings, graph=True)
                    with_result = timeit.repeat(func, repeat=repeat, number=repeat)
                    pyjion.disable()
                    pr.dump_stats(profiles_out / f'{f.stem}_{desc}_with.pstat')

                with open(profiles_out / f'{f.stem}_{desc}.cil', 'w') as il:
                    with redirect_stdout(il):
                        pyjion.dis.dis(func)

                with open(graphs_out / f'{f.stem}.dot', 'w') as dot:
                    for k, attrib in i.__dict__.items():
                        if hasattr(attrib, "__code__"):
                            if pyjion.info(attrib.__code__).failed:
                                warnings.warn(
                                    f"Failed to compile {attrib.__code__} with result {pyjion.info(attrib.__code__).compile_result}")
                            else:
                                g = pyjion.graph(attrib.__code__)
                                if g:
                                    dot.write(g)
                                    dot.write('')

                delta = (abs(fmean(with_result) - fmean(without_result)) / fmean(without_result)) * 100.0
                if fmean(with_result) < fmean(without_result):
                    delta_repr = Text(f"{delta:.3f}%", style="green")
                else:
                    delta_repr = Text(f"-{delta:.1f}%", style="red")
                table.add_row(desc, str(repeat), "{:.3f}".format(min(without_result)), "{:.3f}".format(max(without_result)),
                              "{:.3f}".format(fmean(without_result)), "{:.3f}".format(min(with_result)),
                              "{:.3f}".format(max(with_result)), "{:.3f}".format(fmean(with_result)), delta_repr)

    console = Console(width=150)
    console.print(table)
