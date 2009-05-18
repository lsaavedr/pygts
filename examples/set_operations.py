#! /usr/bin/env python

"""boolean.py - demonstrates boolean operations between surfaces

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

import gts
from enthought.mayavi import mlab

def get_surfaces():
    s1 = gts.tetrahedron()
    s2 = gts.sphere(4)
    s2.scale(0.95,0.95,0.95)
    return s1,s2


# Difference

s1,s2 = get_surfaces()
difference1 = s1.difference(s2)
difference1.translate(dz=-1.5)

s1,s2 = get_surfaces()
difference2 = s2.difference(s1)
difference2.translate(dz=1.5)


# Union
s1,s2 = get_surfaces()
union = s1.union(s2)
union.translate(dx=3)


# Intersection
s1,s2 = get_surfaces()
intersection = s1.intersection(s2)
intersection.translate(dx=-3)


# Plot the surfaces

def plot_surface(s):
    x,y,z,t = gts.get_mayavi_coords_and_triangles(s)
    mlab.triangular_mesh(x,y,z,t,color=(0.8,0.8,0.8))

plot_surface(difference1)
plot_surface(difference2)
plot_surface(union)
plot_surface(intersection)


mlab.show()
