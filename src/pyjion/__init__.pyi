from typing import Dict, Any

def enable() -> bool:
    """
    Enable the JIT.
    
    :returns: ``True`` on success, ``False`` on failure.
    """
    ...

def disable() -> bool:
    """
    Disable the JIT.
    
    :returns: ``True`` on success, ``False`` on failure.
    """
    ...

def info(f: callable) -> dict:
    """
    Pyjion JIT information on a compiled function.

    >>> pyjion.enable()
    >>> def f():
            a = 1
            b = 2
            c = 3
            d = 4
            return a + b + c + d
    >>> f()
    10
    >>> pyjion.info(f)

    
    :param f: The compiled function or code-object
    :returns: Information on the 
    """
    ...

def dump_il(f: callable) -> bytearray:
    ...

def dump_native(f: callable) -> tuple[bytearray, int, int]:
    ...

def enable_tracing() -> None:
    ...

def disable_tracing() -> None:
    ...

def enable_profiling() -> None:
    ...

def disable_profiling() -> None:
    ...

def enable_pgc() -> None:
    ...

def disable_pgc() -> None:
    ...

def set_optimization_level(level: int) -> None:
    ...

def enable_debug() -> None:
    ...

def disable_debug() -> None:
    ...

def get_offsets(f: callable) -> tuple[tuple[int, int, int]]:
    ...

def enable_graphs() -> None:
    ...

def disable_graphs() -> None:
    ...

def get_graph(f: callable) -> str:
    ...

def status() -> Dict[Any, Any]:
    ...

def symbols(f: callable) -> Dict[int, str]:
    ...

__version__: str
