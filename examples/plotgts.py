#! /usr/bin/env python

"""plotgts - plots the contents of a gts file

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

import sys

import numpy

from enthought.mayavi import mlab
import gts

if len(sys.argv)!=2:
    print 'Usage: python plotfile GTSFILE'
fname = sys.argv[1]

print '\n\nReading',fname,'...',
sys.stdout.flush()

f = open(fname)
s = gts.Surface()
s = gts.read(f)
f.close()

print 'Done.'
sys.stdout.flush()

print 'Splitting into separate connected and manifold surfaces...',
sys.stdout.flush()

surfaces = s.split()

print 'Done.'
sys.stdout.flush()

#for i,s in enumerate(surfaces):
#    print '\tSurface',i,'is',
#    if s.is_closed():
#        print 'closed.'
#    else:
#        print 'open.'

print 'Retrieving mayavi data...',
sys.stdout.flush()

args = []
for s in surfaces:
    args.append(gts.get_coords_and_face_indices(s,True))

print 'Done.'
sys.stdout.flush()

# Plot the surfaces
print 'Plotting...',
sys.stdout.flush()
for s,arg in zip(surfaces,args):
    x,y,z,t = arg
    mlab.triangular_mesh(x,y,z,t,color=(0.5,0.5,0.75))
mlab.show()
print 'Done.'
sys.stdout.flush()
