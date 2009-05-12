#! /usr/bin/env python

"""tetrahedron.py - plots a tetrahedron

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

s = gts.tetrahedron()

# Plot the surface
x,y,z,t = gts.get_mayavi_coords_and_triangles(s)
mlab.triangular_mesh(x,y,z,t,color=(0.5,0.5,0.75))
mlab.triangular_mesh(x,y,z,t,color=(0,0,1),representation='fancymesh',
                     scale_factor=0.2)
mlab.show()
