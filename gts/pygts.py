# pygts - python module for the manipulation of triangulated surfaces
#
#   Copyright (C) 2009 Thomas J. Duck
#   All rights reserved.
#
#   Thomas J. Duck <tom.duck@dal.ca>
#   Department of Physics and Atmospheric Science,
#   Dalhousie University, Halifax, Nova Scotia, Canada, B3H 3J5
#
# NOTICE
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Library General Public
#   License as published by the Free Software Foundation; either
#   version 2 of the License, or (at your option) any later version.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Library General Public License for more details.
#
#   You should have received a copy of the GNU Library General Public
#   License along with this library; if not, write to the
#   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
#   Boston, MA 02111-1307, USA.

"""A module for constructing and manipulating triangulated surfaces.

PyGTS is a python binding for the GNU Triangulated Surface (GTS) 
Library, which may be used to build, manipulate, and perform
computations on triangulated surfaces.

The following geometric primitives are provided:

  Point - a point in 3D space
  Vertex - a Point in 3D space that may be used to define a Segment
  Segment - a line defined by two Vertex end-points
  Edge - a Segment that may be used to define the edge of a Triangle
  Triangle - a triangle defined by three Edges
  Face - a Triangle that may be used to define a face on a Surface
  Surface - a surface composed of Faces

A tetrahedron is assembled from these primitives as follows.  First,
create Vertices for each of the tetrahedron's points:

    import gts

    v1 = gts.Vertex(1,1,1)
    v2 = gts.Vertex(-1,-1,1)
    v3 = gts.Vertex(-1,1,-1)
    v4 = gts.Vertex(1,-1,-1)

Next, connect the four vertices to create six unique Edges:

    e1 = gts.Edge(v1,v2)
    e2 = gts.Edge(v2,v3)
    e3 = gts.Edge(v3,v1)
    e4 = gts.Edge(v1,v4)
    e5 = gts.Edge(v4,v2)
    e6 = gts.Edge(v4,v3)

The four triangular faces are composed using three edges each:

    f1 = gts.Face(e1,e2,e3)
    f2 = gts.Face(e1,e4,e5)
    f3 = gts.Face(e2,e5,e6)
    f4 = gts.Face(e3,e4,e6)

Finally, the surface is assembled from the faces:

    s = gts.Surface()
    for face in [f1,f2,f3,f4]:
        s.add(face)

Some care must be taken in the orientation of the faces.  In the above
example, the surface normals are pointing inward, and so the surface
technically defines a void, rather than a solid.  To create a 
tetrahedron with surface normals pointing outward, use the following
instead:

    f1.revert()
    s = Surface()
    for face in [f1,f2,f3,f4]:
        if not face.is_compatible(s):
            face.revert()
        s.add(face)

Once the Surface is constructed, there are many different operations that
can be performed.  For example, the volume can be calculated using:

    s.volume()

The difference between two Surfaces s1 and s2 is given by:

    s3 = s2.difference(s1)

Etc.

It is also possible to read in GTS data files and plot surfaces to
the screen.  See the example programs packaged with PyGTS for
more information.
"""

from _gts import *

def get_coords_and_face_indices(s,unzip=False):
    """Returns the coordinates and face indices of Surface s.

    If unzip is True then four tuples are returned.  The first three 
    are the x, y, and z coordinates for each Vertex on the Surface.  
    The last is a list of tuples, one for each Face on the Surface, 
    containing 3 indices linking the Face Vertices to the coordinate 
    lists.

    If unzip is False then the coordinates are given in a single list 
    of 3-tuples.
    """
    vertices = s.vertices()
    coords = [v.coords() for v in vertices]
    face_indices = s.face_indices(vertices)

    if unzip:
        x,y,z = zip(*coords)
        return x,y,z,face_indices
    else:
        return vertices, coords


def cube():
    """Returns a cube of side length 2 centered at the origin."""
    
    #             
    #       v8 +------+ v5
    #         /      /|
    #        /    v1/ |
    #    v4 +------+  |
    #       |      |  + v6
    #       |(v7)  | /
    #       |      |/
    #    v3 +------+ v2
    #

    v1,v2,v3,v4=Vertex(1,1,1),Vertex(1,1,-1),Vertex(1,-1,-1),Vertex(1,-1,1)
    v5,v6,v7,v8=Vertex(-1,1,1),Vertex(-1,1,-1),Vertex(-1,-1,-1),Vertex(-1,-1,1)

    e12,e23,e34,e14 = Edge(v1,v2), Edge(v2,v3), Edge(v3,v4), Edge(v4,v1)
    e56,e67,e78,e58 = Edge(v5,v6), Edge(v6,v7), Edge(v7,v8), Edge(v8,v5)
    e15,e26,e37,e48 = Edge(v1,v5), Edge(v2,v6), Edge(v3,v7), Edge(v4,v8)
    e13,e16,e18 = Edge(v1,v3), Edge(v1,v6), Edge(v1,v8)
    e27,e47,e57 = Edge(v7,v2), Edge(v7,v4), Edge(v7,v5)

    faces = [ Face(e12,e23,e13), Face(e13,e34,e14),
              Face(e12,e26,e16), Face(e15,e56,e16),
              Face(e15,e58,e18), Face(e14,e48,e18),
              Face(e58,e78,e57), Face(e56,e67,e57),
              Face(e26,e67,e27), Face(e37,e23,e27),
              Face(e37,e47,e34), Face(e78,e48,e47) ]

    faces[0].revert()  # Set the orientation of the first face

    s = Surface()

    for face in faces:
        if not face.is_compatible(s):
            face.revert()
        s.add(face)

    return s


def tetrahedron():
    """Returns a tetrahedron of side length 2*sqrt(2) centered at origin.

    The edges of the tetrahedron are perpendicular to the cardinal
    directions.
    """

    #       v4
    #        +
    #        | \ e6
    #  e5   '|e4 \ 
    #   v1 . +-e3-+ v3
    #       /   . 
    #     ./e1. e2
    #     / .
    #    +
    #   v2


    # Create vertices
    v1 = Vertex(1,1,1)
    v2 = Vertex(-1,-1,1)
    v3 = Vertex(-1,1,-1)
    v4 = Vertex(1,-1,-1)

    # Create edges
    e1 = Edge(v1,v2)
    e2 = Edge(v2,v3)
    e3 = Edge(v3,v1)
    e4 = Edge(v1,v4)
    e5 = Edge(v4,v2)
    e6 = Edge(v4,v3)

    # Create faces
    f1 = Face(e1,e2,e3) # Bottom face
    f2 = Face(e1,e4,e5) # Left face
    f3 = Face(e2,e5,e6) # Right face
    f4 = Face(e3,e4,e6) # Back face

    # Set orientation of first face
    f1.revert()

    # Assemble surface
    s = Surface()
    for face in [f1,f2,f3,f4]:
        if not face.is_compatible(s):
            face.revert()
        s.add(face)

    return s