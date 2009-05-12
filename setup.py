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

from distutils.core import setup, Extension
import commands
import sys


PYGTS_DEBUG = '1'  # '1' for on, '0' for off


# Get the build parameters using pkg-config

command = "pkg-config  --cflags-only-I gts"
result = commands.getoutput(command).strip().split('-I')
if len(result)==1:
    raise RuntimeException, result[0]
else:
    INCLUDE_DIRS = result[1:]
    for i,d in enumerate(INCLUDE_DIRS):
        INCLUDE_DIRS[i] = d.strip()

command = "pkg-config  --libs-only-L gts"
result = commands.getoutput(command).strip().split('-L')
if len(result)==1:
    raise RuntimeException, result[0]
else:
    LIB_DIRS = result[1:]
    for i,d in enumerate(LIB_DIRS):
        LIB_DIRS[i] = d.strip()

command = "pkg-config  --libs-only-l gts"
result = commands.getoutput(command).strip().split('-l')
if len(result)==1:
    raise RuntimeException, result[0]
else:
    LIBS = result[1:]
    for i,d in enumerate(LIBS):
        LIBS[i] = d.strip()

setup(name='pygts', 
      version='0.1.3',
      description=\
'PyGTS creates, manipulates and analyzes triangulated surfaces',
      long_description=\
"""PyGTS provides a python module that can be used to construct, 
manipulate, and perform computations on triangulated surfaces.  
It is a hand-crafted and "pythonic" binding for the GNU Triangulated 
Surface (GTS) Library.""",
      author='Thomas J. Duck',
      author_email='tom.duck@dal.ca',
      license='GNU Library General Public License (LGPL) version 2 or higher',
      url='http://aolab.phys.dal.ca/~tomduck/projects/pygts',
      download_url='http://aolab.phys.dal.ca/~tomduck/projects/pygts/dist',
      package_dir={ '' : 'src' },
      py_modules=['gts'],
      ext_modules=[Extension("_gts", ["src/pygts.c",
                                      "src/object.c",
                                      "src/point.c",
                                      "src/vertex.c",
                                      "src/segment.c",
                                      "src/edge.c",
                                      "src/triangle.c",
                                      "src/face.c",
                                      "src/surface.c",
                                      "src/cleanup.c"
                                      ],
                             define_macros=[('PYGTS_DEBUG',PYGTS_DEBUG)],
                             include_dirs = INCLUDE_DIRS,
                             library_dirs = LIB_DIRS,
                             libraries=LIBS)
                   ]
      )
