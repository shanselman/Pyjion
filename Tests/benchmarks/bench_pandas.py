from pandas import Series
from numpy.random import default_rng
from string import ascii_lowercase


def udf(col):
    """ sample user-defined function """
    return 0


def bench_pandas():
    rng = default_rng(0)

    df = Series(
        rng.integers(-10, 10, size=(size := 1000)),
        index=rng.choice([*ascii_lowercase], size=(size, length := 10)).view(f'<U{length}').ravel(),
    )

    # will call `udf` once per unique group
    df.groupby(df.index).aggregate(udf)

    # will call `udf` once per three rows
    df.rolling(3).aggregate(udf)

    # will call `udf` once per row
    df.expanding().aggregate(udf)


__benchmarks__ = [(bench_pandas, "pandas", {"level": 2, "pgc": True}, 5)]
