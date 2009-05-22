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

  fprintf(stderr,"*** 1.1 ***\n");

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
