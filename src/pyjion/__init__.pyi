from typing import Dict, Any, Callable

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

def info(f: Callable) -> Dict[str, Any]:
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

def dump_il(f: Callable) -> bytearray:
    ...

def dump_native(f: Callable) -> tuple[bytearray, int, int]:
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

def get_offsets(f: Callable) -> tuple[tuple[int, int, int, int]]:
    ...

def enable_graphs() -> None:
    ...

def disable_graphs() -> None:
    ...

def get_graph(f: Callable) -> str:
    ...

def status() -> Dict[Any, Any]:
    ...

def symbols(f: Callable) -> Dict[int, str]:
    ...

class PyjionUnboxingError(ValueError):
    ...

__version__: str
