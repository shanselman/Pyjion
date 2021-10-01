import runpy
import pyjion
import argparse


def main():
    parser = argparse.ArgumentParser(prog='pyjion', description='Python JIT Compiler')

    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('script', help='script file', nargs='?')

    group.add_argument('-m', action='store', type=str, dest="module", help="Execute module")

    parser.add_argument('--graph',
                        action='store_true',
                        help='Enable instruction graph generation')

    parser.add_argument('--debug',
                        action='store_true',
                        help='Enable debuggable JIT methods')

    parser.add_argument('--no-pgc',
                        action='store_true',
                        help='Disable PGC')

    parser.add_argument('-o',
                    '--opt-level',
                    action='store', type=int, default=1,
                    help='Optimization level (default 1')

    args = parser.parse_args()

    pyjion.enable()

    pyjion.config(graph=args.graph, debug=args.debug, pgc=not args.no_pgc, level=args.opt_level)

    if args.module:
        runpy.run_module(args.module)
    else:
        runpy.run_path(args.script)
    pyjion.disable()


if __name__ == "__main__":
    main()
