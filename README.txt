Self-healing Data Structures
============================

(C) Copyright Sascha Peilicke 2010.

A cross-platform C++ library for self-healing and thus fault-rolerant data
structures with a STL-compatible interface and released under the Boost
Software License.

The following documentation can be generated from the source (see below):

 - API documentation and a tutorial
 - Using the library


The public Git_ repository is at: http://github.com/saschpe/self_healing

----


Prerequisites
-------------

This project uses Git_ as the revision-control system. Git_ is optional if you
retrieved this project as a source tarball and is only useful if you plan to
further develop the project or to send patches. The following software is
necessary to build this project:

 - CMake_ 2.6 or later
 - Boost_ 1.42.0 or later

Futhermore there are several optional prerequisites that enable further
functionality (like API documentation generated from the sources):

 - Doxygen_ 1.6.3 or later
 - Python Docutils_ 0.6 or later


Installation
------------

How to build the project depends on your platform. Generally you have to  use
CMake_ to generate a platform-dependent build-environment. You can do that
like this (in a terminal session)::

    $ mkdir build/
    $ cd build/
    $ cmake ..

If you want to specify an optimised build or set an installation target, the
following commands could be used instead::

    $ mkdir build/
    $ cd build/
    $ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../dist ..

You should now have either a Makefile on Unix-like operating systems or a
Visual Studio project on Windows. You can now build and install the project by
using Visual Studio (on Windows) or by issuing these commands (on Unix)::

    $ make
    $ make install

If you also want to build and execute the provided unit-tests as well as
generate useful code documentation, you can do the following::

    $ make && make test
    $ make doc
    $ make install

The exact build procedure may differ slightly on your platform. Have a look at
the `CMake documentation`__ for detailed help. If the build procedure
finished successfully and the unit-tests pass, the folder _dist_ should now
contain the application binary and data files.


Notes
-----

This file is written in reStructuredText_ and can be converted to HTML with
Python Docutils_. Use the following command::

    $ rst2html README.txt > README.html


.. _Git: http://git.or.cz/index.html
.. _CMake: http://www.cmake.org/
.. _Boost: http://www.boost.org/
.. _Doxygen: http://www.stack.nl/~dimitri/doxygen/index.html
.. _reStructuredText: http://en.wikipedia.org/wiki/Restructured_text
.. _docutils: http://docutils.sourceforge.net/

__ http://cmake.org/cmake/help/help.html
