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

static PyObject*
merge(PyObject *self, PyObject *args)
{
  PyObject *tuple, *obj;
  guint i,N;
  GList *vertices=NULL,*v;
  gdouble epsilon;
  PygtsVertex *vertex;

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "Od", &tuple, &epsilon) ) {
    return NULL;
  }
  if(PyList_Check(tuple)) {
    tuple = PyList_AsTuple(tuple);
  }
  else {
    Py_INCREF(tuple);
  }
  if(!PyTuple_Check(tuple)) {
    Py_DECREF(tuple);
    PyErr_SetString(PyExc_TypeError,"expected a list or tuple of vertices");
    return NULL;
  }

  /* Assemble the GList */
  N = PyTuple_Size(tuple);
  for(i=N-1;i>0;i--) {
    obj = PyTuple_GET_ITEM(tuple,i);
    if(!pygts_vertex_check(obj)) {
      Py_DECREF(tuple);
      g_list_free(vertices);
      PyErr_SetString(PyExc_TypeError,"expected a list or tuple of vertices");
      return NULL;
    }
    vertices = g_list_prepend(vertices,GTS_VERTEX(PYGTS_OBJECT(obj)->gtsobj));
  }
  Py_DECREF(tuple);

  /* Make the call */
  vertices = pygts_vertices_merge(vertices,epsilon,NULL);

  /* Assemble the return tuple */
  N = g_list_length(vertices);
  if( (tuple=PyTuple_New(N)) == NULL) {
    PyErr_SetString(PyExc_TypeError,"could not create tuple");
    return NULL;
  }
  v = vertices;
  for(i=0;i<N;i++) {
    if( (vertex = PYGTS_VERTEX(g_hash_table_lookup(obj_table,
						   GTS_OBJECT(v->data))
			       )) ==NULL ) {
      PyErr_SetString(PyExc_TypeError,
		      "could not get object from table (internal error)");
      g_list_free(vertices);
      return NULL;
    }
    Py_INCREF(vertex);
    PyTuple_SET_ITEM(tuple,i,(PyObject*)vertex);
    v = v->next;
  }

  g_list_free(vertices);

  return tuple;
}


static PyObject*
vertices(PyObject *self, PyObject *args)
{
  PyObject *tuple, *obj;
  guint i,N;
  GSList *segments=NULL,*vertices=NULL,*v;
  PygtsVertex *vertex;
  GtsSegment *parent;

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &tuple) ) {
    return NULL;
  }
  if(PyList_Check(tuple)) {
    tuple = PyList_AsTuple(tuple);
  }
  else {
    Py_INCREF(tuple);
  }
  if(!PyTuple_Check(tuple)) {
    Py_DECREF(tuple);
    PyErr_SetString(PyExc_TypeError,"expected a list or tuple of segments");
    return NULL;
  }

  /* Assemble the GSList */
  N = PyTuple_Size(tuple);
  for(i=0;i<N;i++) {
    obj = PyTuple_GET_ITEM(tuple,N-1-i);
    if(!pygts_segment_check(obj)) {
      Py_DECREF(tuple);
      g_slist_free(segments);
      PyErr_SetString(PyExc_TypeError,"expected a list or tuple of segments");
      return NULL;
    }
    segments = g_slist_prepend(segments,GTS_SEGMENT(PYGTS_OBJECT(obj)->gtsobj));
  }
  Py_DECREF(tuple);

  /* Make the call */
  vertices = gts_vertices_from_segments(segments);
  g_slist_free(segments);

  /* Assemble the return tuple */
  N = g_slist_length(vertices);
  if( (tuple=PyTuple_New(N)) == NULL) {
    PyErr_SetString(PyExc_TypeError,"could not create tuple");
    return NULL;
  }
  v = vertices;
  for(i=0;i<N;i++) {
    if( (vertex = PYGTS_VERTEX(g_hash_table_lookup(obj_table,
						   GTS_OBJECT(v->data))
			       )) !=NULL ) {
      Py_INCREF(vertex);
    }
    else {
        /* Chain up object allocation */
      args = Py_BuildValue("dddi",0,0,0,FALSE);
      vertex = PYGTS_VERTEX(PygtsVertexType.tp_new(&PygtsVertexType, 
						   args, NULL));
      Py_DECREF(args);
      if( vertex == NULL ) {
	PyErr_SetString(PyExc_TypeError,"Could not create Vertex");
	Py_DECREF(tuple);
	g_slist_free(vertices);
	return NULL;
      }

      PYGTS_OBJECT(vertex)->gtsobj = GTS_OBJECT(v->data);

      /* Create the parent GtsSegment */
      if( (parent=GTS_SEGMENT(pygts_vertex_parent(
		      GTS_VERTEX(PYGTS_OBJECT(vertex)->gtsobj)))) == NULL ) {
	Py_DECREF(vertex);
	Py_DECREF(tuple);
	g_slist_free(vertices);
	return NULL;
      }
      PYGTS_OBJECT(vertex)->gtsobj_parent = GTS_OBJECT(parent);

      pygts_object_register(vertex);
    }
    PyTuple_SET_ITEM(tuple,i,(PyObject*)vertex);
    v = v->next;
  }

  g_slist_free(vertices);

  return tuple;
}


static PyObject*
segments(PyObject *self, PyObject *args)
{
  PyObject *tuple, *obj;
  guint i,n,N;
  GSList *segments=NULL,*vertices=NULL,*s;
  PygtsSegment *segment;
  GtsTriangle *parent;

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &tuple) ) {
    return NULL;
  }
  if(PyList_Check(tuple)) {
    tuple = PyList_AsTuple(tuple);
  }
  else {
    Py_INCREF(tuple);
  }
  if(!PyTuple_Check(tuple)) {
    Py_DECREF(tuple);
    PyErr_SetString(PyExc_TypeError,"expected a list or tuple of vertices");
    return NULL;
  }

  /* Assemble the GSList */
  N = PyTuple_Size(tuple);
  for(i=0;i<N;i++) {
    obj = PyTuple_GET_ITEM(tuple,i);
    if(!pygts_vertex_check(obj)) {
      Py_DECREF(tuple);
      g_slist_free(vertices);
      PyErr_SetString(PyExc_TypeError,"expected a list or tuple of vertices");
      return NULL;
    }
    vertices = g_slist_prepend(vertices,GTS_VERTEX(PYGTS_OBJECT(obj)->gtsobj));
  }
  Py_DECREF(tuple);

  /* Make the call */
  if( (segments = gts_segments_from_vertices(vertices)) == NULL ) {
    PyErr_SetString(PyExc_TypeError,"could not retrieve segments");
    return NULL;
  }
  g_slist_free(vertices);

  /* Assemble the return tuple */
  N = g_slist_length(segments);
  if( (tuple=PyTuple_New(N)) == NULL) {
    PyErr_SetString(PyExc_TypeError,"could not create tuple");
    return NULL;
  }

  s = segments;
  n=0;
  while(s!=NULL) {
    if( (segment = PYGTS_SEGMENT(g_hash_table_lookup(obj_table,
						     GTS_OBJECT(s->data))
				 )) !=NULL ) {
      Py_INCREF(segment);
    }
    else {

      /* Skip any parent segments */
      if(PYGTS_IS_PARENT_SEGMENT(s->data) || PYGTS_IS_PARENT_EDGE(s->data)) {
	s = s->next;
	segment = NULL;
	continue;
      }

      /* Chain up object allocation for segments or edges */
      if(GTS_IS_EDGE(s->data)) {
	args = Py_BuildValue("OOi",Py_None,Py_None,FALSE);
	segment = PYGTS_SEGMENT(PygtsEdgeType.tp_new(&PygtsEdgeType, 
						     args, NULL));
      }
      else {
	args = Py_BuildValue("OOi",Py_None,Py_None,FALSE);
	segment = PYGTS_SEGMENT(PygtsSegmentType.tp_new(&PygtsSegmentType, 
							args, NULL));
      }
      Py_DECREF(args);
      if( segment == NULL ) {
	PyErr_SetString(PyExc_TypeError,"Could not create Segment");
	Py_DECREF(tuple);
	g_slist_free(segments);
	return NULL;
      }

      PYGTS_OBJECT(segment)->gtsobj = GTS_OBJECT(s->data);

      /* Create the parent GtsTriangle */
      if(GTS_IS_EDGE(s->data)) {
	if( (parent=GTS_TRIANGLE(pygts_edge_parent(
		      GTS_EDGE(PYGTS_OBJECT(segment)->gtsobj)))) == NULL ) {
	  Py_DECREF(segment);
	  Py_DECREF(tuple);
	  g_slist_free(segments);
	  return NULL;
	}
	PYGTS_OBJECT(segment)->gtsobj_parent = GTS_OBJECT(parent);
      }

      pygts_object_register(segment);
    }
    PyTuple_SET_ITEM(tuple,n,(PyObject*)segment);
    s = s->next;
    n += 1;
  }

  g_slist_free(segments);

  if(_PyTuple_Resize(&tuple,n)!=0) {
    Py_DECREF(tuple);
    return NULL;
  }

  return tuple;
}


static PyObject*
triangles(PyObject *self, PyObject *args)
{
  PyObject *tuple, *obj;
  guint i,n,N;
  GSList *edges=NULL,*triangles=NULL,*t;
  PygtsTriangle *triangle;
  GtsSurface *parent;

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &tuple) ) {
    return NULL;
  }
  if(PyList_Check(tuple)) {
    tuple = PyList_AsTuple(tuple);
  }
  else {
    Py_INCREF(tuple);
  }
  if(!PyTuple_Check(tuple)) {
    Py_DECREF(tuple);
    PyErr_SetString(PyExc_TypeError,"expected a list or tuple of edges");
    return NULL;
  }

  /* Assemble the GSList */
  N = PyTuple_Size(tuple);
  for(i=0;i<N;i++) {
    obj = PyTuple_GET_ITEM(tuple,i);
    if(!pygts_edge_check(obj)) {
      Py_DECREF(tuple);
      g_slist_free(edges);
      PyErr_SetString(PyExc_TypeError,"expected a list or tuple of edges");
      return NULL;
    }
    edges = g_slist_prepend(edges,GTS_EDGE(PYGTS_OBJECT(obj)->gtsobj));
  }
  Py_DECREF(tuple);

  /* Make the call */
  if( (triangles = gts_triangles_from_edges(edges)) == NULL ) {
    PyErr_SetString(PyExc_TypeError,"could not retrieve triangles");
    return NULL;
  }
  g_slist_free(edges);

  /* Assemble the return tuple */
  N = g_slist_length(triangles);
  if( (tuple=PyTuple_New(N)) == NULL) {
    PyErr_SetString(PyExc_TypeError,"could not create tuple");
    return NULL;
  }

  t = triangles;
  n=0;
  while(t!=NULL) {
    if( (triangle = PYGTS_TRIANGLE(g_hash_table_lookup(obj_table,
						       GTS_OBJECT(t->data))
				   )) !=NULL ) {
      Py_INCREF(triangle);
    }
    else {

      /* Skip any parent triangles */
      if(PYGTS_IS_PARENT_TRIANGLE(t->data)) {
	t = t->next;
	triangle = NULL;
	continue;
      }

      /* Chain up object allocation for triangles or faces */
      if(GTS_IS_FACE(t->data)) {
	args = Py_BuildValue("OOOi",Py_None,Py_None,Py_None,FALSE);
	triangle = PYGTS_TRIANGLE(PygtsFaceType.tp_new(&PygtsFaceType, 
						       args, NULL));
      }
      else {
	args = Py_BuildValue("OOOi",Py_None,Py_None,Py_None,FALSE);
	triangle = PYGTS_TRIANGLE(PygtsTriangleType.tp_new(&PygtsTriangleType, 
							   args, NULL));
      }
      Py_DECREF(args);
      if( triangle == NULL ) {
	PyErr_SetString(PyExc_TypeError,"Could not create Triangle");
	Py_DECREF(tuple);
	g_slist_free(triangles);
	return NULL;
      }

      PYGTS_OBJECT(triangle)->gtsobj = GTS_OBJECT(t->data);

      /* Create the parent GtsSurface */
      if(GTS_IS_FACE(t->data)) {
	if( (parent=GTS_SURFACE(pygts_face_parent(
		      GTS_FACE(PYGTS_OBJECT(triangle)->gtsobj)))) == NULL ) {
	  Py_DECREF(triangle);
	  Py_DECREF(tuple);
	  g_slist_free(triangles);
	  return NULL;
	}
	PYGTS_OBJECT(triangle)->gtsobj_parent = GTS_OBJECT(parent);
      }

      pygts_object_register(triangle);
    }
    PyTuple_SET_ITEM(tuple,n,(PyObject*)triangle);
    t = t->next;
    n += 1;
  }

  g_slist_free(triangles);

  if(_PyTuple_Resize(&tuple,n)!=0) {
    Py_DECREF(tuple);
    return NULL;
  }

  return tuple;
}


static PyObject*
parent(PygtsSurface *self, PyObject *args)
{
  PyObject *e_;
  PygtsEdge *e;
  GtsFace *f;
  PygtsFace *face;
  GtsSurface *parent;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &e_) )
    return NULL;

  /* Convert to PygtsObjects */
  if(!pygts_edge_check(e_)) {
    PyErr_SetString(PyExc_TypeError,"expected an Edge");
    return NULL;
  }
  e = PYGTS_EDGE(e_);

  if( (f=gts_edge_has_parent_surface(GTS_EDGE(PYGTS_OBJECT(e)->gtsobj),
				     GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj)))
      == NULL ) {

    Py_INCREF(Py_None);
    return Py_None;
  }

  if( (face = PYGTS_FACE(g_hash_table_lookup(obj_table,GTS_OBJECT(f))
			 )) !=NULL ) {
      Py_INCREF(face);
  }
  else {

    /* Chain up object allocation for faces */
    args = Py_BuildValue("OOOi",Py_None,Py_None,Py_None,FALSE);
    face = PYGTS_FACE(PygtsFaceType.tp_new(&PygtsFaceType, args, NULL));
    Py_DECREF(args);
    if( face == NULL ) {
      PyErr_SetString(PyExc_TypeError,"could not create Face");
      return NULL;
    }

    PYGTS_OBJECT(face)->gtsobj = GTS_OBJECT(f);

    /* Create the parent GtsSurface */
    if( (parent=GTS_SURFACE(pygts_face_parent(
		    GTS_FACE(PYGTS_OBJECT(face)->gtsobj)))) == NULL ) {
	Py_DECREF(face);
	return NULL;
    }
    PYGTS_OBJECT(face)->gtsobj_parent = GTS_OBJECT(parent);

    pygts_object_register(face);
  }

  return (PyObject*)face;
}


static PyObject*
triangle_enclosing(PyObject *self, PyObject *args)
{
  PyObject *tuple, *obj;
  guint i,N;
  GSList *points=NULL;
  GtsTriangle *t;
  PygtsTriangle *triangle;

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &tuple) ) {
    return NULL;
  }
  if(PyList_Check(tuple)) {
    tuple = PyList_AsTuple(tuple);
  }
  else {
    Py_INCREF(tuple);
  }
  if(!PyTuple_Check(tuple)) {
    Py_DECREF(tuple);
    PyErr_SetString(PyExc_TypeError,"expected a list or tuple of points");
    return NULL;
  }

  /* Assemble the GSList */
  N = PyTuple_Size(tuple);
  for(i=0;i<N;i++) {
    obj = PyTuple_GET_ITEM(tuple,i);
    if(!pygts_point_check(obj)) {
      Py_DECREF(tuple);
      g_slist_free(points);
      PyErr_SetString(PyExc_TypeError,"expected a list or tuple of points");
      return NULL;
    }
    points = g_slist_prepend(points,GTS_POINT(PYGTS_OBJECT(obj)->gtsobj));
  }
  Py_DECREF(tuple);

  /* Make the call */
  t = gts_triangle_enclosing(gts_triangle_class(),points,1.0);
  g_slist_free(points);

  if(t==NULL) {
    PyErr_SetString(PyExc_RuntimeError,"could not compute triangle");
    return NULL;
  }

  /* Prepare the return Triangle */
  args = Py_BuildValue("OOOi",Py_None,Py_None,Py_None,FALSE);
  triangle = PYGTS_TRIANGLE(PygtsTriangleType.tp_new(&PygtsTriangleType, 
						     args, NULL));
  Py_DECREF(args);
  if( triangle == NULL ) {
    PyErr_SetString(PyExc_TypeError,"Could not create Triangle");
    return NULL;
  }

  PYGTS_OBJECT(triangle)->gtsobj = GTS_OBJECT(t);

  pygts_object_register(triangle);

  return (PyObject*)triangle;
}


static PyObject*
sphere(PyObject *self, PyObject *args)
{
  PygtsSurface *surface;
  guint geodesation_order;

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "i", &geodesation_order) )
    return NULL;

  /* Chain up object allocation */
  args = Py_BuildValue("(i)",TRUE);
  surface = PYGTS_SURFACE(PygtsSurfaceType.tp_new(&PygtsSurfaceType, 
						  args, NULL));
  Py_DECREF(args);
  if( surface == NULL ) {
    PyErr_SetString(PyExc_RuntimeError, "Could not create Surface");
    return NULL;
  }

  gts_surface_generate_sphere(GTS_SURFACE(PYGTS_OBJECT(surface)->gtsobj),
			      geodesation_order);

  pygts_object_register(PYGTS_OBJECT(surface));
  return (PyObject*)surface;
}


static PyMethodDef gts_methods[] = {
  { "sphere", sphere, METH_VARARGS,
    "Returns a unit sphere generated by recursive subdivision.\n"
    "First approximation is an isocahedron; each level of refinement\n"
    "(geodesation_order) increases the number of triangles by a factor\n"
    "of 4.\n"
    "\n"
    "Signature: sphere(geodesation_order)\n"
  },

  { "merge", merge, METH_VARARGS,
    "Merges list of Vertices that are within a box of side-length\n"
    "epsilon of each other.\n"
    "\n"
    "Signature: merge(list,epsilon)\n"
  },

  { "vertices", vertices, METH_VARARGS,
    "Returns tuple of Vertices from a list or tuple of Segments.\n"
    "\n"
    "Signature: vertices(list)\n"
  },

  { "segments", segments, METH_VARARGS,
    "Returns tuple of Segments from a list or tuple of Vertices.\n"
    "\n"
    "Signature: segments(list)\n"
  },

  { "triangles", triangles, METH_VARARGS,
    "Returns tuple of Triangles from a list or tuple of Edges.\n"
    "\n"
    "Signature: triangles(list)\n"
  },

  { "triangle_enclosing", triangle_enclosing, METH_VARARGS,
    "Returns a Triangle that encloses the plane projection of a list\n"
    "or tuple of Points.  The Triangle is equilateral and encloses a\n"
    "rectangle defined by the maximum and minimum x and y coordinates\n"
    "of the points.\n"
    "\n"
    "Signature: triangles(list)\n"
  },

  {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
init_gts(void) 
{
  PyObject* m;

  /* Allocate the object table */
  if( (obj_table=g_hash_table_new(NULL,NULL)) == NULL ) return;

  /* Set class base types and make ready (i.e., inherit methods) */
  if (PyType_Ready(&PygtsObjectType) < 0) return;

  PygtsPointType.tp_base = &PygtsObjectType;
  if (PyType_Ready(&PygtsPointType) < 0) return;

  PygtsVertexType.tp_base = &PygtsPointType;
  if (PyType_Ready(&PygtsVertexType) < 0) return;

  PygtsSegmentType.tp_base = &PygtsObjectType;
  if (PyType_Ready(&PygtsSegmentType) < 0) return;

  PygtsEdgeType.tp_base = &PygtsSegmentType;
  if (PyType_Ready(&PygtsEdgeType) < 0) return;

  PygtsTriangleType.tp_base = &PygtsObjectType;
  if (PyType_Ready(&PygtsTriangleType) < 0) return;

  PygtsFaceType.tp_base = &PygtsTriangleType;
  if (PyType_Ready(&PygtsFaceType) < 0) return;

  PygtsSurfaceType.tp_base = &PygtsObjectType;
  if (PyType_Ready(&PygtsSurfaceType) < 0) return;


  /* Initialize the module */
  m = Py_InitModule3("_gts", gts_methods,"Gnu Triangulated Surface Library");
  if (m == NULL) return;


  /* Add new types to python */
  Py_INCREF(&PygtsObjectType);
  PyModule_AddObject(m, "Object", (PyObject *)&PygtsObjectType);

  Py_INCREF(&PygtsPointType);
  PyModule_AddObject(m, "Point", (PyObject *)&PygtsPointType);

  Py_INCREF(&PygtsVertexType);
  PyModule_AddObject(m, "Vertex", (PyObject *)&PygtsVertexType);

  Py_INCREF(&PygtsSegmentType);
  PyModule_AddObject(m, "Segment", (PyObject *)&PygtsSegmentType);

  Py_INCREF(&PygtsEdgeType);
  PyModule_AddObject(m, "Edge", (PyObject *)&PygtsEdgeType);

  Py_INCREF(&PygtsTriangleType);
  PyModule_AddObject(m, "Triangle", (PyObject *)&PygtsTriangleType);

  Py_INCREF(&PygtsFaceType);
  PyModule_AddObject(m, "Face", (PyObject *)&PygtsFaceType);

  Py_INCREF(&PygtsSurfaceType);
  PyModule_AddObject(m, "Surface", (PyObject *)&PygtsSurfaceType);
}
