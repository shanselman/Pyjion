.. _Graphing:

Graphing
========

Pyjion can create .dot (graphviz) graphs for any function during the compilation process:

.. code-block:: python

    import pyjion
    pyjion.enable()
    pyjion.config(graph=True)

Once this is enabled and functions have been compiled, you can get the graph using the ``graph()`` function:

.. code-block:: python

    >>> pyjion.graph(f)

    digraph f { 
    ...

You can use a tool like `GraphViz Online <https://dreampuf.github.io/GraphvizOnline/>`_ to render the graph:

.. image:: _static/isnone.png
    :width: 50%
    :align: center
