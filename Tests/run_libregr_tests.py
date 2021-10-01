import pyjion
import gc
from test.libregrtest import main

pyjion.enable()
pyjion.config(graph=True, level=1, debug=True, pgc=True)
main()
pyjion.disable()
gc.collect()
