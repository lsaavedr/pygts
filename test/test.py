#! /usr/bin/env python

"""test.py - unit tests for PyGTS

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

import unittest
import sys
import tempfile
import os.path

from math import sqrt, fabs, pi, radians, atan

import gts

try:
    import numpy
    getattr(gts,'isosurface')  # Should be there if numpy was compiled in
except:
    HAS_NUMPY = False
else:
    HAS_NUMPY = True


# GTS throws up error messages for the is_ok() tests
TEST_IS_OK = False


# Sometimes python is missing the all() function
try:
    all([True])
except:
    def all(xs):
        for x in xs:
            if not x:
                return False
        return True


class TestPointMethods(unittest.TestCase):

    Point = gts.Point


    def test_new(self):

        p = self.Point()
        self.assert_(p.is_ok())
        self.assert_(isinstance(p,gts.Point))
        self.assert_(not isinstance(p,gts.Vertex))

        p = self.Point()
        self.assert_(p.is_ok())
        self.assert_(p.x==0 and p.y==0 and p.z==0)

        p = self.Point(1)
        self.assert_(p.is_ok())
        self.assert_(p.x==1 and p.y==0 and p.z==0)

        p = self.Point(1,2)
        self.assert_(p.is_ok())
        self.assert_(p.x==1 and p.y==2 and p.z==0)

        p = self.Point(1,2,3)
        self.assert_(p.is_ok())
        self.assert_(p.x==1 and p.y==2 and p.z==3)


    def test_subclass(self):

        Super = self.Point

        class Point(Super):

            def __init__(self,x,y,z,msg):
                self.msg = msg
                Super.__init__(self,x,y,z)

            def foo(self):
                return self.msg

        p = Point(1,2,3,'bar')
        self.assert_(p == self.Point(1,2,3))
        self.assert_(p.foo()=='bar')


    def test_compare(self):

        p1,p2,p3 = self.Point(0), self.Point(1), self.Point(1)
        self.assert_(p1.id!=p2.id)
        self.assert_(p2.id!=p3.id)
        self.assert_(p1!=p2)
        self.assert_(p2==p3)

        self.assert_(p1.is_ok())
        self.assert_(p2.is_ok())
        self.assert_(p3.is_ok())


    def test_getset(self):

        p = self.Point()
        self.assert_(p.is_ok())
        
        p.set(1)
        self.assert_(p.is_ok())
        self.assert_(p.x==1 and p.y==0 and p.z==0)

        p.set(1,2)
        self.assert_(p.is_ok())
        self.assert_(p.x==1 and p.y==2 and p.z==0)

        p.set(1,2,3)
        self.assert_(p.is_ok())
        self.assert_(p.x==1 and p.y==2 and p.z==3)

        p.x,p.y,p.z = 4,5,6
        self.assert_(p.is_ok())
        self.assert_(p.x==4 and p.y==5 and p.z==6)
        self.assert_( p.coords()==(p.x,p.y,p.z) )


    def test_is_in_rectangle(self):

        p1,p2,p3 = self.Point(0,0,0),self.Point(1,1,1),self.Point(0.5,0.5,0.5)

        self.assert_( p1.is_in_rectangle(p1,p2) == 0 )
        self.assert_( p1.is_in_rectangle(p2,p1) == 0 )
        self.assert_( p3.is_in_rectangle(p1,p2) == 1 )
        self.assert_( p3.is_in_rectangle(p2,p1) == -1 )
        self.assert_( p1.is_in_rectangle(p2,p3) == -1)
        self.assert_( p2.is_in_rectangle(p1,p3) == -1)

        self.assert_( p1.is_in_rectangle((0,0,0),(1,1,1)) == 0 )
        self.assert_( p1.is_in_rectangle((1,1,1),(0,0,0)) == 0 )
        self.assert_( p3.is_in_rectangle((0,0,0),(1,1,1)) == 1 )
        self.assert_( p3.is_in_rectangle((1,1,1),(0,0,0)) == -1 )
        self.assert_( p1.is_in_rectangle((1,1,1),p3) == -1)
        self.assert_( p2.is_in_rectangle((0,0,0),p3) == -1)

        self.assert_(p1.is_ok())
        self.assert_(p2.is_ok())
        self.assert_(p3.is_ok())


    def test_distance(self):

        # Distance from Point
        p1,p2 = self.Point(0,0), self.Point(3,4)
        self.assert_(p1.distance(p2)==5.)
        self.assert_(p1.is_ok())
        self.assert_(p2.is_ok())

        # Distance from tuple Point
        self.assert_(p1.distance([3,4])==5.)
        self.assert_(p1.is_ok())


        # Distance from Segment
        s = gts.Segment(gts.Vertex(-1,0,-2),gts.Vertex(1,0,-2))
        self.assert_(p1.distance(s)==2.)
        self.assert_(p1.is_ok())
        self.assert_(s.is_ok())

        # Distance from Triangle
        t = gts.Triangle(gts.Vertex(1,0,-2), 
                         gts.Vertex(-1,1,-2), 
                         gts.Vertex(-1,-1,-2))
        self.assert_(p1.distance(t)==2.)
        self.assert_(p1.is_ok())
        self.assert_(t.is_ok())


    def test_distance2(self):

        # Distance squared from Point
        p1,p2 = self.Point(0,0), self.Point(3,4)
        self.assert_(p1.distance2(p2)==25.)
        self.assert_(p1.distance2((3,4))==25.)
        self.assert_(p1.is_ok())
        self.assert_(p2.is_ok())

        # Distance squared from Segment
        s = gts.Segment(gts.Vertex(-1,0,-2),gts.Vertex(1,0,-2))
        self.assert_(p1.distance2(s)==4.)
        self.assert_(p1.is_ok())
        self.assert_(s.is_ok())

        # Distance from Triangle
        t = gts.Triangle(gts.Vertex(1,0,-2), 
                         gts.Vertex(-1,1,-2), 
                         gts.Vertex(-1,-1,-2))
        self.assert_(p1.distance2(t)==4.)
        self.assert_(p1.is_ok())
        self.assert_(t.is_ok())


    def test_orientation_3d(self):

        p1,p2,p3 = self.Point(0,0), self.Point(1,0), self.Point(0,1)

        self.assert_(self.Point(0,0,0).orientation_3d(p1,p2,p3)==0)
        self.assert_(self.Point(0,0,1).orientation_3d(p1,p2,p3)<0)
        self.assert_(self.Point(0,0,-1).orientation_3d(p1,p2,p3)>0)

        self.assert_(self.Point(0,0,0).orientation_3d(p1,p3,p2)==0)
        self.assert_(self.Point(0,0,1).orientation_3d(p1,p3,p2)>0)
        self.assert_(self.Point(0,0,-1).orientation_3d(p1,p3,p2)<0)
        self.assert_(self.Point(0,0,-1).orientation_3d((0,0),p3,[1,0])<0)

        self.assert_(p1.is_ok())
        self.assert_(p2.is_ok())
        self.assert_(p3.is_ok())


    def test_orientation_3d_sos(self):

        p1,p2,p3 = self.Point(0,0), self.Point(1,0), self.Point(0,1)

        self.assert_(self.Point(0,0,1).orientation_3d_sos(p1,p2,p3)==-1)
        self.assert_(self.Point(0,0,-1).orientation_3d_sos(p1,p2,p3)==1)

        self.assert_(self.Point(0,0,1).orientation_3d_sos(p1,p3,p2)==1)
        self.assert_(self.Point(0,0,-1).orientation_3d_sos(p1,p3,p2)==-1)

        # *** ATTENTION ***
        # Degenerate cases -- inconsistent results;  why?
#        self.assert_(self.Point(0,0,0).orientation_3d_sos(p1,p2,p3)==1)
#        self.assert_(self.Point(0,0,0).orientation_3d_sos(p1,p3,p2)==-1)
        # *** ATTENTION ***

        self.assert_(p1.is_ok())
        self.assert_(p2.is_ok())
        self.assert_(p3.is_ok())


    def test_is_in_circle(self):

        # Test using Points
        p1,p2,p3 = self.Point(1,0), self.Point(-1,1), self.Point(-1,-1)
        self.assert_(self.Point(0,0).is_in_circle(p1,p2,p3)==1)
        self.assert_(self.Point(2,0).is_in_circle(p1,p2,p3)==-1)
        self.assert_(p1.is_in_circle(p1,p2,p3)==0)
        self.assert_(p1.is_ok())
        self.assert_(p2.is_ok())
        self.assert_(p3.is_ok())


        # Test using a triangle
        p1,p2,p3 = gts.Vertex(1,0), gts.Vertex(-1,1), gts.Vertex(-1,-1)
        t = gts.Triangle(p1,p2,p3)
        self.assert_(self.Point(0,0).is_in_circle(t)==1)
        self.assert_(self.Point(2,0).is_in_circle(t)==-1)
        self.assert_(p1.is_in_circle(t)==0)
        self.assert_(p1.is_ok())
        self.assert_(p2.is_ok())
        self.assert_(p3.is_ok())
        self.assert_(t.is_ok())

        # Try using both Point and Triangle arguments (should fail)
        try:
            self.assert_(self.Point(0,0).is_in_circle(p1,t)==1)
        except:
            pass
        else:
            raise RuntimeError, 'Did not catch faulty args'
        try:
            self.assert_(self.Point(0,0).is_in_circle(p1,p2,t)==1)
        except:
            pass
        else:
            raise RuntimeError, 'Did not catch faulty args'

        self.assert_(p1.is_ok())
        self.assert_(p2.is_ok())
        self.assert_(p3.is_ok())
        self.assert_(t.is_ok())


    def test_is_in(self):

        p1,p2,p3 = gts.Vertex(1,0), gts.Vertex(-1,1), gts.Vertex(-1,-1)
        t = gts.Triangle(p1,p2,p3)
        self.assert_(self.Point(0,0).is_in(t)>0)
        self.assert_(self.Point(2,0).is_in(t)<0)
        self.assert_(p1.is_in(t)==0)

        self.assert_(p1.is_ok())
        self.assert_(p2.is_ok())
        self.assert_(p3.is_ok())
        self.assert_(t.is_ok())


    def test_is_inside(self):

        p1 = self.Point(0,0,0)
        p2 = self.Point(10,0,0)
        s = gts.tetrahedron()

        self.assert_(p1.is_inside(s))
        self.assert_(not p2.is_inside(s))

        for f in s:
            f.revert()

        self.assert_(not p1.is_inside(s))
        self.assert_(p2.is_inside(s))


    def test_closest(self):

        # Distance from Point
        p1,p2 = self.Point(0,0), self.Point(0,0)

        # Distance from Segment
        s = gts.Segment(gts.Vertex(-1,0,-2),gts.Vertex(1,0,-2))
        self.assert_(p1.closest(s,p2)==self.Point(0,0,-2))

        # Distance from Triangle
        t = gts.Triangle(gts.Vertex(1,0,-2), 
                         gts.Vertex(-1,1,-2), 
                         gts.Vertex(-1,-1,-2))
        self.assert_(p1.closest(t,p2)==self.Point(0,0,-2))

        self.assert_(p1.is_ok())
        self.assert_(p2.is_ok())
        self.assert_(s.is_ok())
        self.assert_(t.is_ok())

        
    def test_rotate(self):

        p1 = self.Point(1)
        p1.rotate(dy=1,a=radians(90))

        p2 = self.Point(0,0,-1)

        self.assert_( fabs(p1.x-p2.x)<1.e-9 )
        self.assert_( p1.y==p2.y )
        self.assert_( p1.z==p2.z )

        self.assert_(p1.is_ok())
        self.assert_(p2.is_ok())


    def test_scale(self):

        p = self.Point(1)
        p.scale(dx=2)
        self.assert_(p==self.Point(2))
        self.assert_(p.is_ok())


    def test_translate(self):

        p = self.Point()
        p.translate(1,2,3)
        self.assert_(p==self.Point(1,2,3))
        self.assert_(p.is_ok())


class TestVertexMethods(TestPointMethods):

    Point = gts.Vertex

    def test_new(self):

        v = self.Point()
        self.assert_(v.is_ok())

        self.assert_(isinstance(v,gts.Point))
        self.assert_(isinstance(v,gts.Vertex))


    def test_is_unattached(self):

        v1 = self.Point()
        self.assert_(v1.is_ok())
        self.assert_(v1.is_unattached())

        # Create a segment and check that the vertices are marked as attached
        v2 = self.Point()
        self.assert_(v2.is_ok())
        self.assert_(v2.is_unattached())
        s = gts.Segment(v1,v2)
        self.assert_(not v1.is_unattached())
        self.assert_(not v2.is_unattached())
        self.assert_(not s.v1.is_unattached())
        self.assert_(not s.v2.is_unattached())

        # Destroy segment and make sure that vertices are unattached
        self.assert_(s.is_ok())
        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        del s
        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v1.is_unattached())
        self.assert_(v2.is_unattached())


    def test_replace(self):

        v1,v2,v3,v4 = self.Point(0), self.Point(1), self.Point(2), self.Point(3)
        s = gts.Segment(v1,v2)
        self.assert_(s.v1.id == v1.id)

        self.assert_(s.is_ok())
        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v3.is_ok())
        self.assert_(v4.is_ok())

        # Try a straight-forward replacement of v1 with v3
        self.assert_(v1.id != v3.id)
        self.assert_(s.v1.id == v1.id)
        v1.replace(v3)
        self.assert_(v1.id != v3.id)
        self.assert_(s.v1.id == v3.id)

        self.assert_(s.is_ok())
        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v3.is_ok())
        self.assert_(v4.is_ok())

        # Try replacing v3 with itself (should be ignored)
        v3.replace(v3)
        self.assert_(s.v1.id == v3.id)

        self.assert_(s.is_ok())
        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v3.is_ok())
        self.assert_(v4.is_ok())

        # Try replacing v3 with itself, directly in the segment 
        # (should be ignored)
        s.v1.replace(v3)
        self.assert_(s.v1.id == v3.id)

        self.assert_(s.is_ok())
        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v3.is_ok())
        self.assert_(v4.is_ok())

        # Replace a vertex directly in the segment
        self.assert_(s.v1.id == v3.id)
        s.v1.replace(v4)
        self.assert_(s.v1.id == v4.id)
        self.assert_(v3.id != v4.id)

        # Replace a vertex using a sequence
        s.v1.replace([2,4,6])
        self.assert_(s.v1==gts.Point(2,4,6) or s.v2==gts.Point(2,4,6))

        # Final checkout
        self.assert_(s.is_ok())
        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v3.is_ok())
        self.assert_(v4.is_ok())


    def test_is_boundary(self):

        #         v4
        #      e3 e4 e5
        #   v1 e1 v2 e2 v3
        #      e6 e7 e8
        #         v5

        v1 = self.Point(-1,0)
        v2 = self.Point(0,0)
        v3 = self.Point(1,0)
        v4 = self.Point(0,1)
        v5 = self.Point(0,-1)
        v6 = self.Point(0.5,0.5)

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v1,v4)
        e4 = gts.Edge(v4,v2)
        e5 = gts.Edge(v4,v3)
        e6 = gts.Edge(v1,v5)
        e7 = gts.Edge(v5,v2)
        e8 = gts.Edge(v5,v3)

        s = gts.Surface()
        s.add(gts.Face(e1,e4,e3))
        s.add(gts.Face(e2,e5,e4))
        s.add(gts.Face(e1,e7,e6))
        s.add(gts.Face(e2,e8,e7))

        self.assert_(v1.is_boundary(s))
        self.assert_(not v2.is_boundary(s))
        self.assert_(v3.is_boundary(s))
        self.assert_(v4.is_boundary(s))
        self.assert_(v5.is_boundary(s))
        self.assert_(not v6.is_boundary(s))

        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v3.is_ok())
        self.assert_(v4.is_ok())
        self.assert_(v5.is_ok())
        self.assert_(v6.is_ok())
        self.assert_(e1.is_ok())
        self.assert_(e2.is_ok())
        self.assert_(e3.is_ok())
        self.assert_(e4.is_ok())
        self.assert_(e5.is_ok())
        self.assert_(e6.is_ok())
        self.assert_(e7.is_ok())
        self.assert_(e8.is_ok())
        self.assert_(s.is_ok())


    def test_contacts(self):

        #         v4
        #      e3 e4 e5
        #   v1 e1 v2 e2 v3
        #      e6 e7 e8
        #         v5

        v1a = self.Point(-1,0)
        v1b = self.Point(-1,0,1)
        v2 = self.Point(0,0)
        v3a = self.Point(1,0)
        v3b = self.Point(1,0,1)
        v4 = self.Point(0,1)
        v5 = self.Point(0,-1)
        v6 = self.Point(0.5,0.5)

        e1a = gts.Edge(v1a,v2)
        e1b = gts.Edge(v1b,v2)
        e2a = gts.Edge(v2,v3a)
        e2b = gts.Edge(v2,v3b)
        e3 = gts.Edge(v1a,v4)
        e4 = gts.Edge(v4,v2)
        e5 = gts.Edge(v4,v3a)
        e6 = gts.Edge(v1b,v5)
        e7 = gts.Edge(v5,v2)
        e8 = gts.Edge(v5,v3b)

        s = gts.Surface()
        s.add(gts.Face(e1a,e4,e3))
        s.add(gts.Face(e2a,e5,e4))
        s.add(gts.Face(e1b,e7,e6))
        s.add(gts.Face(e2b,e8,e7))
        
        # *** ATTENTION ***
        # Some of these results seem to be wrong
        self.assert_(v1a.contacts()==1)  # Not a contact vertex!
        self.assert_(v1b.contacts()==1)  # Not a contact vertex!
        self.assert_(v2.contacts()==2)
        self.assert_(v3a.contacts()==1)  # Not a contact vertex!
        self.assert_(v3b.contacts()==1)  # Not a contact vertex!
        self.assert_(v4.contacts()==1)
        self.assert_(v5.contacts()==1)
        self.assert_(v6.contacts()==0)
        # *** ATTENTION ***

        # *** ATTENTION ***
        # Need to write sever tests
        # *** ATTENTION ***

        self.assert_(v1a.is_ok())
        self.assert_(v1b.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v3a.is_ok())
        self.assert_(v3b.is_ok())
        self.assert_(v4.is_ok())
        self.assert_(v5.is_ok())
        self.assert_(v6.is_ok())

        self.assert_(e1a.is_ok())
        self.assert_(e1b.is_ok())
        self.assert_(e2a.is_ok())
        self.assert_(e2b.is_ok())
        self.assert_(e3.is_ok())
        self.assert_(e4.is_ok())
        self.assert_(e5.is_ok())
        self.assert_(e6.is_ok())
        self.assert_(e7.is_ok())
        self.assert_(e8.is_ok())


    def test_is_connected(self):

        v1 = self.Point(0)
        v2 = self.Point(1)
        v3 = self.Point(2)

        self.assert_(not v1.is_connected(v2))
        self.assert_(not v1.is_connected(v3))
        self.assert_(not v2.is_connected(v3))

        s1 = gts.Segment(v1,v2)
        s2 = gts.Segment(v2,v3)

        self.assert_(v1.is_connected(v2))
        self.assert_(not v1.is_connected(v3))
        self.assert_(v2.is_connected(v3))

        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v3.is_ok())
        self.assert_(s1.is_ok())
        self.assert_(s2.is_ok())


    def test_neighbors(self):

        v1 = self.Point(0,1)
        self.assert_(v1.neighbors()==())

        v2 = self.Point(1,0)
        e1 = gts.Edge(v1,v2)

        self.assert_(v1.neighbors()==(v2,))

        v3 = self.Point(10,0)
        e2 = gts.Edge(v1,v3)
        v4 = self.Point(12,0)
        e3 = gts.Edge(v1,v4)
        self.assert_(len(v1.neighbors())==3)
        self.assert_(v2 in v1.neighbors())
        self.assert_(v3 in v1.neighbors())
        self.assert_(v4 in v1.neighbors())

        e4 = gts.Edge(v2,v3)
        f = gts.Face(e1,e2,e4)
        s = gts.Surface()
        s.add(f)

        self.assert_(len(v1.neighbors(s))==2)
        self.assert_(v2 in v1.neighbors(s))
        self.assert_(v3 in v1.neighbors(s))
        self.assert_(not v4 in v1.neighbors(s))

        self.assert_(v3.is_ok())
        del v3
        x = list(v1.neighbors())
        x.remove(v2)
        x.remove(v4)
        self.assert_(len(x)==1)
        self.assert_(x[0]==self.Point(10,0))

        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v4.is_ok())
        self.assert_(e1.is_ok())
        self.assert_(e2.is_ok())
        self.assert_(e3.is_ok())
        self.assert_(e4.is_ok())


    def test_faces(self):

        #         v4
        #      e3 e4 e5
        #   v1 e1 v2 e2 v3
        #      e6 e7 e8
        #         v5

        v1 = self.Point(-1,0)
        v2 = self.Point(0,0)
        v3 = self.Point(1,0)
        v4 = self.Point(0,1)
        v5 = self.Point(0,-1)
        v6 = self.Point(0.5,0.5)

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v1,v4)
        e4 = gts.Edge(v4,v2)
        e5 = gts.Edge(v4,v3)
        e6 = gts.Edge(v1,v5)
        e7 = gts.Edge(v5,v2)
        e8 = gts.Edge(v5,v3)

        f1 = gts.Face(e1,e4,e3)
        f2 = gts.Face(e2,e5,e4)
        f3 = gts.Face(e1,e7,e6)
        f4 = gts.Face(e2,e8,e7)

        s1 = gts.Surface()
        s1.add(f1)
        s1.add(f2)

        s2 = gts.Surface()
        s2.add(f3)
        s2.add(f4)

        faces = v1.faces()
        self.assert_(len(faces)==2)
        self.assert_(f1 in faces)
        self.assert_(f3 in faces)

        faces = v1.faces(s1)
        self.assert_(len(faces)==1)
        self.assert_(f1 in faces)
        self.assert_(not f3 in faces)

        faces = v2.faces()
        self.assert_(len(faces)==4)
        self.assert_(f1 in faces)
        self.assert_(f2 in faces)
        self.assert_(f3 in faces)
        self.assert_(f4 in faces)

        faces = v2.faces(s2)
        self.assert_(len(faces)==2)
        self.assert_(not f1 in faces)
        self.assert_(not f2 in faces)
        self.assert_(f3 in faces)
        self.assert_(f4 in faces)

        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v3.is_ok())
        self.assert_(v4.is_ok())
        self.assert_(v5.is_ok())
        self.assert_(v6.is_ok())
        self.assert_(e1.is_ok())
        self.assert_(e2.is_ok())
        self.assert_(e3.is_ok())
        self.assert_(e4.is_ok())
        self.assert_(e5.is_ok())
        self.assert_(e6.is_ok())
        self.assert_(e7.is_ok())
        self.assert_(e8.is_ok())
        self.assert_(s1.is_ok())
        self.assert_(s2.is_ok())


    def test_encroaches(self):

        v1 = self.Point(-1,0,100)
        v2 = self.Point(1,0,-100)
        e = gts.Edge(v1,v2)

        self.assert_(self.Point(0).encroaches(e))
        self.assert_(self.Point(0.999999).encroaches(e))
        self.assert_(self.Point(0,0.999999).encroaches(e))
        self.assert_(self.Point(0,0,0.999999).encroaches(e))

        self.assert_(not self.Point(1).encroaches(e))
        self.assert_(not self.Point(0,1).encroaches(e))

        self.assert_(self.Point(0,0,10).encroaches(e))


    def test_triangles(self):

        v = self.Point()

        self.assert_(v.is_ok())

        self.assert_(len(v.triangles())==0)
        
        t = gts.Triangle(v,gts.Vertex(0,1),gts.Vertex(1,0))
        
        self.assert_(t.is_ok())

        self.assert_(len(v.triangles())==1)
        self.assert_(t in v.triangles())

        f = gts.Face(v,gts.Vertex(0,1),gts.Vertex(1,0))

        self.assert_(f.is_ok())

        self.assert_(len(v.triangles())==2)
        self.assert_(t in v.triangles())
        self.assert_(f in v.triangles())


class TestSegmentMethods(unittest.TestCase):

    Segment = gts.Segment

    def test_new(self):

        # Check for segment class
        v1,v2 = gts.Vertex(0,0,0), gts.Vertex(1,1,1)
        s1 = self.Segment(v1,v2)
        self.assert_(isinstance(s1,gts.Segment))
        self.assert_(not v1.is_unattached())
        self.assert_(not v2.is_unattached())

        self.assert_(s1.is_ok())
        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())

        # Create a duplicate segment from vertices (should result in same 
        # segment)
        s2 = self.Segment(v2,v1)
        self.assert_(s1.id==s2.id)
        self.assert_(id(s1)==id(s2))

        self.assert_(s1.is_ok())
        self.assert_(s2.is_ok())
        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())

        # Try to create a degenerate segment (should fail)
        v = gts.Vertex(0,0,0)
        try:
            s = self.Segment(v,v)
        except:
            pass
        else:
            msg = 'Allowed creation of degenerate segment'
            raise RuntimeError, msg

        self.assert_(s1.is_ok())
        self.assert_(s2.is_ok())
        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())


    def test_subclass(self):

        Super = self.Segment

        class Segment(Super):

            def __init__(self,v1,v2,msg):
                self.msg = msg
                Super.__init__(self,v1,v2)

            def foo(self):
                return self.msg

        v1 = gts.Vertex(0)
        v2 = gts.Vertex(1)
        s = Segment(v1,v2,'bar')
        self.assert_(v1 in [s.v1,s.v2])
        self.assert_(v2 in [s.v1,s.v2])
        self.assert_(s.foo()=='bar')

        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(s.is_ok())


    def test_is_ok(self):

        if TEST_IS_OK:
            v1,v2 = gts.Vertex(0,0,0),gts.Vertex(1,1,1)

            s1 = self.Segment(v1,v2)
            self.assert_(s1.is_ok())        

            # Duplicate segment (should fail)
            s2 = self.Segment(v1,v2)
            self.assert_(not s2.is_ok())
            del s2

            # Degenerate segment (created via vertex replacement) should fail.
            self.assert_(s1.is_ok())
            v1.replace(v2)
            self.assert_(not s1.is_ok())


    def test_compare(self):

        s1 = self.Segment(gts.Vertex(0,0,0),gts.Vertex(1,1,1))
        s2 = self.Segment(gts.Vertex(0,0,0),gts.Vertex(1,1,1))
        s3 = self.Segment(gts.Vertex(1,1,1),gts.Vertex(0,0,0))
        s4 = self.Segment(gts.Vertex(1,0,0),gts.Vertex(0,1,1))

        self.assert_(s1.id!=s2.id)
        self.assert_(s1==s2)
        self.assert_(s1==s3)
        self.assert_(s2==s3)
        self.assert_(s1!=s4)
        self.assert_(s2!=s4)
        self.assert_(s3!=s4)

        self.assert_(s1.is_ok())
        self.assert_(s2.is_ok())
        self.assert_(s3.is_ok())
        self.assert_(s4.is_ok())


    def test_getset(self):

        v1,v2 = gts.Vertex(0,0,0), gts.Vertex(1,1,1)
        s = self.Segment(v1,v2)

        self.assert_(id(s.v1)==id(v1))
        self.assert_(id(s.v2)==id(v2))

        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(s.is_ok())


        # These should fail
        try:
            s.v1 = v1
        except:
            pass
        else:
            msg = 'Attribute should be read only'
            raise RuntimeError, msg
        try:
            s.v2 = v2
        except:
            pass
        else:
            msg = 'Attribute should be read only'
            raise RuntimeError, msg

        # Vertices should be accessible even if originals are deleted
        del v1, v2

        self.assert_(s.v1.is_ok())
        self.assert_(s.v2.is_ok())
        self.assert_(s.is_ok())

        self.assert_(s.v1.distance(s.v2)==sqrt(3))  # Any calculation


    def test_intersects(self):
        
        v1 = gts.Vertex(0,-1)
        v2 = gts.Vertex(0,1)
        s1 = self.Segment(v1,v2)

        v3 = gts.Vertex(-1,0)
        v4 = gts.Vertex(1,0)
        s2 = self.Segment(v3,v4)

        self.assert_(s1.intersects(s2))
        self.assert_(s2.intersects(s1))

        # *** ATTENTION ***
        # This routine appears to consider the horizontal projections,
        # because the following Segment s2 is in a different vertical
        # plane from s1.
        v3 = gts.Vertex(-1,0,1)
        v4 = gts.Vertex(1,0,1)
        s2 = self.Segment(v3,v4)
        self.assert_(s1.intersects(s2)==1)
        self.assert_(s2.intersects(s1)==1)
        # *** ATTENTION ***

        v3 = gts.Vertex(-3,0,1)
        v4 = gts.Vertex(-2,0,1)
        s2 = self.Segment(v3,v4)

        self.assert_(s1.intersects(s2)==-1)
        self.assert_(s2.intersects(s1)==-1)

        v3 = gts.Vertex(-1,-1)
        v4 = gts.Vertex(1,-1)
        s2 = self.Segment(v3,v4)

        self.assert_(s1.intersects(s2)==0)
        self.assert_(s2.intersects(s1)==0)


    def test_connects(self):

        v1 = gts.Vertex(0,-1)
        v2 = gts.Vertex(0,1)
        s1 = self.Segment(v1,v2)

        v3 = gts.Vertex(-1,0)
        v4 = gts.Vertex(1,0)
        s2 = self.Segment(v3,v4)

        self.assert_(s1.connects(v1,v2))
        self.assert_(s2.connects(v3,v4))

        self.assert_(not s2.connects(v1,v2))
        self.assert_(not s1.connects(v3,v4))


    def test_touches(self):
        
        v1 = gts.Vertex(0,-1)
        v2 = gts.Vertex(0,1)
        s1 = self.Segment(v1,v2)

        v3 = gts.Vertex(-1,0)
        s2 = self.Segment(v2,v3)

        v4 = gts.Vertex(1,0)
        s3 = self.Segment(v3,v4)


        self.assert_(s1.touches(s2))
        self.assert_(s2.touches(s3))
        self.assert_(not s1.touches(s3))


    def test_midvertex(self):

        v1 = gts.Vertex(1)
        v2 = gts.Vertex(-1)
        s = self.Segment(v1,v2)

        self.assert_(s.midvertex() == gts.Vertex(0))


    def test_intersection(self):

        v1 = gts.Vertex(1,0,1)
        v2 = gts.Vertex(-1,1,1)
        v3 = gts.Vertex(-1,-1,1)
        t =  gts.Triangle(v1,v2,v3)

        # Segment intersects triangle
        s = self.Segment(gts.Vertex(0,0,2),gts.Vertex(0,0,0))
        self.assert_(s.intersection(t)==gts.Vertex(0,0,1))

        # Segment intersects triangle summit
        s = self.Segment(gts.Vertex(1,0,2),gts.Vertex(1,0,0))
        self.assert_(s.intersection(t) is v1)

        # Segment intersects triangle summit with boundary==False
        s = self.Segment(gts.Vertex(1,0,2),gts.Vertex(1,0,0))
        self.assert_(s.intersection(t,False) is None)

        # Segment in triangle's plane
        s = self.Segment(gts.Vertex(1,0,1),gts.Vertex(-1,0,1))
        self.assert_(s.intersection(t) is None)

        self.assert_(s.is_ok())
        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v3.is_ok())
        self.assert_(t.is_ok())


class TestEdgeMethods(TestSegmentMethods):

    Segment = gts.Edge

    def test_is_unattached(self):

        v1,v2,v3 = gts.Vertex(1,0,0), gts.Vertex(0,1,0), gts.Vertex(0,0,1)
        e1,e2,e3 = self.Segment(v1,v2), self.Segment(v2,v3), self.Segment(v3,v1)

        self.assert_(e1.is_unattached())
        self.assert_(e2.is_unattached())
        self.assert_(e3.is_unattached())
        
        # Create a triangle and check that the edges are marked as attached
        t = gts.Triangle(e1,e2,e3)
        self.assert_(not e1.is_unattached())
        self.assert_(not e2.is_unattached())
        self.assert_(not e3.is_unattached())

        # Destroy triangle and make sure that edges get detached
        del t
        self.assert_(e1.is_unattached())
        self.assert_(e2.is_unattached())
        self.assert_(e3.is_unattached())

        self.assert_(e1.is_ok())
        self.assert_(e2.is_ok())
        self.assert_(e3.is_ok())


#    def test_replace(self):
#
#        v1,v2,v3 = gts.Vertex(0,0),gts.Vertex(0,1),gts.Vertex(1,1)
#        e1,e2,e3 = self.Segment(v1,v2),self.Segment(v2,v3),self.Segment(v3,v1)
#
#        v4,v5 = gts.Vertex(0,0,1),gts.Vertex(0,1,1)
#        e4 = self.Segment(v4,v5)
#
#        t = gts.Triangle(e1,e2,e3)
#        e1.replace(e4)
#
#        self.assert_(v1.is_ok())
#        self.assert_(v2.is_ok())
#        self.assert_(v3.is_ok())
#        self.assert_(v4.is_ok())
#        self.assert_(v5.is_ok())
#
#        self.assert_(e1.is_ok())
#        self.assert_(e2.is_ok())
#        self.assert_(e3.is_ok())
#        self.assert_(e4.is_ok())
#
#        self.assert_(t.is_ok())



    def test_face_number(self):

        #         v4
        #      e3 e4 e5
        #   v1 e1 v2 e2 v3
        #      e6 e7 e8
        #         v5

        v1 = gts.Vertex(-1,0)
        v2 = gts.Vertex(0,0)
        v3 = gts.Vertex(1,0)
        v4 = gts.Vertex(0,1)
        v5 = gts.Vertex(0,-1)
        v6 = gts.Vertex(0.5,0.5)

        e1 = self.Segment(v1,v2)
        e2 = self.Segment(v2,v3)
        e3 = self.Segment(v1,v4)
        e4 = self.Segment(v4,v2)
        e5 = self.Segment(v4,v3)
        e6 = self.Segment(v1,v5)
        e7 = self.Segment(v5,v2)
        e8 = self.Segment(v5,v3)

        s = gts.Surface()
        s.add(gts.Face(e1,e4,e3))
        s.add(gts.Face(e2,e5,e4))
        s.add(gts.Face(e1,e7,e6))
        s.add(gts.Face(e2,e8,e7))

        self.assert_(e1.face_number(s)==2)
        self.assert_(e2.face_number(s)==2)
        self.assert_(e3.face_number(s)==1)
        self.assert_(e4.face_number(s)==2)
        self.assert_(e5.face_number(s)==1)
        self.assert_(e6.face_number(s)==1)
        self.assert_(e7.face_number(s)==2)
        self.assert_(e8.face_number(s)==1)

        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v3.is_ok())
        self.assert_(v4.is_ok())
        self.assert_(v5.is_ok())
        self.assert_(v6.is_ok())
        self.assert_(e1.is_ok())
        self.assert_(e2.is_ok())
        self.assert_(e3.is_ok())
        self.assert_(e4.is_ok())
        self.assert_(e5.is_ok())
        self.assert_(e6.is_ok())
        self.assert_(e7.is_ok())
        self.assert_(e8.is_ok())
        self.assert_(s.is_ok())


    def test_belongs_to_tetrahedron(self):

        e = self.Segment(gts.Vertex(0),gts.Vertex(1))
        self.assert_(not e.belongs_to_tetrahedron())
        
        s = gts.tetrahedron()
        self.assert_(list(s)[0].e1.belongs_to_tetrahedron())


    def test_contacts(self):

        #         v4
        #      e3 e4 e5
        #   v1 e1 v2 e2 v3
        #      e6 e7 e8
        #         v5

        v1 = gts.Vertex(-1,0)
        v2 = gts.Vertex(0,0)
        v3 = gts.Vertex(1,0)
        v4 = gts.Vertex(0,1)
        v5 = gts.Vertex(0,-1)
        v6 = gts.Vertex(0.5,0.5)

        e1 = self.Segment(v1,v2)
        e2 = self.Segment(v2,v3)
        e3 = self.Segment(v1,v4)
        e4 = self.Segment(v4,v2)
        e5 = self.Segment(v4,v3)
        e6 = self.Segment(v1,v5)
        e7 = self.Segment(v5,v2)
        e8 = self.Segment(v5,v3)

        s = gts.Surface()
        s.add(gts.Face(e1,e4,e3))
        s.add(gts.Face(e2,e5,e4))
        s.add(gts.Face(e1,e7,e6))
        s.add(gts.Face(e2,e8,e7))

        # *** ATTENTION ***
        # I can't make any sense of these results.  The parent might be
        # adding an extra triangle, but even then I can't see
        # how these results would be obtained.
        self.assert_(e1.contacts()==2)
        self.assert_(e2.contacts()==2)
        self.assert_(e3.contacts()==2)
        self.assert_(e4.contacts()==2)
        self.assert_(e5.contacts()==2)
        self.assert_(e6.contacts()==2)
        self.assert_(e7.contacts()==2)
        self.assert_(e8.contacts()==2)
        # *** ATTENTION ***

        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v3.is_ok())
        self.assert_(v4.is_ok())
        self.assert_(v5.is_ok())
        self.assert_(v6.is_ok())
        self.assert_(e1.is_ok())
        self.assert_(e2.is_ok())
        self.assert_(e3.is_ok())
        self.assert_(e4.is_ok())
        self.assert_(e5.is_ok())
        self.assert_(e6.is_ok())
        self.assert_(e7.is_ok())
        self.assert_(e8.is_ok())
        self.assert_(s.is_ok())


    def test_is_boundary(self):

        #         v4
        #      e3 e4 e5
        #   v1 e1 v2 e2 v3
        #      e6 e7 e8
        #         v5

        v1 = gts.Vertex(-1,0)
        v2 = gts.Vertex(0,0)
        v3 = gts.Vertex(1,0)
        v4 = gts.Vertex(0,1)
        v5 = gts.Vertex(0,-1)
        v6 = gts.Vertex(0.5,0.5)

        e1 = self.Segment(v1,v2)
        e2 = self.Segment(v2,v3)
        e3 = self.Segment(v1,v4)
        e4 = self.Segment(v4,v2)
        e5 = self.Segment(v4,v3)
        e6 = self.Segment(v1,v5)
        e7 = self.Segment(v5,v2)
        e8 = self.Segment(v5,v3)

        s = gts.Surface()
        s.add(gts.Face(e1,e4,e3))
        s.add(gts.Face(e2,e5,e4))
        s.add(gts.Face(e1,e7,e6))
        s.add(gts.Face(e2,e8,e7))

        self.assert_(e3.is_boundary(s))
        self.assert_(e5.is_boundary(s))
        self.assert_(e6.is_boundary(s))
        self.assert_(e8.is_boundary(s))

        self.assert_(not e1.is_boundary(s))
        self.assert_(not e2.is_boundary(s))
        self.assert_(not e4.is_boundary(s))
        self.assert_(not e7.is_boundary(s))


class TestTriangleMethods(unittest.TestCase):

    Triangle = gts.Triangle

    def test_new(self):

        # Create some edges (NOTE: These are reused in the next test)
        v1,v2,v3 = gts.Vertex(1,0,0), gts.Vertex(0,1,0), gts.Vertex(0,0,1)
        e1,e2,e3 = gts.Edge(v1,v2), gts.Edge(v2,v3), gts.Edge(v1,v3)
        e3b =  gts.Edge(v3,v1)  # Order of vertices reversed

        # Create triangles.  Order the segments differently each time
        t = self.Triangle(e1,e2,e3)
        self.assert_(t.is_ok())
        self.assert_(isinstance(t,gts.Triangle))
        t = self.Triangle(e1,e3,e2)
        self.assert_(t.is_ok())
        self.assert_(isinstance(t,gts.Triangle))
        t = self.Triangle(e3,e1,e2)
        self.assert_(t.is_ok())
        self.assert_(isinstance(t,gts.Triangle))
        t = self.Triangle(e1,e2,e3b)
        self.assert_(t.is_ok())
        self.assert_(isinstance(t,gts.Triangle))
        t = self.Triangle(e1,e3b,e2)
        self.assert_(t.is_ok())
        self.assert_(isinstance(t,gts.Triangle))
        t = self.Triangle(e3b,e1,e2)
        self.assert_(t.is_ok())
        self.assert_(t.e1.is_ok())
        self.assert_(t.e2.is_ok())
        self.assert_(t.e3.is_ok())
        self.assert_(isinstance(t,gts.Triangle))

        # Check creation with vertices
        t = self.Triangle(gts.Vertex(1,0,0),gts.Vertex(0,1,0),gts.Vertex(0,0,1))
        self.assert_(t.is_ok())
        self.assert_(t.e1.is_ok())
        self.assert_(t.e2.is_ok())
        self.assert_(t.e3.is_ok())
        self.assert_(isinstance(t,gts.Triangle))

        # Try re-using some vertices and edges after deleting the triangle
        del t
        v4 = gts.Vertex(0,0,0)
        e2 = gts.Edge(v2,v4)
        e3 = gts.Edge(v1,v4)
        t = self.Triangle(e1,e2,e3)
        self.assert_(t.is_ok())
        self.assert_(isinstance(t,gts.Triangle))

        # Check creation where vertices are equal, but are different 
        # objects (should fail)
        try:
            v1 = gts.Vertex(1,0,0)
            v2 = gts.Vertex(0,1,0)
            t = self.Triangle(gts.Edge(v1,v2),
                              gts.Edge(v2,gts.Vertex(0,0,1)),
                              gts.Edge(v1,gts.Vertex(0,0,1)))
        except:
            pass
        else:
            raise RuntimeError, 'Did not identify non-connecting edges'

        # Check mixed creation (should fail)
        try:
            t = self.Triangle(e1,gts.Vertex(1,0,0),gts.Vertex(0,1,0))
        except:
            pass
        else:
            raise Error,'Did not identify mixed-type arguments'
        try:
            t = self.Triangle(gts.Vertex(1,0,0),gts.Vertex(0,1,0),e1)
        except:
            pass
        else:
            raise Error,'Did not identify mixed-type arguments'


    def test_subclass(self):

        Super = self.Triangle

        class Triangle(Super):

            def __init__(self,o1,o2,o3,msg):
                self.msg = msg
                Super.__init__(self,o1,o2,o3)

            def foo(self):
                return self.msg

        v1 = gts.Vertex(0,0)
        v2 = gts.Vertex(0,1)
        v3 = gts.Vertex(1,0)
        t = Triangle(v1,v2,v3,'bar')
        self.assert_(v1 in t.vertices())
        self.assert_(v2 in t.vertices())
        self.assert_(v3 in t.vertices())
        self.assert_(t.foo()=='bar')

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v3,v1)
        t = Triangle(e1,e2,e3,'bar')
        self.assert_(v1 in t.vertices())
        self.assert_(v2 in t.vertices())
        self.assert_(v3 in t.vertices())

        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(t.is_ok())

        


    def test_is_ok(self):

        if TEST_IS_OK:
            v1,v2,v3 = gts.Vertex(0,0,1),gts.Vertex(0,1,0),gts.Vertex(0,0,1)
            e1,e2,e3 = gts.Edge(v1,v2),gts.Edge(v2,v3),gts.Edge(v3,v1)

            t = self.Triangle(e1,e2,e3)
            self.assert_(t.is_ok())

            # Duplicate triangle
            t2 = self.Triangle(e1,e2,e3)
            self.assert_(not t2.is_ok())
            del t2

            # Degenerate triangle (created via vertex replacement)
            self.assert_(t.is_ok())
            v3.replace(v1)
            self.assert_(not t.is_ok())

    
    def test_area(self):
        t = self.Triangle( gts.Vertex(0,0), gts.Vertex(1,0), gts.Vertex(0,1) )
        self.assert_(t.is_ok())
        self.assert_(t.area()==0.5)


    def test_perimeter(self):
        t = self.Triangle( gts.Vertex(0,0), gts.Vertex(3,0), gts.Vertex(3,4) )
        self.assert_(t.is_ok())
        self.assert_(t.perimeter()==12)


    def test_quality(self):
        t = self.Triangle( gts.Vertex(0,0), gts.Vertex(3,0), gts.Vertex(3,4) )
        self.assert_(t.is_ok())
        self.assert_(fabs(t.quality()-(sqrt(6)/12)/(sqrt(sqrt(3))/6))<1.e-9)


    def test_normal(self):

        # Normal pointing upward
        t = self.Triangle( gts.Vertex(0,0), gts.Vertex(3,0), gts.Vertex(3,4) )
        self.assert_(t.is_ok())
        self.assert_(t.normal()==(0,0,12))

        # Normal pointing downward
        t = self.Triangle( gts.Vertex(0,0), gts.Vertex(3,4), gts.Vertex(3,0) )
        self.assert_(t.is_ok())
        self.assert_(t.normal()==(0,0,-12))


    def test_revert(self):

        # Normal pointing upward
        t = self.Triangle( gts.Vertex(0,0), gts.Vertex(3,0), gts.Vertex(3,4) )
        self.assert_(t.is_ok())
        self.assert_(t.normal()==(0,0,12))

        # Normal pointing downward
        t.revert()
        self.assert_(t.is_ok())
        self.assert_(t.normal()==(0,0,-12))


    def test_orientation(self):

        # Normal pointing upward
        t = self.Triangle( gts.Vertex(0,0), gts.Vertex(3,0), gts.Vertex(3,4) )
        self.assert_(t.is_ok())
        self.assert_(t.orientation()>0)

        # Normal pointing upward using arrays
        t = self.Triangle( [0,0], [3,0], [3,4] )
        self.assert_(t.is_ok())
        self.assert_(t.orientation()>0)

        # Normal pointing downward
        t.revert()
        self.assert_(t.is_ok())
        self.assert_(t.orientation()<0)

        # Colinear case
        t = self.Triangle([0,0], [3,0], [2,0] )
        self.assert_(t.is_ok())
        self.assert_(t.orientation()==0)


    def test_angle(self):

        # Normals pointing upward and horizontally
        t1 = self.Triangle( gts.Vertex(0,0), gts.Vertex(3,0), gts.Vertex(3,4) )
        self.assert_(t1.is_ok())
        t2 = self.Triangle( gts.Vertex(0,0,0), 
                           gts.Vertex(0,3,0), 
                           gts.Vertex(0,3,4) )
        self.assert_(t2.is_ok())
        self.assert_(t1.angle(t2)==-pi/2)


    def test_is_compatible(self):

        v1 = gts.Vertex(0,0)
        v2 = gts.Vertex(1,0)
        v3 = gts.Vertex(0,1)
        v4 = gts.Vertex(-1,0)

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v3,v1)
        e4 = gts.Edge(v3,v4)
        e5 = gts.Edge(v4,v1)

        t1, t2 = self.Triangle(e1,e2,e3), self.Triangle(e3,e4,e5)
        self.assert_(t1.is_ok())
        self.assert_(t2.is_ok())
        self.assert_(t1.is_compatible(t2))

        # Turn the second triangle inside out; should be incompatible
        t2.revert()
        self.assert_(t2.is_ok())
        self.assert_(not t1.is_compatible(t2))


    def test_common_edge(self):

        v1a = gts.Vertex(0,0)
        v1b = gts.Vertex(0,0)
        v2 = gts.Vertex(1,0)
        v3 = gts.Vertex(0,1)
        v4 = gts.Vertex(-1,0)

        e1 = gts.Edge(v1a,v2)
        e2 = gts.Edge(v2,v3)
        e3a = gts.Edge(v3,v1a)
        e3b = gts.Edge(v3,v1b)
        e4 = gts.Edge(v3,v4)
        e5a = gts.Edge(v4,v1a)
        e5b = gts.Edge(v4,v1b)


        t1 = self.Triangle(e1,e2,e3a)
        t2 = self.Triangle(e3a,e4,e5a)
        t3 = self.Triangle(e3b,e4,e5b)

        self.assert_(t1.common_edge(t2) is e3a)
        self.assert_(t1.common_edge(t3) is None)

        self.assert_(t1.is_ok())
        self.assert_(t2.is_ok())
        self.assert_(t3.is_ok())


    def test_opposite(self):

        v1 = gts.Vertex(0,0)
        v2 = gts.Vertex(1,0)
        v3 = gts.Vertex(0,1)

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v3,v1)

        t = self.Triangle(e1,e2,e3)

        self.assert_(t.opposite(e1)==v3)
        self.assert_(t.opposite(e2)==v1)
        self.assert_(t.opposite(e3)==v2)

        self.assert_(t.opposite(v1)==e2)
        self.assert_(t.opposite(v2)==e3)
        self.assert_(t.opposite(v3)==e1)

        self.assert_(t.is_ok())


    def test_getset(self):

        v1 = gts.Vertex(1,0,0)
        v2 = gts.Vertex(0,1,0)
        v3 = gts.Vertex(0,0,1)

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v1,v3)

        t = self.Triangle(e1,e2,e3)
        self.assert_(t.is_ok())

        self.assert_(t.e1.id==e1.id)
        self.assert_(t.e2.id==e2.id)
        self.assert_(t.e3.id==e3.id)

        del e1,e2,e3
        self.assert_(t.e1.is_ok())
        self.assert_(t.e2.is_ok())
        self.assert_(t.e3.is_ok())


    def test_compare(self):

        t1 = self.Triangle( gts.Vertex(0,0,1),
                            gts.Vertex(0,1,0),
                            gts.Vertex(0,0,1) )

        t2 = self.Triangle( gts.Vertex(0,0,1),
                            gts.Vertex(0,1,0),
                            gts.Vertex(0,0,1) )

        t3 = self.Triangle( gts.Vertex(0,1,0),
                            gts.Vertex(0,0,1),
                            gts.Vertex(0,0,1) )

        t4 = self.Triangle( gts.Vertex(0,0,0),
                            gts.Vertex(0,1,0),
                            gts.Vertex(0,0,1) )

        self.assert_(t1.is_ok())
        self.assert_(t2.is_ok())
        self.assert_(t3.is_ok())
        self.assert_(t4.is_ok())

        self.assert_(t1.id!=t2.id)
        self.assert_(t1==t2)
        self.assert_(t1==t3)
        self.assert_(t1!=t4)


    def test_vertices(self):

        v1,v2,v3 = gts.Vertex(0,0,1), gts.Vertex(0,1,0), gts.Vertex(0,0,1)
        vs = [v1,v2,v3]
        t = self.Triangle(v1,v2,v3)
        self.assert_(t.is_ok())

        for v in t.vertices():
            self.assert_(v.is_ok())
            vs.remove(v)

        self.assert_(len(vs)==0)


    def test_vertices(self):

        v1,v2,v3 = gts.Vertex(0,0,1), gts.Vertex(0,1,0), gts.Vertex(0,0,1)
        e1,e2,e3 = gts.Edge(v1,v2), gts.Edge(v2,v3), gts.Edge(v1,v3)

        t = self.Triangle(e1,e2,e3)

        self.assert_(t.vertex() == v3)
        del v1,v2,e1,e2,e3

        self.assert_(v3.is_ok())
        self.assert_(t.is_ok())


    def test_circumcenter(self):

        v1,v2,v3 = gts.Vertex(-1,0),gts.Vertex(1,0),gts.Vertex(0,sqrt(3))
        t = self.Triangle(v1,v2,v3)

        v = t.circumcenter()
        self.assert_(v.x == 0.)
        self.assert_( fabs(v.y - sqrt(3)/3.) < 1.e-9 )


    def test_is_stabbed(self):

        v1,v2,v3 = gts.Vertex(0,0), gts.Vertex(0,1), gts.Vertex(1,0)
        e1,e2,e3 = gts.Edge(v1,v2), gts.Edge(v1,v3), gts.Edge(v2,v3)
        t = self.Triangle(e1,e2,e3)

        self.assert_( not t.is_stabbed(gts.Point(0.3,0.3,1)) )
        self.assert_( t.is_stabbed(gts.Point(0.3,0.3,-1)) == t )
        self.assert_( t.is_stabbed(gts.Point(0,0,-1)) == v1 )
        self.assert_( t.is_stabbed(gts.Point(0,0.5,-1)) == e1 )


    def test_interpolate_height(self):

        v1,v2,v3 = gts.Vertex(0,0,2), gts.Vertex(0,1,2), gts.Vertex(1,0,2)
        e1,e2,e3 = gts.Edge(v1,v2), gts.Edge(v1,v3), gts.Edge(v2,v3)
        t = self.Triangle(e1,e2,e3)

        self.assert_(t.interpolate_height(gts.Point(0.2,0.2)) == 2)
        self.assert_(t.interpolate_height(gts.Point(-1,-1)) == 2)


class TestFaceMethods(TestTriangleMethods):

    Triangle = gts.Face

    def test_is_unattached(self):

        v1,v2,v3 = gts.Vertex(1,0,0), gts.Vertex(0,1,0), gts.Vertex(0,0,1)
        e1,e2,e3 = gts.Edge(v1,v2), gts.Edge(v2,v3), gts.Edge(v3,v1)
        f = self.Triangle(e1,e2,e3)
        self.assert_(f.is_ok())
        self.assert_(f.is_unattached())

        # Create a surface and attach the triangle
        s = gts.Surface()
        s.add(f)
        self.assert_(f.is_ok())
        self.assert_(not f.is_unattached())

        # Destroy surface and make sure that the triangle gets detached
        del s
        self.assert_(f.is_ok())
        self.assert_(f.is_unattached())


    def test_is_compatible(self):

        TestTriangleMethods.test_is_compatible(self)

        # Check if faces are compatible with a surface, then add it
        v1 = gts.Vertex(0,0)
        v2 = gts.Vertex(1,0)
        v3 = gts.Vertex(0,1)
        v4 = gts.Vertex(-1,0)

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v3,v1)
        e4 = gts.Edge(v3,v4)
        e5 = gts.Edge(v4,v1)
        s = gts.Surface()

        f = self.Triangle(e1,e2,e3)
        self.assert_(f.is_ok())
        self.assert_(f.is_compatible(s))

        s.add(f)
        self.assert_(f.is_ok())

        f = self.Triangle(e3,e4,e5)
        self.assert_(f.is_ok())
        self.assert_(f.is_compatible(s))

        # Turn inside out so that they are incompatible
        f.revert()
        self.assert_(f.is_ok())
        self.assert_(not f.is_compatible(s))


    def test_neighbor_number(self):

        #         v4
        #      e3 e4 e5
        #   v1 e1 v2 e2 v3
        #      e6 e7 e8
        #         v5

        v1 = gts.Vertex(-1,0)
        v2 = gts.Vertex(0,0)
        v3 = gts.Vertex(1,0)
        v4 = gts.Vertex(0,1)
        v5 = gts.Vertex(0,-1)
        v6 = gts.Vertex(0.5,0.5)

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v1,v4)
        e4 = gts.Edge(v4,v2)
        e5 = gts.Edge(v4,v3)
        e6 = gts.Edge(v1,v5)
        e7 = gts.Edge(v5,v2)
        e8 = gts.Edge(v5,v3)

        s = gts.Surface()

        f1 = self.Triangle(e1,e4,e3)
        s.add(f1)

        self.assert_(f1.neighbor_number(s)==0)

        f2 = self.Triangle(e2,e5,e4)
        s.add(f2)

        f3 = self.Triangle(e1,e7,e6)
        s.add(f3)

        f4 = self.Triangle(e2,e8,e7)
        s.add(f4)

        self.assert_(f1.neighbor_number(s)==2)


    def test_neighbors(self):

        #         v4
        #      e3 e4 e5
        #   v1 e1 v2 e2 v3
        #      e6 e7 e8
        #         v5

        v1 = gts.Vertex(-1,0)
        v2 = gts.Vertex(0,0)
        v3 = gts.Vertex(1,0)
        v4 = gts.Vertex(0,1)
        v5 = gts.Vertex(0,-1)
        v6 = gts.Vertex(0.5,0.5)

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v1,v4)
        e4 = gts.Edge(v4,v2)
        e5 = gts.Edge(v4,v3)
        e6 = gts.Edge(v1,v5)
        e7 = gts.Edge(v5,v2)
        e8 = gts.Edge(v5,v3)

        s = gts.Surface()

        f1 = self.Triangle(e1,e4,e3)
        s.add(f1)

        f2 = self.Triangle(e2,e5,e4)
        s.add(f2)

        f3 = self.Triangle(e1,e7,e6)
        s.add(f3)

        f4 = self.Triangle(e2,e8,e7)
        s.add(f4)

        neighbors = f1.neighbors(s)

        self.assert_(len(neighbors)==2)
        self.assert_(f2 in neighbors)
        self.assert_(f3 in neighbors)

        self.assert_(f1.is_ok())
        self.assert_(f2.is_ok())
        self.assert_(f3.is_ok())
        self.assert_(f4.is_ok())
        self.assert_(s.is_ok())


    def test_is_on(self):

        #         v4
        #      e3 e4 e5
        #   v1 e1 v2 e2 v3
        #      e6 e7 e8
        #         v5

        v1 = gts.Vertex(-1,0)
        v2 = gts.Vertex(0,0)
        v3 = gts.Vertex(1,0)
        v4 = gts.Vertex(0,1)
        v5 = gts.Vertex(0,-1)
        v6 = gts.Vertex(0.5,0.5)

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v1,v4)
        e4 = gts.Edge(v4,v2)
        e5 = gts.Edge(v4,v3)
        e6 = gts.Edge(v1,v5)
        e7 = gts.Edge(v5,v2)
        e8 = gts.Edge(v5,v3)

        s = gts.Surface()

        f1 = self.Triangle(e1,e4,e3)
        f2 = self.Triangle(e2,e5,e4)
        f3 = self.Triangle(e1,e7,e6)
        f4 = self.Triangle(e2,e8,e7)

        s.add(f1)
        self.assert_(f1.is_on(s))
        self.assert_(not f2.is_on(s))
        self.assert_(not f3.is_on(s))
        self.assert_(not f4.is_on(s))

        s.add(f2)
        self.assert_(f1.is_on(s))
        self.assert_(f2.is_on(s))
        self.assert_(not f3.is_on(s))
        self.assert_(not f4.is_on(s))

        s.add(f3)
        self.assert_(f1.is_on(s))
        self.assert_(f2.is_on(s))
        self.assert_(f3.is_on(s))
        self.assert_(not f4.is_on(s))

        s.add(f4)
        self.assert_(f1.is_on(s))
        self.assert_(f2.is_on(s))
        self.assert_(f3.is_on(s))
        self.assert_(f4.is_on(s))

        self.assert_(f1.is_ok())
        self.assert_(f2.is_ok())
        self.assert_(f3.is_ok())
        self.assert_(f4.is_ok())
        self.assert_(s.is_ok())


class TestSurfaceMethods(unittest.TestCase):

    def setUp(self):

        # Create open and closed surfaces

        self.closed_surface = gts.tetrahedron()
        self.assert_(self.closed_surface.is_ok())

        self.open_surface = gts.Surface()
        for f in list(self.closed_surface)[:-1]:
            self.open_surface.add(f)

        self.assert_(self.open_surface.is_ok())

        # Get the vertices
        self.vertices = []
        self.faces = []
        self.vertex_ids = []

        for f in self.closed_surface:
            self.faces.append(f)
            for v in f.vertices():
                self.assert_(v.is_ok())
                if v.id not in self.vertex_ids:
                    self.vertices.append(v)
                    self.vertex_ids.append(v.id)

    def test_new(self):
        self.assert_(isinstance(self.open_surface,gts.Surface))
        self.assert_(isinstance(self.closed_surface,gts.Surface))

        # Co-linear points
        f = gts.Face([0],[1],[2])
        s = gts.Surface()
        s.add(f)
        self.assert_(s.is_ok())


    def test_subclass(self):

        class Surface(gts.Surface):

            def __init__(self,msg):
                self.msg = msg
                gts.Surface.__init__(self)

            def foo(self):
                return self.msg

        tetrahedron = gts.tetrahedron()
        s = Surface('bar')
        for face in tetrahedron:
            s.add(face)
        del tetrahedron
        self.assert_(s.is_ok)
        self.assert_(s.foo()=='bar')


    def test_add(self):
        f1,f2 = list(self.closed_surface)[:2]
        s1 = gts.Surface()
        s1.add(f1)
        s1.add(f2)
        self.assert_(s1.is_ok())
        self.assert_(s1.Nfaces == 2)

        # Add surface
        f3,f4 = list(self.closed_surface)[2:]
        s2 = gts.Surface()
        s2.add(f3)
        s2.add(f4)
        s1.add(s2)
        self.assert_(s1.is_ok())
        self.assert_(s1.Nfaces == 4)
        for face in self.closed_surface:
            self.assert_(face in s1)


    def test_remove(self):

        f = list(self.closed_surface)[0]
        self.assert_(f in self.closed_surface)
        self.closed_surface.remove(f)
        self.assert_(not f in self.closed_surface)
        self.assert_(self.closed_surface.Nfaces == 3)

        self.assert_(f.is_ok())
        self.assert_(self.closed_surface.is_ok())


    def test_copy(self):

        s = gts.Surface()
        s.copy(self.closed_surface)

        # Turn surface inside out to check that faces are all independent
        for f in s:
            f.revert()
        self.assert_(s.volume()==-self.closed_surface.volume())

        self.assert_(s.is_ok())


    def test_is_manifold(self):
        f2 = list(self.closed_surface)[1]
        self.assert_(self.open_surface.is_manifold())
        self.assert_(self.closed_surface.is_manifold())
        f2.revert()
        self.assert_(self.open_surface.is_manifold())
        self.assert_(self.closed_surface.is_manifold())

        self.assert_(self.open_surface.is_ok())
        self.assert_(self.closed_surface.is_ok())

        # Co-linear points
        f = gts.Face([0],[1],[2])
        s = gts.Surface()
        s.add(f)
        self.assert_(s.is_manifold())


    def test_manifold_faces(self):

        #         v4
        #      e3 e4 e5
        #   v1 e1 v2 e2 v3
        #      e6 e7 e8
        #         v5

        v1 = gts.Vertex(-1,0)
        v2 = gts.Vertex(0,0)
        v3 = gts.Vertex(1,0)
        v4 = gts.Vertex(0,1)
        v5 = gts.Vertex(0,-1)

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v1,v4)
        e4 = gts.Edge(v4,v2)
        e5 = gts.Edge(v4,v3)
        e6 = gts.Edge(v1,v5)
        e7 = gts.Edge(v5,v2)
        e8 = gts.Edge(v5,v3)

        s = gts.Surface()

        f1 = gts.Face(e1,e4,e3)
        f2 = gts.Face(e2,e5,e4)
        f3 = gts.Face(e1,e7,e6)
        f4 = gts.Face(e2,e8,e7)

        for f in [f1,f2,f3,f4]:
            s.add(f)

        self.assert_(s.manifold_faces(e3)==None)
        faces = s.manifold_faces(e1)
        self.assert_(len(faces)==2)
        self.assert_(f1 in faces)
        self.assert_(f3 in faces)


    def test_fan_oriented(self):

        #         v4
        #      e3 e4 e5
        #   v1 e1 v2 e2 v3
        #      e6 e7 e8
        #         v5

        v1 = gts.Vertex(-1,0)
        v2 = gts.Vertex(0,0)
        v3 = gts.Vertex(1,0)
        v4 = gts.Vertex(0,1)
        v5 = gts.Vertex(0,-1)

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v1,v4)
        e4 = gts.Edge(v4,v2)
        e5 = gts.Edge(v4,v3)
        e6 = gts.Edge(v1,v5)
        e7 = gts.Edge(v5,v2)
        e8 = gts.Edge(v5,v3)

        f1 = gts.Face(e1,e4,e3)
        f2 = gts.Face(e2,e5,e4)
        f3 = gts.Face(e1,e7,e6)
        f4 = gts.Face(e2,e8,e7)

        # Unoriented Surface (should fail)
        s = gts.Surface()
        for f in [f1,f2,f3,f4]:
            s.add(f)
        try:
            edges = s.fan_oriented(v2)
        except:
            pass
        else:
            raise RuntimeError, "Did not detect unoriented Surface"


        # Now try with an oriented Surface
        s = gts.Surface()
        for f in [f1,f2,f3,f4]:
            if not f.is_compatible(s):
                f.revert()
            s.add(f)

        edges = s.fan_oriented(v2)

        self.assert_(len(edges)==4)
        self.assert_(e3 in edges)
        self.assert_(e5 in edges)
        self.assert_(e6 in edges)
        self.assert_(e8 in edges)

        edges = s.fan_oriented(v1)
        self.assert_(len(edges)==2)
        self.assert_(e4 in edges)
        self.assert_(e7 in edges)


    def test_is_orientable(self):
        f2 = list(self.closed_surface)[1]
        self.assert_(self.open_surface.is_orientable())
        self.assert_(self.closed_surface.is_orientable())
        f2.revert()
        self.assert_(not self.open_surface.is_orientable())
        self.assert_(not self.closed_surface.is_orientable())

        self.assert_(self.open_surface.is_ok())
        self.assert_(self.closed_surface.is_ok())

        # Co-linear points
        f = gts.Face([0],[1],[2])
        s = gts.Surface()
        s.add(f)
        self.assert_(s.is_orientable())


    def test_is_closed(self):
        self.assert_(not self.open_surface.is_closed())
        self.assert_(self.closed_surface.is_closed())


    def test_area(self):
        self.assert_(fabs(self.closed_surface.area()-8*sqrt(3))<1.e-9)
        self.assert_(fabs(self.open_surface.area()-8*sqrt(3)*0.75)<1.e-9)


        # Co-linear points
        f = gts.Face([0],[1],[2])
        s = gts.Surface()
        s.add(f)
        self.assert_(s.area()==0.)


    def test_faces(self):

        #         v4
        #      e3 e4 e5
        #   v1 e1 v2 e2 v3
        #      e6 e7 e8
        #         v5

        v1 = gts.Vertex(-1,0)
        v2 = gts.Vertex(0,0)
        v3 = gts.Vertex(1,0)
        v4 = gts.Vertex(0,1)
        v5 = gts.Vertex(0,-1)

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v1,v4)
        e4 = gts.Edge(v4,v2)
        e5 = gts.Edge(v4,v3)
        e6 = gts.Edge(v1,v5)
        e7 = gts.Edge(v5,v2)
        e8 = gts.Edge(v5,v3)

        s = gts.Surface()

        f1 = gts.Face(e1,e4,e3)
        f2 = gts.Face(e2,e5,e4)
        f3 = gts.Face(e1,e7,e6)
        f4 = gts.Face(e2,e8,e7)

        for f in [f1,f2,f3,f4]:
            s.add(f)

        edges = s.boundary()
        
        self.assert_(len(edges)==4)
        self.assert_(e3 in edges)
        self.assert_(e5 in edges)
        self.assert_(e6 in edges)
        self.assert_(e8 in edges)


    def test_volume(self):
        self.assert_(self.closed_surface.volume()==8./3.)
        try:
            self.open_surface.volume()
        except:
            pass
        else:
            raise RuntimeError, "Did not detect open surface"


    def test_center_of_mass(self):
        self.closed_surface.translate(1,2,3)
        self.assert_(self.closed_surface.center_of_mass()==(1,2,3))


    def test_center_of_area(self):
        self.closed_surface.translate(1,2,3)
        c = self.closed_surface.center_of_area()
        self.assert_(fabs(1-c[0])<1.e-9)
        self.assert_(fabs(2-c[1])<1.e-9)
        self.assert_(fabs(3-c[2])<1.e-9)


    def test_iter(self):
        n = 0
        for face in self.closed_surface:
            self.assert_(isinstance(face,gts.Face))
            self.assert_(face.is_ok())
            self.assert_(face in self.faces)
            self.faces.remove(face)
            n += 1
        self.assert_(len(self.faces)==0)
        self.assert_(self.closed_surface.is_ok())
        self.assert_(n==self.closed_surface.Nfaces)


    def test_iter2(self):

        # *** ATTENTION ***
        # This strange test revealed problems that occur when the gts 
        # traverse is not handled carefully in the iter(), iternext() 
        # and inter() methods.  Leave it in and don't change it.  This
        # appears to have exposed a bug in GTS.  Look for the commentary
        # in the inter() function.
        #
        # There may be other places where a dangling iter() could cause
        # problems.  This was just found by chance, and other
        # operations should be checked.

        s1 = gts.tetrahedron()
        self.assert_(s1.is_ok())
        
        s2 = gts.tetrahedron()
        s2.translate(0.5,0.5,0.5)
        self.assert_(s2.is_ok())

        for f in s1:
            pass
        iter(s1)

        self.assert_(s1.is_ok())
        self.assert_(s2.is_ok())

        s3 = s2.difference(s1)

        self.assert_(s1.is_ok())
        self.assert_(s2.is_ok())
        self.assert_(s3.is_ok())


    def test_readwrite(self):

        path = os.path.join(tempfile.gettempdir(),'pygts_test.dat')

        s1 = self.closed_surface
        f = open(path,'w')
        s1.write(f)
        f.close()

        f = open(path,'r')
        s2 = gts.read(f)
        f.close()

        vertices1 = [f.vertices() for f in s1]
        vertices2 = [f.vertices() for f in s2]
        self.assert_(len(vertices1)==len(vertices2))
        self.assert_( all([vertex in vertices2 for vertex in vertices1]) )

        self.assert_(s1.is_ok())
        self.assert_(s2.is_ok())


    def test_distance(self):

        # Two spheres
        s1 = gts.sphere(4)
        s2 = gts.sphere(4)
        s2.scale(2,2,2)
        s2.translate(10)

        fr = s1.distance(s2)
        self.assert_(fr['min']==7.0)
        self.assert_(fr['max']==9.0)

        fr = s2.distance(s1)
        self.assert_(fr['min']==7.0)
        self.assert_(fr['max']==11.0)


    def test_distance_2(self):

        # Surfaces with boundaries

        def getsurf():
            #         v4
            #      e3 e4 e5
            #   v1 e1 v2 e2 v3
            #      e6 e7 e8
            #         v5

            v1 = gts.Vertex(-1,0)
            v2 = gts.Vertex(0,0)
            v3 = gts.Vertex(1,0)
            v4 = gts.Vertex(0,1)
            v5 = gts.Vertex(0,-1)

            e1 = gts.Edge(v1,v2)
            e2 = gts.Edge(v2,v3)
            e3 = gts.Edge(v1,v4)
            e4 = gts.Edge(v4,v2)
            e5 = gts.Edge(v4,v3)
            e6 = gts.Edge(v1,v5)
            e7 = gts.Edge(v5,v2)
            e8 = gts.Edge(v5,v3)

            s = gts.Surface()

            f1 = gts.Face(e1,e4,e3)
            f2 = gts.Face(e2,e5,e4)
            f3 = gts.Face(e1,e7,e6)
            f4 = gts.Face(e2,e8,e7)

            for f in [f1,f2,f3,f4]:
                s.add(f)

            return s

        s1 = getsurf()
        s2 = getsurf()
        s2.scale(2)
        s2.translate(10)

        r = s1.distance(s2)
        self.assert_(len(r)==2)
        fr,br = r
        self.assert_(fr['min']==7.0)
        self.assert_(fr['max']==9.0)
        self.assert_(br['min']==7.0)
        self.assert_(br['max']==9.0)

        r = s2.distance(s1)
        self.assert_(len(r)==2)
        fr,br = r
        self.assert_(fr['min']==7.0)
        self.assert_(fr['max']==11.0)
        self.assert_(br['min']==7.0)
        self.assert_(br['max']==11.0)


    def test_split(self):

        # Create two surfaces
        f1 = gts.Face(gts.Vertex(0,0),
                      gts.Vertex(1,0),
                      gts.Vertex(0,1))
        f2 = gts.Face(gts.Vertex(0,0,-1),
                      gts.Vertex(1,0,-1),
                      gts.Vertex(0,1,-1))
        s1 = gts.Surface()
        s1.add(f1)
        s1.add(f2)

        self.assert_(s1.is_ok())

        # *** ATTENTION ***
        # Write and read surfaces to/from GTS files.  This should not be 
        # necessary, but is required for the test to succeed.  Is it a GTS
        # bug?
        path = os.path.join(tempfile.gettempdir(),'pygts_test.dat')        
        f = open(path,'w')
        s1.write(f)
        f.close()
        s1 = gts.Surface()
        f = open(path,'r')
        s1 = gts.read(f)
        f.close()
        # *** ATTENTION ***

        self.assert_(s1.is_ok())

        surfaces = s1.split()
        self.assert_(s1.is_ok())
        self.assert_(len(surfaces)==2)

        self.assert_((f1 in surfaces[0] and f2 in surfaces[1]) or \
                         (f1 in surfaces[1] and f2 in surfaces[0]))
                     

    def test_Nfaces(self):
        self.assert_(self.closed_surface.Nfaces==4)
        self.assert_(self.open_surface.Nfaces==3)


    def test_Nedges(self):
        self.assert_(self.closed_surface.Nedges==6)
        self.assert_(self.open_surface.Nedges==6)


    def test_Nvertices(self):
        self.assert_(self.closed_surface.Nvertices==4)
        self.assert_(self.open_surface.Nvertices==4)


    def test_vertices(self):
        self.assert_(len(self.closed_surface.vertices())==len(self.vertices))
        self.assert_(len(self.open_surface.vertices())==len(self.vertices))
        for v in self.vertices:
            self.assert_(v.is_ok())
            self.assert_(v.id in self.vertex_ids)
        self.assert_(self.closed_surface.is_ok())
        self.assert_(self.open_surface.is_ok())


    def test_strip(self):


        #         v4 e9  v6
        #      e3 e4 e5  e10
        #   v1 e1 v2 e2  v3      <----fold
        #      e6 e7 e8  e12
        #         v5 e11 v7

        v1 = gts.Vertex(-1,0)
        v2 = gts.Vertex(0,0)
        v3 = gts.Vertex(1,0)
        v4 = gts.Vertex(0,0,1)
        v5 = gts.Vertex(0,-1)
        v6 = gts.Vertex(1,0,1)
        v7 = gts.Vertex(1,-1)

        vertices = [v1,v2,v3,v4,v5,v6,v7]

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v1,v4)
        e4 = gts.Edge(v4,v2)
        e5 = gts.Edge(v4,v3)
        e6 = gts.Edge(v1,v5)
        e7 = gts.Edge(v5,v2)
        e8 = gts.Edge(v5,v3)

        # *** ATTENTION ***
        # The existence of these extra edges affects the output of strip(),
        # regardless of whether or not the edges are actually used in a 
        # face (they are commented out below).  When these extra edges are
        # not used, there are 2 strips containing 2 faces each.  When they
        # are used, the strips have 3 and 1 faces instead.
        e9 = gts.Edge(v4,v6)
        e10 = gts.Edge(v6,v3)
        e11 = gts.Edge(v5,v7)
        e12 = gts.Edge(v7,v3)
        # *** ATTENTION ***

        edges = [e1,e2,e3,e4,e5,e6,e7,e8,e9,e10,e11,e12]

        # *** ATTENTION ***
        # If I don't delete the edges then GTS bails with the following 
        # internal error:
        #
        #   **
        #   Gts:ERROR:stripe.c:500:map_lookup: assertion failed: (td)
        #   Abort trap
        #
        # This is probably due to the fact that each Edge has a parent
        # triangle in the python object.  GTS should handle that,
        # and so this is a GTS bug.
        del edges
        # *** ATTENTION ***

        faces = [gts.Face(e1,e4,e3),gts.Face(e2,e5,e4),gts.Face(e1,e7,e6),
                 gts.Face(e2,e8,e7)]#,gts.Face(e9,e10,e5), gts.Face(e11,e12,e8)]

        s = gts.Surface()
        for f in faces:
            if not f.is_compatible(s):
                f.revert()
            s.add(f)

        del e1,e2,e3,e4,e5,e6,e7,e8,e9,e10,e11,e12

        strip_faces = []
        for strip in s.strip():
            strip_faces += list(strip)

        for face in faces:
            self.assert_(face in strip_faces)
            strip_faces.remove(face)

        self.assert_(len(s.strip())==2)

        for vertex in vertices:
            self.assert_(vertex.is_ok())

        for face in faces:
            self.assert_(face.is_ok())

        self.assert_(s.is_ok())


    def test_stats(self):

        stats = self.closed_surface.stats()

        self.assert_(stats['n_faces'] == 4)
        self.assert_(stats['n_incompatible_faces'] == 0)
        self.assert_(stats['n_boundary_edges'] == 0)
        self.assert_(stats['n_non_manifold_edges'] == 0)
        self.assert_(stats['edges_per_vertex']['min'] == 3.)
        self.assert_(stats['edges_per_vertex']['max'] == 3.)
        self.assert_(stats['edges_per_vertex']['mean'] == 3.)
        self.assert_(stats['edges_per_vertex']['stddev'] == 0.)
        self.assert_(stats['faces_per_edge']['min'] == 2.)
        self.assert_(stats['faces_per_edge']['max'] == 2.)
        self.assert_(stats['faces_per_edge']['mean'] == 2.)
        self.assert_(stats['faces_per_edge']['stddev'] == 0.)

        self.assert_(self.closed_surface.is_ok())


    def test_quality_stats(self):

        stats = self.closed_surface.quality_stats()

        self.assert_(fabs(stats['face_quality']['min']-1.)<1.e-9)
        self.assert_(fabs(stats['face_quality']['max']-1.)<1.e-9)
        self.assert_(fabs(stats['face_quality']['mean']-1.)<1.e-9)
        self.assert_(stats['face_quality']['stddev'] == 0.)

        self.assert_(fabs(stats['face_area']['min']-2*sqrt(3))<1.e-9)
        self.assert_(fabs(stats['face_area']['max']-2*sqrt(3))<1.e-9)
        self.assert_(fabs(stats['face_area']['mean']-2*sqrt(3))<1.e-9)
        self.assert_(stats['face_area']['stddev'] == 0.)


        self.assert_(fabs(stats['edge_length']['min']-2*sqrt(2))<1.e-9)
        self.assert_(fabs(stats['edge_length']['max']-2*sqrt(2))<1.e-9)
        self.assert_(fabs(stats['edge_length']['mean']-2*sqrt(2))<1.e-9)
        self.assert_(stats['edge_length']['stddev'] == 0.)

        self.assert_(fabs(stats['edge_angle']['min']-atan(2*sqrt(2)))<1.e-9)
        self.assert_(fabs(stats['edge_angle']['max']-atan(2*sqrt(2)))<1.e-9)
        self.assert_(fabs(stats['edge_angle']['mean']-atan(2*sqrt(2)))<1.e-9)
        self.assert_(stats['edge_angle']['stddev'] == 0.)

        self.assert_(self.closed_surface.is_ok())


    def test_tessellate(self):

        self.closed_surface.tessellate()
        self.assert_(self.closed_surface.Nfaces==16)


    def test_indices(self):

        vs = self.closed_surface.vertices()
        faces = [face for face in self.closed_surface]

        indices = self.closed_surface.face_indices(vs)
        self.assert_(len(indices)==self.closed_surface.Nfaces)

        # Check the indexed vertices against the retrieved faces
        for i in indices:
            self.assert_(len(i)==3)
            t = gts.Triangle(vs[i[0]],vs[i[1]],vs[i[2]])

            flag1 = flag2 = False
            for face in faces:
                if t == face:
                    if flag1==True:
                        flag2 = True
                    flag1 = True

            self.assert_(flag1 and not flag2)

        self.assert_(self.closed_surface.is_ok())


    def test_inter(self):

        s1 = gts.tetrahedron()

        s2 = gts.tetrahedron()
        self.assert_(s2.is_ok())

        s2.translate(0.5,0.5,0.5)
        self.assert_(s2.is_ok())

        self.assert_(s1.is_closed())
        self.assert_(s2.is_closed())

        # Test the intersection
        s3 = s1.intersection(s2)
        self.assert_(s3.is_ok())        
        self.assert_(isinstance(s3,gts.Surface))
        self.assert_(s3.is_closed())
        self.assert_(s3.Nfaces==4)
        self.assert_(s3.volume()<s2.volume())

        # Test the union
        s3 = s1.union(s2)
        self.assert_(s3.is_ok())
        self.assert_(isinstance(s3,gts.Surface))
        self.assert_(s3.is_closed())
        self.assert_(s3.Nfaces==16)
        self.assert_(s3.volume()>s2.volume())

        # Test the difference 1
        s3 = s1.difference(s2)
        self.assert_(s3.is_ok())
        self.assert_(isinstance(s3,gts.Surface))
        self.assert_(s3.is_closed())
        self.assert_(s3.Nfaces==8)
        # Make an orientable copy
        s = gts.Surface()
        for f in gts.Surface().copy(s3):
            if not f.is_compatible(s):
                f.revert()
            s.add(f)
        self.assert_(s.is_ok())
        self.assert_(s.volume()<s2.volume())

        # Test the difference 2
        s3 = s2.difference(s1)
        self.assert_(s3.is_ok())
        self.assert_(isinstance(s3,gts.Surface))
        self.assert_(s3.is_closed())
        self.assert_(s3.Nfaces==12)
        # Make an orientable copy
        s = gts.Surface()
        for f in gts.Surface().copy(s3):
            if not f.is_compatible(s):
                f.revert()
            s.add(f)
        self.assert_(s.is_ok())
        self.assert_(s.volume()<s2.volume())

        # Test mutual intersection
        s1 = gts.cube()
        s2 = gts.cube()
        try:
            s3 = s2.difference(s1)
        except RuntimeError:
            pass
        else:
            raise RuntimeError, 'Did not catch mutual intersection'
        self.assert_(s1.is_ok())
        self.assert_(s2.is_ok())


#    def test_inter2(self):
#
#        EPS = 2**(-51)
#        SCALE = 1.+EPS
#
#        s1 = gts.cube()
#        s2 = gts.cube()
#        s2.scale(1.,SCALE,SCALE)
#        s2.translate(0.5)
#
#        s3 = s1.union(s2)
#        self.assert_(s3.is_ok())
#        self.assert_(fabs(s3.volume()-10.)<1.e-9)
#
#        s3 = s1.intersection(s2)
#        self.assert_(s3.is_ok())
#        self.assert_(fabs(s3.volume()-2.)<1.e-9)
#
#        s3 = s1.difference(s2)
#        s4 = gts.Surface()
#        for f in s3:
#            if not f.is_compatible(s4):
#                f.revert()
#            s4.add(f)
#        self.assert_(s3.is_ok())
#        self.assert_(fabs(s3.volume()-6.)<1.e-9)


    def test_rotate(self):

        f = gts.Face(gts.Vertex(0,0),
                      gts.Vertex(1,0),
                      gts.Vertex(0,2))
        s = gts.Surface()
        s.add(f)
        s.rotate(dz=1,a=radians(90))

        self.assert_(s.is_ok())

        v1,v2,v3 = iter(s).next().vertices()

        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v3.is_ok())

        self.assert_(v1 == gts.Vertex())
        self.assert_(fabs(v2.x)<1.e-9)
        self.assert_(v2.y==1)
        self.assert_(v3.x==-2)
        self.assert_(fabs(v3.y)<1.e-9)

        self.assert_(s.is_ok())


    def test_scale(self):

        f = gts.Face(gts.Vertex(0,0),
                      gts.Vertex(1,0),
                      gts.Vertex(0,2))
        s = gts.Surface()
        s.add(f)
        s.scale(2,3)

        self.assert_(s.is_ok())

        v1,v2,v3 = iter(s).next().vertices()

        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v3.is_ok())

        self.assert_(v1 == gts.Vertex())
        self.assert_(v2 == gts.Vertex(2,0))
        self.assert_(v3 == gts.Vertex(0,6))


    def test_translate(self):

        f = gts.Face(gts.Vertex(0,0),
                      gts.Vertex(1,0),
                      gts.Vertex(0,2))
        s = gts.Surface()
        s.add(f)
        s.translate(1,2,3)

        self.assert_(s.is_ok())

        v1,v2,v3 = iter(s).next().vertices()

        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v3.is_ok())

        self.assert_(v1 == gts.Vertex(1,2,3))
        self.assert_(v2 == gts.Vertex(2,2,3))
        self.assert_(v3 == gts.Vertex(1,4,3))


    def test_is_self_intersecting(self):
        
        v1 = gts.Vertex(-1,0)
        v2 = gts.Vertex(0,0)
        v3 = gts.Vertex(0,1)
        v4 = gts.Vertex(1,0)

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v3,v1)
        e4 = gts.Edge(v2,v4)
        e5 = gts.Edge(v4,v3)

        f1 = gts.Face(e1,e2,e3)
        f2 = gts.Face(e2,e4,e5)

        s = gts.Surface()
        s.add(f1)
        s.add(f2)

        self.assert_(not s.is_self_intersecting())

        v5 = gts.Vertex(0,0,1)
        v6 = gts.Vertex(0.5,0,-1)
        v7 = gts.Vertex(0.5,0.5,0)
        e6 = gts.Edge(v5,v6)
        e7 = gts.Edge(v6,v7)
        e8 = gts.Edge(v7,v5)
        f3 = gts.Face(e6,e7,e8)
        s.add(f3)

        self.assert_(s.is_self_intersecting())

        self.assert_(s.is_ok())


    def test_cleanup(self):

        v1 = gts.Vertex(-1,0)
        v2a = gts.Vertex(0,0)
        v2b = gts.Vertex(1.e-10,0)
        v3 = gts.Vertex(0,1)
        v4 = gts.Vertex(1,0)

        f1 = gts.Face(v1,v2a,v3)
        f2 = gts.Face(v2b,v3,v4)

        s = gts.Surface()
        s.add(f1)
        s.add(f2)

        self.assert_(s.is_ok())
        self.assert_(not f1.common_edge(f2))

        s.cleanup(1.e-6)

        self.assert_(v1.is_ok())
        self.assert_(v2a.is_ok())
        self.assert_(v2b.is_ok())
        self.assert_(v3.is_ok())
        self.assert_(v4.is_ok())

        self.assert_(f1.is_ok())
        self.assert_(f2.is_ok())

        self.assert_(f1.e1.is_ok())
        self.assert_(f1.e2.is_ok())
        self.assert_(f1.e3.is_ok())

        self.assert_(f2.e1.is_ok())
        self.assert_(f2.e2.is_ok())
        self.assert_(f2.e3.is_ok())

        self.assert_(s.is_ok())

        self.assert_(f1.common_edge(f2))


    def test_parent(self):

        #         v4
        #      e3 e4 e5
        #   v1 e1 v2 e2 v3
        #      e6 e7 e8
        #         v5

        v1 = gts.Vertex(-1,0)
        v2 = gts.Vertex(0,0)
        v3 = gts.Vertex(1,0)
        v4 = gts.Vertex(0,1)
        v5 = gts.Vertex(0,-1)

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v1,v4)
        e4 = gts.Edge(v4,v2)
        e5 = gts.Edge(v4,v3)
        e6 = gts.Edge(v1,v5)
        e7 = gts.Edge(v5,v2)
        e8 = gts.Edge(v5,v3)

        s = gts.Surface()

        f1 = gts.Face(e1,e4,e3)
        f2 = gts.Face(e2,e5,e4)
        f3 = gts.Face(e1,e7,e6)
        f4 = gts.Face(e2,e8,e7)

        for f in [f1,f2,f3,f4]:
            s.add(f)

        self.assert_(s.parent(e3)==f1)
        self.assert_(s.parent(e5)==f2)
        self.assert_(s.parent(e6)==f3)
        self.assert_(s.parent(e8)==f4)
        self.assert_(s.parent(e1)==f1 or s.parent(e1)==f3)
        self.assert_(s.parent(e2)==f2 or s.parent(e2)==f4)


    def test_edges(self):

        #         v4
        #      e3 e4 e5
        #   v1 e1 v2 e2 v3
        #      e6 e7 e8
        #         v5

        v1 = gts.Vertex(-1,0)
        v2 = gts.Vertex(0,0)
        v3 = gts.Vertex(1,0)
        v4 = gts.Vertex(0,1)
        v5 = gts.Vertex(0,-1)

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v1,v4)
        e4 = gts.Edge(v4,v2)
        e5 = gts.Edge(v4,v3)
        e6 = gts.Edge(v1,v5)
        e7 = gts.Edge(v5,v2)
        e8 = gts.Edge(v5,v3)

        s = gts.Surface()

        f1 = gts.Face(e1,e4,e3)
        s.add(f1)

        f2 = gts.Face(e2,e5,e4)
        s.add(f2)

        f3 = gts.Face(e1,e7,e6)
        s.add(f3)

        f4 = gts.Face(e2,e8,e7)
        s.add(f4)

        vertices = [v1,v5]
        del v1, e1

        edges = s.edges(vertices)

        self.assert_(len(edges)==5)
        self.assert_(e3 in edges)
        self.assert_(e6 in edges)
        self.assert_(e7 in edges)
        self.assert_(e8 in edges)

        edges = s.edges()
        self.assert_(len(edges)==8)

        self.assert_(e2.is_ok())
        self.assert_(e3.is_ok())
        self.assert_(e4.is_ok())
        self.assert_(e5.is_ok())
        self.assert_(e6.is_ok())
        self.assert_(e7.is_ok())
        self.assert_(e8.is_ok())

        self.assert_(f1.is_ok())
        self.assert_(f2.is_ok())
        self.assert_(f3.is_ok())
        self.assert_(f4.is_ok())

        self.assert_(s.is_ok())


    def test_faces(self):

        #         v4
        #      e3 e4 e5
        #   v1 e1 v2 e2 v3
        #      e6 e7 e8
        #         v5

        v1 = gts.Vertex(-1,0)
        v2 = gts.Vertex(0,0)
        v3 = gts.Vertex(1,0)
        v4 = gts.Vertex(0,1)
        v5 = gts.Vertex(0,-1)

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v1,v4)
        e4 = gts.Edge(v4,v2)
        e5 = gts.Edge(v4,v3)
        e6 = gts.Edge(v1,v5)
        e7 = gts.Edge(v5,v2)
        e8 = gts.Edge(v5,v3)

        s = gts.Surface()

        f1 = gts.Face(e1,e4,e3)
        f2 = gts.Face(e2,e5,e4)
        f3 = gts.Face(e1,e7,e6)
        f4 = gts.Face(e2,e8,e7)

        for f in [f1,f2,f3,f4]:
            s.add(f)

        edges = [e3,e6]
        del v1,v2,v3,v4,v5
        del f2,f4
        del e1,e2,e3,e4,e5,e6,e7,e8

        faces = s.faces(edges)

        self.assert_(len(faces)==2)
        self.assert_(f1 in faces)
        self.assert_(f3 in faces)

        faces = s.faces()
        self.assert_(len(faces)==4)

        self.assert_(f1.is_ok())
        self.assert_(f3.is_ok())

        self.assert_(s.is_ok())


class TestFunctions(unittest.TestCase):

    def test_merge(self):

        vertices = [gts.Vertex(0),gts.Vertex(0.1),gts.Vertex(10),
                    gts.Vertex(100)]

        v = gts.merge(vertices,0.2)
        self.assert_(len(v)==3)
        self.assert_(vertices[1] in v)
        self.assert_(vertices[2] in v)
        self.assert_(vertices[3] in v)

        for v in vertices:
            self.assert_(v.is_ok())

        v = gts.merge(vertices,100)
        self.assert_(len(v)==1)

        for v in vertices:
            self.assert_(v.is_ok())

        s = gts.Surface()
        s.add(gts.Face(gts.Vertex(-1,0),gts.Vertex(0,0),gts.Vertex(0,1)))
        s.add(gts.Face(gts.Vertex(1,0),gts.Vertex(0,0),gts.Vertex(0,1)))

        self.assert_(len(s.vertices())==6)

        gts.merge(s.vertices(),1.e-6)

        self.assert_(len(s.vertices())==4)
        
        self.assert_(s.is_ok())


    def test_vertices(self):

        v1,v2,v3 = gts.Vertex(0),gts.Vertex(1),gts.Vertex(2)
        segments = [ gts.Segment(v1,v2), 
                     gts.Segment(v2,v3), 
                     gts.Edge(v1,v3) ]

        vertices = gts.vertices(segments)

        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v3.is_ok())
        for segment in segments:
            self.assert_(segment.is_ok())

        self.assert_(len(vertices)==3)
        self.assert_(v1 in vertices)
        self.assert_(v2 in vertices)
        self.assert_(v3 in vertices)


    def test_segments(self):

        v1,v2,v3 = gts.Vertex(0),gts.Vertex(1),gts.Vertex(2)
        s1,s2 = gts.Segment(v1,v2), gts.Edge(v2,v3)

        segments = gts.segments([v1,v2,v3])

        self.assert_(len(segments)==2)

        self.assert_(v1.is_ok())
        self.assert_(v2.is_ok())
        self.assert_(v3.is_ok())

        self.assert_(s1.is_ok())
        self.assert_(s2.is_ok())


    def test_triangles(self):

        #         v4
        #      e3 e4 e5
        #   v1 e1 v2 e2 v3
        #      e6 e7 e8
        #         v5

        v1 = gts.Vertex(-1,0)
        v2 = gts.Vertex(0,0)
        v3 = gts.Vertex(1,0)
        v4 = gts.Vertex(0,1)
        v5 = gts.Vertex(0,-1)

        e1 = gts.Edge(v1,v2)
        e2 = gts.Edge(v2,v3)
        e3 = gts.Edge(v1,v4)
        e4 = gts.Edge(v4,v2)
        e5 = gts.Edge(v4,v3)
        e6 = gts.Edge(v1,v5)
        e7 = gts.Edge(v5,v2)
        e8 = gts.Edge(v5,v3)

        f1 = gts.Face(e1,e4,e3)
        f2 = gts.Face(e2,e5,e4)
        f3 = gts.Triangle(e1,e7,e6)
        f4 = gts.Triangle(e2,e8,e7)

        triangles = gts.triangles([e1,e4])
        self.assert_(len(triangles)==3)
        self.assert_(f1 in triangles)
        self.assert_(f2 in triangles)
        self.assert_(f3 in triangles)

        
    def test_triangle_enclosing(self):

        points=[gts.Point(-1,-1),gts.Point(-1,1),
                gts.Vertex(1,-1),gts.Vertex(1,1)]
        triangle = gts.triangle_enclosing(points)

        for point in points:
            self.assert_(point.is_in(triangle))

    def test_isosurface(self):

        if HAS_NUMPY:

            # Check parameters of a sphere generated from an isosurface
            N = 50                          # Size of data cube

            # Radius of sphere. If is too large (4.0 is too large) this will
            # tickle a GTS bug with the dual method, which produces an extra
            # surface in the z=-5 plane; see examples/iso.py.
            r = 3.0

            tol = 1e-2                      # Needs changing with N
            Nj = N*(0+1j)
            x, y, z = numpy.ogrid[-5:5:Nj, -5:5:Nj, -5:5:Nj]

            scalars = x*x + y*y + z*z
            extents= numpy.asarray([-5.0, 5.0, -5.0, 5.0, -5.0, 5.0])

            # Doesn't work for method='bounded' because the sphere is completely
            # contained within the bounding cube: for a visualisation see
            # examples/iso.py --function=ellipsoid -c20 --method=bounded
            for method in ['cubes',  'tetra', 'dual' ]:
                # print '\n--', method
                S = gts.isosurface(scalars,r**2,extents=extents,method=method)  
                self.assert_(S.is_manifold())
                self.assert_(S.is_closed())
                A = S.area()
                V = S.volume()
                # print 'Area', S.area() / (4*numpy.pi*r**2)
                # print 'Volume', S.volume() / (4*numpy.pi*r**3/3)
                self.assert_(fabs(A/(4*numpy.pi*r**2) - 1) < tol)
                self.assert_(fabs(V/(4*numpy.pi*r**3/3) - 1) < tol)
                xyz = numpy.asarray([v.coords() for v in S.vertices()])
                dd = (xyz*xyz).sum(1)
                self.assert_(fabs(dd.max()/(r*r) - 1) < tol)
                self.assert_(fabs(dd.min()/(r*r) - 1) < tol)


            """
            def iso_test_method(method='c'):
                s = gts.isosurface(scalars,r**2,extents=extents,method=method)
                self.assert_(s.is_ok())
                self.assert_(s.is_manifold())
                self.assert_(s.is_closed())
#                print '\n\n*** Area', s.area() / (4*numpy.pi*r**2)
#                print '\n\n*** Volume', s.volume() / (4*numpy.pi*r**3/3)
                self.assert_(fabs(s.area() / (4*numpy.pi*r**2) - 1) < tol)
                self.assert_(fabs(s.volume() / (4*numpy.pi*r**3/3) - 1) < tol)
                xyz = numpy.asarray([v.coords() for v in s.vertices()])
                dd = (xyz*xyz).sum(1)
                self.assert_(fabs(dd.max()/(r*r) - 1) < tol)
                self.assert_(fabs(dd.min()/(r*r) - 1) < tol)

            iso_test_method('c')
            iso_test_method('t')
#            iso_test_method('d')  # Fails
#            iso_test_method('b')  # Fails
"""
        else:
            sys.stderr.write('*** skipping *** ...')


tests = [TestPointMethods,
         TestVertexMethods,
         TestSegmentMethods,
         TestEdgeMethods,
         TestTriangleMethods,
         TestFaceMethods,
         TestSurfaceMethods,
         TestFunctions
         ]


if __name__ == '__main__':

    suite = unittest.TestSuite()
    for test in tests:
        suite.addTest(unittest.makeSuite(test))

    result = unittest.TextTestRunner(verbosity=2).run(suite)

    Nerrors = len(result.errors)
    Nfailures = len(result.failures)

    sys.exit(Nfailures)
