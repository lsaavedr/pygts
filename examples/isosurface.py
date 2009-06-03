#! /usr/bin/env python

"""isosurface.py -- plot isosurfaces

   Heavily based on iso.c in the GTS examples directory

   Copyright (C) 2009 Richard Everson
   All rights reserved.

   Richard Everson <R.M.Everson@exeter.ac.uk>
   School of Engineering, Computing and Mathematics,
   University of Exeter
   Exeter, EX4 4 QF.  UK. 

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

import sys, string
from optparse import OptionParser
import numpy
from enthought.mayavi import mlab
import gts

# Visualise a bug in GTS by running this with args --function=ellipsoid
# -c80.0 --method=dual
# In addition to the expected ellipsoid, there is an open surface in the
# z=-10 plane which appears to be the "shadow" of the ellipsoid intersected
# with the data grid. Running the GTS/examples/iso -d 50 50 50 80 yields
# similar, confirming that this is a GTS bug.



def ellipsoid(N=50):
    Nj = N*(0+1j)
    x, y, z = numpy.ogrid[-10:10:Nj, -10:10:Nj, -10:10:Nj]
    scalars = x*x + 2.0*y*y + z*z/2.0
    return scalars

def clebsch(N=50):
    # The Clebsch diagonal surface: a smooth cubic surface admitting the
    # symmetry of the tetrahedron (courtesy Johannes Beigel via GTS).
    Nj = N*(0+1j)
    w2 = numpy.sqrt(2.0)
    x, y, z = numpy.ogrid[-10:10:Nj, -10:10:Nj, -10:10:Nj]
    p = 1 - z - w2*x
    q = 1 - z + w2*x
    r = 1 + z + w2*y
    s = 1 + z - w2*y
    c1 = p + q + r - s
    c2 = p*p*p + q*q*q + r*r*r - s*s*s
    return c2 - c1*c1*c1


# Function to convert data between gts to mayavi formats
def get_mayavi_coords_and_triangles(s):
    vertices = s.vertices()
    coords = [v.get() for v in vertices]
    triangles = s.face_indices(vertices)
    x,y,z = zip(*coords)
    return x,y,z,triangles

functions = { "ellipsoid" : ellipsoid,
              "clebsch" : clebsch
            }

parser = OptionParser()

parser.add_option("-n", "--N", type="int",
                  dest="N", default=50,
                  help="Size of data cube")

parser.add_option("-c", "--isovalue", type="string",
                  dest="c", default="80.0",
                  help="""Comma separated list of values defining isosurfaces.
                  Try: --isovalue=25.0,120.0 --function=ellipsoid
                  """)

parser.add_option("--function", type="string", dest="function",
                  default="ellipsoid", 
                  help="Function defining surface: ['ellipsoid'|'clebsch']")

parser.add_option("--method", type="string",
                  dest="method", default="cube",
                  help="Iso-surface generation method: " \
                      "['cube'|'tetra'|'dual'|'bounded']")

(opt, args) = parser.parse_args()

extents = numpy.asarray([-10.0, 10.0, -10.0, 10.0, -10.0, 10.0])
func = functions[opt.function]
data = func(opt.N)
            
S = gts.Surface()
for c in [string.atof(x) for x in opt.c.split(',')]:
    S.add(gts.isosurface(data, c, extents=extents, method=opt.method))
print S.stats()['n_faces'], 'facets'
x,y,z,t = gts.get_coords_and_face_indices(S,True)
mlab.clf()
mlab.triangular_mesh(x,y,z,t,color=(0.25,0.25,0.75))

mlab.show()
