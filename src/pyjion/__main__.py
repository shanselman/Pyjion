import runpy
import pyjion
import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(
            """
    Usage: python -m pyjion <script.py> or python -m pyjion -m module ...args
            """
        )
        exit(1)
    pyjion.enable()
    if sys.argv[1] == "-m":
        mod = sys.argv[2]
        del sys.argv[1]  # Drop -m
        del sys.argv[1]  # Drop module
        runpy.run_module(mod, run_name="__main__", alter_sys=True)
    else:
        script = sys.argv[1]
        del sys.argv[1]  # Drop script
        runpy.run_path(script, run_name="__main__")
    pyjion.disable()
