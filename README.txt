=======
Robust
=======

A library for robust (fault-rolerant) data-structures with an STL-like
templated interface.

----


Prerequisites
-------------

This project uses Git_ as the revision-control system. Git_ is optional if you
retrieved this project as a source tarball and is only useful if you plan to
further develop the project or to send patches. The following software is
necessary to build this project:

 - CMake_ 2.6 or later
 - Boost_ 1.42.0 or later


Installation
------------

How to build the project depends on your platform. Generally you have to  use
CMake_ to generate a platform-dependent build-environment. You can do that
like this (in a terminal session)::

    $ mkdir build/
    $ cd build/
    $ cmake -DCMAKE_INSTALL_PREFIX=../dist ..

If you want to specify an optimised build, the following commands could be used
instead::

    $ mkdir build/ dist/
    $ cd build/
    $ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../dist ..

You should now have either a Makefile on Unix-like operating systems or a
Visual Studio project on Windows. You can now build and install the project by
using Visual Studio (on Windows) or by issuing these commands (on Unix)::

    $ make && make test
    $ make install

The exact build procedure may differ slightly on your platform. Have a look at
the `CMake documentation`__ for detailed help. The install step is mandatory
if you want to correctly setup translations and some other stuff. If the build
procedure successfully finished, the folder _dist_ should now contain the
application binary and data files.


License
-------

Please see the file COPYING.txt for details.


Authors
-------

Please see the file AUTHORS.txt for details.


Notes
-----

This file is written in reStructuredText_ and can be converted to HTML with
Python docutils_. Use the following command::

    $ rst2html README.txt > README.html


.. _Git: http://git.or.cz/index.html
.. _CMake: http://www.cmake.org/
.. _Boost: http://www.boost.org/
.. _Octave: http://www.gnu.org/software/octave/
.. _Matlab: http://www.mathworks.com/products/matlab/
.. _reStructuredText: http://en.wikipedia.org/wiki/Restructured_text
.. _docutils: http://docutils.sourceforge.net/

__ http://cmake.org/cmake/help/help.html
