#! /usr/bin/env python

"""PyGTS - python module for the manipulation of triangulated surfaces

  Copyright (C) 2009 Thomas J. Duck
  All rights reserved.

  Thomas J. Duck <tom.duck@dal.ca>
  Department of Physics and Atmospheric Science,
  Dalhousie University, Halifax, Nova Scotia, Canada, B3H 3J5

NOTICE

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.
"""

# Please report any installation difficulties on the mailing list at
# pygts-users@lists.sourceforge.net

from distutils.core import setup, Extension
from distutils import sysconfig
import commands
import os, sys
import numpy, numpy.distutils


VERSION = '0.3.0'

PYGTS_DEBUG = '1'  # '1' for on, '0' for off

# Hand-code these lists if the auto-detection below doesn't work
INCLUDE_DIRS = []
LIB_DIRS = []
LIBS = []


# Get the build parameters using pkg-config and numpy's distutils

if not INCLUDE_DIRS:
    command = "pkg-config --cflags-only-I gts"
    result = commands.getoutput(command).strip().split('-I')
    if len(result)==1:
        if result[0]:  # then it's an error message; otherwise an empty result
            raise RuntimeError, result[0]
    else:
        INCLUDE_DIRS = result[1:]
        for i,d in enumerate(INCLUDE_DIRS):
            INCLUDE_DIRS[i] = d.strip()

    # numpy stuff
    numpy_include_dirs = numpy.distutils.misc_util.get_numpy_include_dirs()
    flag = False
    for path in numpy_include_dirs:
        if os.path.exists(os.path.join(path,'numpy/arrayobject.h')):
            flag = True
            break
    if not flag:
        raise RuntimeError, "Cannot find numpy/arrayobject.h"
    INCLUDE_DIRS.extend(numpy_include_dirs)


if not LIB_DIRS:
    command = "pkg-config --libs-only-L gts"
    result = commands.getoutput(command).strip().split('-L')
    if len(result)==1:
        if result[0]:  # then it's an error message; otherwise an empty result
            raise RuntimeError, result[0]
    else:
        LIB_DIRS = result[1:]
        for i,d in enumerate(LIB_DIRS):
            LIB_DIRS[i] = d.strip()

if not LIBS:
    command = "pkg-config --libs-only-l gts"
    result = commands.getoutput(command).strip().split('-l')
    if len(result)==1:
        if result[0]:  # then it's an error message; otherwise an empty result
            raise RuntimeError, result[0]
    else:
        LIBS = result[1:]
        for i,d in enumerate(LIBS):
            LIBS[i] = d.strip()


# Test for Python.h
python_inc_dir = sysconfig.get_python_inc()
if not python_inc_dir or \
        not os.path.exists(os.path.join(python_inc_dir, "Python.h")):
    raise RuntimeError, 'Python.h not found'


setup(name='pygts', 
      version=VERSION,
      description=\
'PyGTS creates, manipulates and analyzes triangulated surfaces',
      long_description=\
"""PyGTS is a python package that can be used to construct, 
manipulate, and perform computations on 3D triangulated surfaces.  
It is a hand-crafted and "pythonic" binding for the GNU Triangulated 
Surface (GTS) Library.""",
      author='Thomas J. Duck',
      author_email='tom.duck@dal.ca',
      license='GNU Library General Public License (LGPL) version 2 or higher',
      url='http://pygts.sourceforge.net/',
      download_url='https://sourceforge.net/project/downloading.php?group_id=262159&filename=pygts-'+VERSION+'.tar.gz',
      platforms='Platform-Independent',
      py_modules=['gts.__init__','gts.pygts','gts.blender'],
      classifiers = ['Development Status :: 3 - Alpha',
                     'Intended Audience :: Developers',
                     'Intended Audience :: Science/Research',
                     'License :: OSI Approved :: GNU Library or Lesser General Public License (LGPL)',
                     'Operating System :: OS Independent',
                     'Programming Language :: C',
                     'Programming Language :: Python',
                     'Topic :: Multimedia :: Graphics :: 3D Modeling',
                     'Topic :: Scientific/Engineering :: Mathematics',
                     'Topic :: Scientific/Engineering :: Visualization',
],
      ext_modules=[Extension("gts._gts", ["gts/pygts.c",
                                          "gts/object.c",
                                          "gts/point.c",
                                          "gts/vertex.c",
                                          "gts/segment.c",
                                          "gts/edge.c",
                                          "gts/triangle.c",
                                          "gts/face.c",
                                          "gts/surface.c",
                                          "gts/cleanup.c"
                                          ],
                             define_macros=[('PYGTS_DEBUG',PYGTS_DEBUG)],
                             include_dirs = INCLUDE_DIRS,
                             library_dirs = LIB_DIRS,
                             libraries=LIBS)
                   ]
      )
