"""
A benchmark suite for Pyjion
"""
import timeit
import pyjion
import pathlib
from statistics import fmean
from rich.console import Console
from rich.table import Table
from rich.text import Text


if __name__ == "__main__":
    table = Table(title="Pyjion Benchmark Suite")

    table.add_column("Benchmark", justify="right", style="cyan", no_wrap=True)
    table.add_column("Min", style="magenta")
    table.add_column("Max", style="magenta")
    table.add_column("Mean", style="magenta")
    table.add_column("Min (+)", style="blue")
    table.add_column("Max (+)", style="blue")
    table.add_column("Mean (+)", style="blue")
    table.add_column("Delta", justify="right", style="green")

    for f in pathlib.Path(__file__).parent.glob("bench_*.py"):
        i = __import__(f.stem, globals(), locals(), ["__benchmarks__"])
        if hasattr(i, "__benchmarks__"):
            for benchmark in i.__benchmarks__:
                func, desc, settings = benchmark
                without_result = timeit.repeat(func, repeat=5, number=5)
                pyjion.enable()
                pyjion.config(**settings)
                with_result = timeit.repeat(func, repeat=5, number=5)
                pyjion.disable()
                delta = (abs(fmean(with_result) - fmean(without_result)) / fmean(without_result)) * 100.0
                if fmean(with_result) < fmean(without_result):
                    delta_repr = Text(f"{delta:.2f}%", style="green")
                else:
                    delta_repr = Text(f"-{delta:.2f}%", style="red")
                table.add_row(desc, "{:.2f}".format(min(without_result)), "{:.2f}".format(max(without_result)), "{:.2f}".format(fmean(without_result)), "{:.2f}".format(min(with_result)), "{:.2f}".format(max(with_result)), "{:.2f}".format(fmean(with_result)), delta_repr)

    console = Console()
    console.print(table)
