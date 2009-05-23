/* pygts - python module for the manipulation of triangulated surfaces
 *
 *   Copyright (C) 2009 Thomas J. Duck
 *   All rights reserved.
 *
 *   Thomas J. Duck <tom.duck@dal.ca>
 *   Department of Physics and Atmospheric Science,
 *   Dalhousie University, Halifax, Nova Scotia, Canada, B3H 3J5
 *
 * NOTICE
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this library; if not, write to the
 *   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *   Boston, MA 02111-1307, USA.
 */

#include "pygts.h"


/*-------------------------------------------------------------------------*/
/* Methods exported to python */

static PyObject*
is_ok(PygtsTriangle *self, PyObject *args)
{
  if(pygts_triangle_is_ok(self)) {
    Py_INCREF(Py_True);
    return Py_True;
  }
  else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}


static PyObject*
area(PygtsTriangle *self, PyObject *args)
{
#if PYGTS_DEBUG
  if(!pygts_triangle_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  return Py_BuildValue("d",
      gts_triangle_area(GTS_TRIANGLE(PYGTS_OBJECT(self)->gtsobj)));
}


static PyObject*
perimeter(PygtsTriangle *self, PyObject *args)
{
#if PYGTS_DEBUG
  if(!pygts_triangle_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  return Py_BuildValue("d",
      gts_triangle_perimeter(GTS_TRIANGLE(PYGTS_OBJECT(self)->gtsobj)));
}


static PyObject*
quality(PygtsTriangle *self, PyObject *args)
{
#if PYGTS_DEBUG
  if(!pygts_triangle_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  return Py_BuildValue("d",
      gts_triangle_quality(GTS_TRIANGLE(PYGTS_OBJECT(self)->gtsobj)));
}


static PyObject*
normal(PygtsTriangle *self, PyObject *args)
{
  gdouble x,y,z;

#if PYGTS_DEBUG
  if(!pygts_triangle_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  gts_triangle_normal(GTS_TRIANGLE(PYGTS_OBJECT(self)->gtsobj),&x,&y,&z);
  return Py_BuildValue("ddd",x,y,z);
}


static PyObject*
revert(PygtsTriangle *self, PyObject *args)
{
#if PYGTS_DEBUG
  if(!pygts_triangle_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  gts_triangle_revert(GTS_TRIANGLE(PYGTS_OBJECT(self)->gtsobj));
  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject*
orientation(PygtsTriangle *self, PyObject *args)
{
#if PYGTS_DEBUG
  if(!pygts_triangle_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  return Py_BuildValue("d",
      gts_triangle_orientation(GTS_TRIANGLE(PYGTS_OBJECT(self)->gtsobj)));
}


static PyObject*
angle(PygtsTriangle* self, PyObject *args)
{
  PyObject *t_;
  PygtsTriangle *t;

#if PYGTS_DEBUG
  if(!pygts_triangle_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */
  if(! PyArg_ParseTuple(args, "O", &t_) ) {
    return NULL;
  }

  /* Convert to PygtsObjects */
  if(!pygts_triangle_check(t_)) {
    PyErr_SetString(PyExc_TypeError,"t is not a Triangle");
    return NULL;
  }
  t = PYGTS_TRIANGLE(t_);

  return Py_BuildValue("d",
      gts_triangles_angle(GTS_TRIANGLE(PYGTS_OBJECT(self)->gtsobj),
			  GTS_TRIANGLE(PYGTS_OBJECT(t)->gtsobj)));
}


static PyObject*
is_compatible(PygtsTriangle *self, PyObject *args)
{
  PyObject *t2_;
  PygtsTriangle *t2;
  GtsEdge *e;

#if PYGTS_DEBUG
  if(!pygts_triangle_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &t2_) ) {
    return NULL;
  }

  /* Convert to PygtsObjects */
  if(!pygts_triangle_check(t2_)) {
    PyErr_SetString(PyExc_TypeError,"expected a Triangle");
    return NULL;
  }
  t2 = PYGTS_TRIANGLE(t2_);

  /* Get the common edge */
  if( (e = gts_triangles_common_edge(GTS_TRIANGLE(PYGTS_OBJECT(self)->gtsobj),
				     GTS_TRIANGLE(PYGTS_OBJECT(t2)->gtsobj))) 
      == NULL ) {
    PyErr_SetString(PyExc_TypeError,"Triangles do not share common edge");
    return NULL;
  }

  if(gts_triangles_are_compatible(GTS_TRIANGLE(PYGTS_OBJECT(self)->gtsobj),
				  GTS_TRIANGLE(PYGTS_OBJECT(t2)->gtsobj),e)
				  ==TRUE) {
    Py_INCREF(Py_True);
    return Py_True;
  }
  else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}


static PyObject*
common_edge(PygtsTriangle *self, PyObject *args)
{
  PyObject *t2_;
  PygtsTriangle *t2;
  GtsEdge *e;
  GtsTriangle *parent;
  PygtsEdge *edge;

#if PYGTS_DEBUG
  if(!pygts_triangle_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &t2_) ) {
    return NULL;
  }

  /* Convert to PygtsObjects */
  if(!pygts_triangle_check(t2_)) {
    PyErr_SetString(PyExc_TypeError,"expected a Triangle");
    return NULL;
  }
  t2 = PYGTS_TRIANGLE(t2_);

  /* Get the common edge */
  if( (e = gts_triangles_common_edge(GTS_TRIANGLE(PYGTS_OBJECT(self)->gtsobj),
				     GTS_TRIANGLE(PYGTS_OBJECT(t2)->gtsobj))) 
      == NULL ) {
    Py_INCREF(Py_None);
    return Py_None;
  }

  /* If corresponding PyObject found in object table, we are done */
  if( (edge = PYGTS_EDGE(g_hash_table_lookup(obj_table,e))) != NULL ) {
    Py_INCREF(edge);
    return (PyObject*)edge;
  }

  /* Chain up object allocation */
  args = Py_BuildValue("OOi",Py_None,Py_None,FALSE);
  edge = PYGTS_EDGE(PygtsEdgeType.tp_new(&PygtsEdgeType, args, NULL));
  Py_DECREF(args);
  if( edge == NULL ) {
    PyErr_SetString(PyExc_RuntimeError, "Could not create Edge");
    return NULL;
  }
  PYGTS_OBJECT(edge)->gtsobj = GTS_OBJECT(e);

  /* Attach parent */
  if( (parent = pygts_edge_parent(e)) == NULL ) {
    Py_DECREF(edge);
    return NULL;
  }
  PYGTS_OBJECT(edge)->gtsobj_parent = GTS_OBJECT(parent);

  pygts_object_register(PYGTS_OBJECT(edge));
  return (PyObject*)edge;
}


static PyObject*
opposite(PygtsTriangle *self, PyObject *args)
{
  PyObject *o_;
  PygtsEdge *e=NULL;
  PygtsVertex *v=NULL;
  GtsVertex *vertex=NULL,*v1,*v2,*v3;
  GtsEdge *edge=NULL;
  GtsTriangle *triangle;
  GtsObject *parent;

#if PYGTS_DEBUG
  if(!pygts_triangle_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &o_) ) {
    return NULL;
  }

  /* Convert to PygtsObjects */
  if(pygts_edge_check(o_)) {
    e = PYGTS_TRIANGLE(o_);
  }
  else {
    if(pygts_vertex_check(o_)) {
      v = PYGTS_TRIANGLE(o_);
    }
    else {
      PyErr_SetString(PyExc_TypeError,"expected an Edge or a Vertex");
      return NULL;
    }
  }

  /* Error check */
  triangle = GTS_TRIANGLE(PYGTS_OBJECT(self)->gtsobj);
  if( e!=NULL ) {
    edge = GTS_EDGE(PYGTS_OBJECT(e)->gtsobj);
    if(! ((triangle->e1==edge)||(triangle->e2==edge)||(triangle->e3==edge)) ) {
      PyErr_SetString(PyExc_RuntimeError,"Edge not in Triangle");
      return NULL;
    }
  }
  else {
    vertex = GTS_VERTEX(PYGTS_OBJECT(v)->gtsobj);
    gts_triangle_vertices(triangle,&v1,&v2,&v3);
    if(! ((vertex==v1)||(vertex==v2)||(vertex==v3)) ) {
      PyErr_SetString(PyExc_RuntimeError,"Vertex not in Triangle");
      return NULL;
    }
  }

  if( e!=NULL) {
    vertex = gts_triangle_vertex_opposite(triangle, edge);

    /* If corresponding PyObject found in object table, we are done */
    if( (v = PYGTS_VERTEX(g_hash_table_lookup(obj_table,vertex))) != NULL ) {
      Py_INCREF(v);
      return (PyObject*)v;
    }

    /* Chain up object allocation */
    args = Py_BuildValue("OOOi",Py_None,Py_None,Py_None,FALSE);
    v = PYGTS_VERTEX(PygtsVertexType.tp_new(&PygtsVertexType, args, NULL));
    Py_DECREF(args);
    if( v == NULL ) {
      PyErr_SetString(PyExc_RuntimeError, "Could not create Vertex");
      return NULL;
    }
    PYGTS_OBJECT(v)->gtsobj = GTS_OBJECT(vertex);

    /* Attach parent */
    if( (parent = GTS_OBJECT(pygts_vertex_parent(vertex))) == NULL ) {
      Py_DECREF(v);
      return NULL;
    }
    PYGTS_OBJECT(v)->gtsobj_parent = parent;

    pygts_object_register(PYGTS_OBJECT(v));
    return (PyObject*)v;
  }
  else{
    edge = gts_triangle_edge_opposite(triangle, vertex);

    /* If corresponding PyObject found in object table, we are done */
    if( (e = PYGTS_EDGE(g_hash_table_lookup(obj_table,edge))) != NULL ) {
      Py_INCREF(e);
      return (PyObject*)e;
    }

    /* Chain up object allocation */
    args = Py_BuildValue("OOi",Py_None,Py_None,FALSE);
    e = PYGTS_EDGE(PygtsEdgeType.tp_new(&PygtsEdgeType, args, NULL));
    Py_DECREF(args);
    if( e == NULL ) {
      PyErr_SetString(PyExc_RuntimeError, "Could not create Edge");
      return NULL;
    }
    PYGTS_OBJECT(e)->gtsobj = GTS_OBJECT(edge);

    /* Attach parent */
    if( (parent = GTS_OBJECT(pygts_edge_parent(edge))) == NULL ) {
      Py_DECREF(e);
      return NULL;
    }
    PYGTS_OBJECT(e)->gtsobj_parent = parent;

    pygts_object_register(PYGTS_OBJECT(e));
    return (PyObject*)e;
  }
}


static PyObject *
vertices(PygtsTriangle *self,PyObject *args)
{
  GtsVertex *v1_,*v2_,*v3_;
  PygtsObject *v1,*v2,*v3;
  GtsSegment *parent;

#if PYGTS_DEBUG
  if(!pygts_triangle_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Get the vertices */
  gts_triangle_vertices(GTS_TRIANGLE(PYGTS_OBJECT(self)->gtsobj),
			&v1_, &v2_, &v3_);

  /* Get object from table (if available); otherwise chain-up allocation
   * and attach parent.
   */
  args = Py_BuildValue("dddi",0,0,0,FALSE);
  if( (v1 = g_hash_table_lookup(obj_table,GTS_OBJECT(v1_))) != NULL ) {
    Py_INCREF(v1);
  }
  else {
    if( (v1 = PYGTS_OBJECT(PygtsVertexType.tp_new(&PygtsVertexType, 
						  args, NULL))) == NULL ) {
      Py_DECREF(args);
      return NULL;
    }
    v1->gtsobj = GTS_OBJECT(v1_);
    if( (parent = pygts_vertex_parent(GTS_VERTEX(v1->gtsobj))) == NULL ) {
      Py_DECREF(args);
      Py_DECREF(v1);
      return NULL;
    }
    v1->gtsobj_parent = GTS_OBJECT(parent);
    pygts_object_register(v1);
  }

  if( (v2 = g_hash_table_lookup(obj_table,GTS_OBJECT(v2_))) != NULL ) {
    Py_INCREF(v2);
  }
  else {
    if( (v2 = PYGTS_OBJECT(PygtsVertexType.tp_new(&PygtsVertexType, 
						  args, NULL))) == NULL ) {
      Py_DECREF(args);
      Py_DECREF(v1);
      return NULL;
    }
    v2->gtsobj = GTS_OBJECT(v2_);
    if( (parent = pygts_vertex_parent(GTS_VERTEX(v2->gtsobj))) == NULL ) {
      Py_DECREF(args);
      Py_DECREF(v1);
      Py_DECREF(v2);
      return NULL;
    }
    v2->gtsobj_parent = GTS_OBJECT(parent);
    pygts_object_register(v2);
  }

  if( (v3 = g_hash_table_lookup(obj_table,GTS_OBJECT(v3_))) != NULL ) {
    Py_INCREF(v3);
  }
  else {
    if( (v3 = PYGTS_OBJECT(PygtsVertexType.tp_new(&PygtsVertexType, 
						  args, NULL))) == NULL ) {
      Py_DECREF(args);
      Py_DECREF(v1);
      Py_DECREF(v2);
      return NULL;
    }
    v3->gtsobj = GTS_OBJECT(v3_);
    if( (parent = pygts_vertex_parent(GTS_VERTEX(v3->gtsobj))) == NULL ) {
      Py_DECREF(args);
      Py_DECREF(v1);
      Py_DECREF(v2);
      Py_DECREF(v3);
      return NULL;
    }
    v3->gtsobj_parent = GTS_OBJECT(parent);
    pygts_object_register(v3);
  }
  Py_DECREF(args);

#if PYGTS_DEBUG
  if(!pygts_vertex_check((PyObject*)v1)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
  if(!pygts_vertex_check((PyObject*)v2)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
  if(!pygts_vertex_check((PyObject*)v3)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  return Py_BuildValue("OOO",v1,v2,v3);
}


static PyObject *
vertex(PygtsTriangle *self,PyObject *args)
{
  GtsVertex *v1_;
  PygtsObject *v1;
  GtsSegment *parent;

#if PYGTS_DEBUG
  if(!pygts_triangle_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Get the vertices */
  v1_ = gts_triangle_vertex(GTS_TRIANGLE(PYGTS_OBJECT(self)->gtsobj));

  /* Get object from table (if available); otherwise chain-up allocation
   * and attach parent.
   */
  args = Py_BuildValue("dddi",0,0,0,FALSE);
  if( (v1 = g_hash_table_lookup(obj_table,GTS_OBJECT(v1_))) != NULL ) {
    Py_INCREF(v1);
  }
  else {
    if( (v1 = PYGTS_OBJECT(PygtsVertexType.tp_new(&PygtsVertexType, 
						  args, NULL))) == NULL ) {
      Py_DECREF(args);
      return NULL;
    }
    v1->gtsobj = GTS_OBJECT(v1_);
    if( (parent = pygts_vertex_parent(GTS_VERTEX(v1->gtsobj))) == NULL ) {
      Py_DECREF(args);
      Py_DECREF(v1);
      return NULL;
    }
    v1->gtsobj_parent = GTS_OBJECT(parent);
    pygts_object_register(v1);
  }

  return (PyObject*)v1;
}

static PyObject *
circumcenter(PygtsTriangle *self,PyObject *args)
{
  PygtsVertex *v;
  GtsVertex *vertex;
  GtsSegment *parent;


#if PYGTS_DEBUG
  if(!pygts_triangle_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Get the Vertex */
  vertex = GTS_VERTEX(
	       gts_triangle_circumcircle_center(
	           GTS_TRIANGLE(PYGTS_OBJECT(self)->gtsobj),
		   GTS_POINT_CLASS(gts_vertex_class())));

  if( vertex == NULL ) {
    Py_INCREF(Py_None);
    return Py_None;
  }

  /* Get object from table (if available); otherwise chain-up allocation
   * and attach parent.
   */
  if( (v = g_hash_table_lookup(obj_table,GTS_OBJECT(vertex))) != NULL ) {
    Py_INCREF(v);
  }
  else {
    args = Py_BuildValue("dddi",0,0,0,FALSE);
    if( (v = PYGTS_OBJECT(PygtsVertexType.tp_new(&PygtsVertexType, 
						 args, NULL))) == NULL ) {
      Py_DECREF(args);
      return NULL;
    }
    Py_DECREF(args);
    PYGTS_OBJECT(v)->gtsobj = GTS_OBJECT(vertex);
    if( (parent = pygts_vertex_parent(vertex)) == NULL ) {
      Py_DECREF(args);
      return NULL;
    }
    PYGTS_OBJECT(v)->gtsobj_parent = GTS_OBJECT(parent);
    pygts_object_register(v);
  }

  return (PyObject*)v;
}


/* Methods table */
static PyMethodDef methods[] = {
  {"is_ok", (PyCFunction)is_ok,
   METH_NOARGS,
   "True if this Triangle t is non-degenerate and non-duplicate.\n"
   "False otherwise.\n"
   "\n"
   "Signature: t.is_ok()\n"
  },  

  {"area", (PyCFunction)area,
   METH_NOARGS,
   "Returns the area of Triangle t.\n"
   "\n"
   "Signature: t.area()\n"
  },  

  {"perimeter", (PyCFunction)perimeter,
   METH_NOARGS,
   "Returns the perimeter of Triangle t.\n"
   "\n"
   "Signature: t.perimeter()\n"
  },  

  {"quality", (PyCFunction)quality,
   METH_NOARGS,
   "Returns the quality of Triangle t.\n"
   "\n"
   "The quality of a triangle is defined as the ratio of the square\n"
   "root of its surface area to its perimeter relative to this same\n"
   "ratio for an equilateral triangle with the same area.  The quality\n"
   "is then one for an equilateral triangle and tends to zero for a\n"
   "very stretched triangle."
   "\n"
   "Signature: t.quality()\n"
  },  

  {"normal", (PyCFunction)normal,
   METH_NOARGS,
   "Returns a tuple of coordinates of the oriented normal of Triangle t\n"
   "as the cross-product of two edges, using the left-hand rule.  The\n"
   "normal is not normalized.  If this triangle is part of a closed and\n" 
   "oriented surface, the normal points to the outside of the surface.\n"
   "\n"
   "Signature: t.normal()\n"
  },  

  {"revert", (PyCFunction)revert,
   METH_NOARGS,
   "Changes the orientation of triangle t, turning it inside out.\n"
   "\n"
   "Signature: t.revert()\n"
  },  

  {"orientation", (PyCFunction)orientation,
   METH_NOARGS,
   "Determines orientation of the plane (x,y) projection of Triangle t\n"
   "\n"
   "Signature: t.orientation()\n"
   "\n"
   "Returns a positive value if Points p1, p2 and p3 in Triangle t\n"
   "appear in counterclockwise order, a negative value if they appear\n"
   "in clockwise order and zero if they are colinear.\n"
  },  

  {"angle", (PyCFunction)angle,
   METH_VARARGS,
   "Returns the angle (radians) between Triangles t1 and t2\n"
   "\n"
   "Signature: t1.angle(t2)\n"
  },  

  {"is_compatible", (PyCFunction)is_compatible,
   METH_VARARGS,
   "True if this triangle t1 and other t2 are compatible;\n"
   "otherwise False.\n"
   "\n"
   "Checks if this triangle t1 and other t2, which share a common\n"
   "Edge, can be part of the same surface without conflict in the\n"
   "surface normal orientation.\n"
   "\n"
   "Signature: t1.is_compatible(t2)\n"
  },  

  {"common_edge", (PyCFunction)common_edge,
   METH_VARARGS,
   "Returns Edge common to both this Triangle t1 and other t2.\n"
   "Returns None if the triangles do not share an Edge.\n"
   "\n"
   "Signature: t1.common_edge(t2)\n"
  },  

  {"opposite", (PyCFunction)opposite,
   METH_VARARGS,
   "Returns Vertex opposite to Edge e or Edge opposite to Vertex v\n"
   "for this Triangle t.\n"
   "\n"
   "Signature: t.opposite(e) or t.opposite(v)\n"
  },  

  {"vertices", (PyCFunction)vertices,
   METH_NOARGS,
   "Returns the three oriented set of vertices in Triangle t.\n"
   "\n"
   "Signature: t.vertices()\n"
  },  

  {"vertex", (PyCFunction)vertex,
   METH_NOARGS,
   "Returns the Vertex of this Triangle t not in t.e1.\n"
   "\n"
   "Signature: t.vertex()\n"
  },  

  {"circumcenter", (PyCFunction)circumcenter,
   METH_NOARGS,
   "Returns a Vertex at the center of the circumscribing circle of\n"
   "this Triangle t, or None if the circumscribing circle is not\n"
   "defined.\n"
   "\n"
   "Signature: t.circumcircle_center()\n"
  },  

  {NULL}  /* Sentinel */
};


/*-------------------------------------------------------------------------*/
/* Attributes exported to python */

static PyObject *
get_e1(PygtsTriangle *self, void *closure)
{
  PygtsEdge *e1;
  GtsTriangle *parent;
  PyObject *args;

#if PYGTS_DEBUG
  if(!pygts_triangle_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Get object from table (if available) */
  if( (e1 = g_hash_table_lookup(obj_table,
                GTS_OBJECT(GTS_TRIANGLE(self->gtsobj)->e1))) != NULL ) {
    Py_INCREF(e1);
    return (PyObject*)e1;
  }

  /* Chain up object allocation */
  args = Py_BuildValue("OOi",Py_None,Py_None,FALSE);
  e1 = PYGTS_OBJECT(PygtsEdgeType.tp_new(&PygtsEdgeType, args, NULL));
  Py_DECREF(args);
  if( e1 == NULL ) {
    PyErr_SetString(PyExc_RuntimeError, "Could not create Edge");
    return NULL;
  }

  /* Object initialization */
  e1->gtsobj = GTS_OBJECT(GTS_TRIANGLE(self->gtsobj)->e1);
  if( (parent = pygts_edge_parent(GTS_EDGE(e1->gtsobj))) == NULL ) {
    e1->gtsobj = NULL; /* Don't want to deallocate it */
    Py_DECREF(e1);
    return NULL;
  }
  PYGTS_OBJECT(e1)->gtsobj_parent = GTS_OBJECT(parent);

  pygts_object_register(e1);

  return (PyObject *)e1;
}


static PyObject *
get_e2(PygtsTriangle *self, void *closure)
{
  PygtsEdge *e2;
  GtsTriangle *parent;
  PyObject *args;

#if PYGTS_DEBUG
  if(!pygts_triangle_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Get object from table (if available) */
  if( (e2 = g_hash_table_lookup(obj_table,
                GTS_OBJECT(GTS_TRIANGLE(self->gtsobj)->e2))) != NULL ) {
    Py_INCREF(e2);
    return (PyObject*)e2;
  }

  /* Chain up object allocation */
  args = Py_BuildValue("OOi",Py_None,Py_None,FALSE);
  e2 = PYGTS_OBJECT(PygtsEdgeType.tp_new(&PygtsEdgeType, args, NULL));
  Py_DECREF(args);
  if( e2 == NULL ) {
    PyErr_SetString(PyExc_RuntimeError, "Could not create Edge");
    return NULL;
  }

  /* Object initialization */
  e2->gtsobj = GTS_OBJECT(GTS_TRIANGLE(self->gtsobj)->e2);
  if( (parent = pygts_edge_parent(GTS_EDGE(e2->gtsobj))) == NULL ) {
    e2->gtsobj = NULL; /* Don't want to deallocate it */
    Py_DECREF(e2);
    return NULL;
  }
  PYGTS_OBJECT(e2)->gtsobj_parent = GTS_OBJECT(parent);

  pygts_object_register(e2);

  return (PyObject *)e2;
}


static PyObject *
get_e3(PygtsTriangle *self, void *closure)
{
  PygtsEdge *e3;
  GtsTriangle *parent;
  PyObject *args;

#if PYGTS_DEBUG
  if(!pygts_triangle_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Get object from table (if available) */
  if( (e3 = g_hash_table_lookup(obj_table,
                GTS_OBJECT(GTS_TRIANGLE(self->gtsobj)->e3))) != NULL ) {
    Py_INCREF(e3);
    return (PyObject*)e3;
  }

  /* Chain up object allocation */
  args = Py_BuildValue("OOi",Py_None,Py_None,FALSE);
  e3 = PYGTS_OBJECT(PygtsEdgeType.tp_new(&PygtsEdgeType, args, NULL));
  Py_DECREF(args);
  if( e3 == NULL ) {
    PyErr_SetString(PyExc_RuntimeError, "Could not create Edge");
    return NULL;
  }

  /* Object initialization */
  e3->gtsobj = GTS_OBJECT(GTS_TRIANGLE(self->gtsobj)->e3);
  if( (parent = pygts_edge_parent(GTS_EDGE(e3->gtsobj))) == NULL ) {
    e3->gtsobj = NULL; /* Don't want to deallocate it */
    Py_DECREF(e3);
    return NULL;
  }
  PYGTS_OBJECT(e3)->gtsobj_parent = GTS_OBJECT(parent);

  pygts_object_register(e3);

  return (PyObject *)e3;
}


/* Methods table */
static PyGetSetDef getset[] = {
    {"e1", (getter)get_e1, NULL, "Edge 1", NULL},

    {"e2", (getter)get_e2, NULL, "Edge 2", NULL},

    {"e3", (getter)get_e3, NULL, "Edge 3", NULL},

    {NULL}  /* Sentinel */
};


/*-------------------------------------------------------------------------*/
/* Python type methods */

static PyObject *
new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  PygtsObject *obj;
  GtsTriangle *tmp;
  GtsObject *triangle=NULL;

  PyObject *o1_,*o2_,*o3_;
  PygtsVertex *v1=NULL, *v2=NULL, *v3=NULL;
  PygtsEdge *e1=NULL,*e2=NULL,*e3=NULL;
  GtsSegment *s1,*s2,*s3;
  gboolean flag=FALSE;  /* Flag when the args are gts.Point objects */
  guint alloc_gtsobj = TRUE;
  static char *kwlist[] = {"o1", "o2", "o3", "alloc_gtsobj", NULL};

  /* Parse the args */  
  if( args != NULL ) {
    if(! PyArg_ParseTupleAndKeywords(args, kwds, "OOO|i", kwlist,&o1_,&o2_,&o3_,
				     &alloc_gtsobj) ) {
      return NULL;
    }
  }

  /* Allocate the gtsobj (if needed) */
  if( alloc_gtsobj ) {

    /* Convert to PygtsObjects */
    if( pygts_edge_check(o1_) ) {
      e1 = PYGTS_EDGE(o1_);
    }
    else {
      if( pygts_vertex_check(o1_) ) {
	v1 = PYGTS_VERTEX(o1_);
	flag = TRUE;
      }
    }

    if( pygts_edge_check(o2_) ) {
      e2 = PYGTS_EDGE(o2_);
    }
    else {
      if( pygts_vertex_check(o2_) ) {
	v2 = PYGTS_VERTEX(o2_);
	flag = TRUE;
      }
    }

    if( pygts_edge_check(o3_) ) {
      e3 = PYGTS_EDGE(o3_);
    }
    else {
      if(pygts_vertex_check(o3_)) {
	v3 = PYGTS_VERTEX(o3_);
	flag = TRUE;
      }
    }

    /* Check for three edges or three vertices */
    if( !((e1!=NULL && e2!=NULL && e3!=NULL) ||
	  (v1!=NULL && v2!=NULL && v3!=NULL)) ) {
      PyErr_SetString(PyExc_TypeError,
		      "3 gts.Edge or gts.Vertex objects expected");
      return NULL;
    }

    /* Get gts edges */
    if(flag) {
      if( (s1 = GTS_SEGMENT(gts_edge_new(gts_edge_class(),
					 GTS_VERTEX(PYGTS_OBJECT(v1)->gtsobj),
					 GTS_VERTEX(PYGTS_OBJECT(v2)->gtsobj)
					 )))==NULL ) {
	PyErr_SetString(PyExc_TypeError,"GTS could not create edge");
	return NULL;
      }
      if( (s2 = GTS_SEGMENT(gts_edge_new(gts_edge_class(),
					 GTS_VERTEX(PYGTS_OBJECT(v2)->gtsobj),
					 GTS_VERTEX(PYGTS_OBJECT(v3)->gtsobj)
					 )))==NULL ) {
	PyErr_SetString(PyExc_TypeError,"GTS could not create edge");
	gts_object_destroy(GTS_OBJECT(s1));
	return NULL;
      }
      if( (s3 = GTS_SEGMENT(gts_edge_new(gts_edge_class(),
					 GTS_VERTEX(PYGTS_OBJECT(v3)->gtsobj),
					 GTS_VERTEX(PYGTS_OBJECT(v1)->gtsobj)
					 )))==NULL ) {
	PyErr_SetString(PyExc_TypeError,"GTS could not create edge");
	gts_object_destroy(GTS_OBJECT(s1));
	gts_object_destroy(GTS_OBJECT(s2));
	return NULL;
      }
    }
    else {
      s1 = GTS_SEGMENT(PYGTS_OBJECT(e1)->gtsobj);
      s2 = GTS_SEGMENT(PYGTS_OBJECT(e2)->gtsobj);
      s3 = GTS_SEGMENT(PYGTS_OBJECT(e3)->gtsobj);
    }

    /* Check that edges connect with common vertices */
    if( !((s1->v1==s3->v2 && s1->v2==s2->v1 && s2->v2==s3->v1) ||
	  (s1->v1==s3->v2 && s1->v2==s2->v2 && s2->v1==s3->v1) ||
	  (s1->v1==s3->v1 && s1->v2==s2->v1 && s2->v2==s3->v2) ||
	  (s1->v2==s3->v2 && s1->v1==s2->v1 && s2->v2==s3->v1) ||
	  (s1->v1==s3->v1 && s1->v2==s2->v2 && s2->v1==s3->v2) ||
	  (s1->v2==s3->v2 && s1->v1==s2->v2 && s2->v1==s3->v1) ||
	  (s1->v2==s3->v1 && s1->v1==s2->v1 && s2->v2==s3->v2) ||
	  (s1->v2==s3->v1 && s1->v1==s2->v2 && s2->v1==s3->v2)) ) {
      PyErr_SetString(PyExc_TypeError,
		      "Edges in triangle must have common vertices");
      if(flag) {
	gts_object_destroy(GTS_OBJECT(s1));
	gts_object_destroy(GTS_OBJECT(s2));
	gts_object_destroy(GTS_OBJECT(s3));
      }
      return NULL;
    }

    /* Create the GtsTriangle */
    triangle = GTS_OBJECT(gts_triangle_new(gts_triangle_class(),
					   GTS_EDGE(s1),
					   GTS_EDGE(s2),
					   GTS_EDGE(s3)));
    if( triangle == NULL )  {
      PyErr_SetString(PyExc_RuntimeError, "GTS could not create Triangle");
      if(flag) {
	gts_object_destroy(GTS_OBJECT(s1));
	gts_object_destroy(GTS_OBJECT(s2));
	gts_object_destroy(GTS_OBJECT(s3));
      }
      return NULL;
    }

    /* Check for duplicate */
    tmp = gts_triangle_is_duplicate(GTS_TRIANGLE(triangle));
    if( tmp != NULL ) {
      gts_object_destroy(triangle);
      triangle = GTS_OBJECT(tmp);
    }

    /* If corresponding PyObject found in object table, we are done */
    if( (obj=g_hash_table_lookup(obj_table,triangle)) != NULL ) {
      Py_INCREF(obj);
      return (PyObject*)obj;
    }
  }
  
  /* Chain up */
  args = Py_BuildValue("OOOi",&o1_,&o2_,&o3_,FALSE);
  obj = PYGTS_OBJECT(PygtsObjectType.tp_new(type,args,NULL));
  Py_DECREF(args);

  if( alloc_gtsobj ) {
    obj->gtsobj = triangle;
    pygts_object_register(PYGTS_OBJECT(obj));
  }

  return (PyObject*)obj;
}


static int
init(PygtsTriangle *self, PyObject *args, PyObject *kwds)
{
  gint ret;

  /* Chain up */
  if( (ret=PygtsObjectType.tp_init((PyObject*)self,args,kwds)) != 0 ){
    return ret;
  }

#if PYGTS_DEBUG
  if(!pygts_triangle_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return -1;
  }
#endif

  return 0;
}


static int
compare(PyObject *o1, PyObject *o2)
{
  GtsTriangle *t1, *t2;

  if( !(pygts_triangle_check(o1) && pygts_triangle_check(o2)) ) {
    return -1;
  }
  t1 = GTS_TRIANGLE(PYGTS_TRIANGLE(o1)->gtsobj);
  t2 = GTS_TRIANGLE(PYGTS_TRIANGLE(o2)->gtsobj);
  
  return pygts_triangle_compare(t1,t2);  
}


/* Methods table */
PyTypeObject PygtsTriangleType = {
    PyObject_HEAD_INIT(NULL)
    0,                       /* ob_size */
    "gts.Triangle",          /* tp_name */
    sizeof(PygtsTriangle),   /* tp_basicsize */
    0,                       /* tp_itemsize */
    0,                       /* tp_dealloc */
    0,                       /* tp_print */
    0,                       /* tp_getattr */
    0,                       /* tp_setattr */
    (cmpfunc)compare,        /* tp_compare */
    0,                       /* tp_repr */
    0,                       /* tp_as_number */
    0,                       /* tp_as_sequence */
    0,                       /* tp_as_mapping */
    0,                       /* tp_hash */
    0,                       /* tp_call */
    0,                       /* tp_str */
    0,                       /* tp_getattro */
    0,                       /* tp_setattro */
    0,                       /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
      Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "Triangle object",       /* tp_doc */
    0,                       /* tp_traverse */
    0,                       /* tp_clear */
    0,                       /* tp_richcompare */
    0,                       /* tp_weaklistoffset */
    0,                       /* tp_iter */
    0,                       /* tp_iternext */
    methods,                 /* tp_methods */
    0,                       /* tp_members */
    getset,                  /* tp_getset */
    0,                       /* tp_base */
    0,                       /* tp_dict */
    0,                       /* tp_descr_get */
    0,                       /* tp_descr_set */
    0,                       /* tp_dictoffset */
    (initproc)init,          /* tp_init */
    0,                       /* tp_alloc */
    (newfunc)new             /* tp_new */
};


/*-------------------------------------------------------------------------*/
/* Pygts functions */

gboolean 
pygts_triangle_check(PyObject* o)
{
  if(! PyObject_TypeCheck(o, &PygtsTriangleType)) {
    return FALSE;
  }
  else {
#if PYGTS_DEBUG
    return pygts_triangle_is_ok(PYGTS_TRIANGLE(o));
#else
    return TRUE;
#endif
  }
}


gboolean 
pygts_triangle_is_ok(PygtsTriangle *t)
{
  if(!pygts_object_is_ok(PYGTS_OBJECT(t))) return FALSE;
  return pygts_gts_triangle_is_ok(GTS_TRIANGLE(PYGTS_OBJECT(t)->gtsobj));
}


int 
pygts_triangle_compare(GtsTriangle* t1,GtsTriangle* t2)
{
  if( (pygts_segment_compare(GTS_SEGMENT(t1->e1),GTS_SEGMENT(t2->e1))==0 &&
       pygts_segment_compare(GTS_SEGMENT(t1->e2),GTS_SEGMENT(t2->e2))==0 &&
       pygts_segment_compare(GTS_SEGMENT(t1->e3),GTS_SEGMENT(t2->e3))==0) ||
      (pygts_segment_compare(GTS_SEGMENT(t1->e1),GTS_SEGMENT(t2->e3))==0 &&
       pygts_segment_compare(GTS_SEGMENT(t1->e2),GTS_SEGMENT(t2->e1))==0 &&
       pygts_segment_compare(GTS_SEGMENT(t1->e3),GTS_SEGMENT(t2->e2))==0) ||
      (pygts_segment_compare(GTS_SEGMENT(t1->e1),GTS_SEGMENT(t2->e2))==0 &&
       pygts_segment_compare(GTS_SEGMENT(t1->e2),GTS_SEGMENT(t2->e3))==0 &&
       pygts_segment_compare(GTS_SEGMENT(t1->e3),GTS_SEGMENT(t2->e1))==0) ||

      (pygts_segment_compare(GTS_SEGMENT(t1->e1),GTS_SEGMENT(t2->e3))==0 &&
       pygts_segment_compare(GTS_SEGMENT(t1->e2),GTS_SEGMENT(t2->e2))==0 &&
       pygts_segment_compare(GTS_SEGMENT(t1->e3),GTS_SEGMENT(t2->e1))==0) ||
      (pygts_segment_compare(GTS_SEGMENT(t1->e1),GTS_SEGMENT(t2->e2))==0 &&
       pygts_segment_compare(GTS_SEGMENT(t1->e2),GTS_SEGMENT(t2->e1))==0 &&
       pygts_segment_compare(GTS_SEGMENT(t1->e3),GTS_SEGMENT(t2->e3))==0) ||
      (pygts_segment_compare(GTS_SEGMENT(t1->e1),GTS_SEGMENT(t2->e1))==0 &&
       pygts_segment_compare(GTS_SEGMENT(t1->e2),GTS_SEGMENT(t2->e3))==0 &&
       pygts_segment_compare(GTS_SEGMENT(t1->e3),GTS_SEGMENT(t2->e2))==0) ) {
    return 0;
  }
  return -1;
}


/**
 * gts_triangle_is_ok:
 * @t: a #GtsTriangle.
 *
 * Returns: %TRUE if @t is a non-degenerate, non-duplicate triangle,
 * %FALSE otherwise.
 */
gboolean pygts_gts_triangle_is_ok (GtsTriangle * t)
{
  g_return_val_if_fail (t != NULL, FALSE);
  g_return_val_if_fail (t->e1 != NULL, FALSE);
  g_return_val_if_fail (t->e2 != NULL, FALSE);
  g_return_val_if_fail (t->e3 != NULL, FALSE);
  g_return_val_if_fail (t->e1 != t->e2 && t->e1 != t->e3 && t->e2 != t->e3, 
                        FALSE);
  g_return_val_if_fail (gts_segments_touch (GTS_SEGMENT (t->e1), 
                                            GTS_SEGMENT (t->e2)),
                        FALSE);
  g_return_val_if_fail (gts_segments_touch (GTS_SEGMENT (t->e1), 
                                            GTS_SEGMENT (t->e3)), 
                        FALSE);
  g_return_val_if_fail (gts_segments_touch (GTS_SEGMENT (t->e2), 
                                            GTS_SEGMENT (t->e3)), 
                        FALSE);
  g_return_val_if_fail (GTS_SEGMENT (t->e1)->v1 != GTS_SEGMENT (t->e1)->v2, 
                        FALSE);
  g_return_val_if_fail (GTS_SEGMENT (t->e2)->v1 != GTS_SEGMENT (t->e2)->v2, 
                        FALSE);
  g_return_val_if_fail (GTS_SEGMENT (t->e3)->v1 != GTS_SEGMENT (t->e3)->v2, 
                        FALSE);
  /*  g_return_val_if_fail (GTS_OBJECT (t)->reserved == NULL, FALSE); */
  g_return_val_if_fail (!gts_triangle_is_duplicate (t), FALSE);
  return TRUE;
}
