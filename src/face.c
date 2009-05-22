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
is_ok(PygtsFace *self, PyObject *args, PyObject *kwds)
{
  if(pygts_face_is_ok(self)) {
    Py_INCREF(Py_True);
    return Py_True;
  }
  else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}


static PyObject*
is_unattached(PygtsFace *self, PyObject *args, PyObject *kwds)
{
  guint n;

  /* Check for attachments other than to the gtsobj_parent */
  n = g_slist_length(GTS_FACE(PYGTS_OBJECT(self)->gtsobj)->surfaces);
  if( n > 1 ) {
    Py_INCREF(Py_False);
    return Py_False;
  }
  else if( n == 1 ){
    Py_INCREF(Py_True);
    return Py_True;
  }
  else {
    PyErr_SetString(PyExc_TypeError,"Face lost parent (internal error)");
    return NULL;
  }
}


static PyObject*
neighbor_number(PygtsFace *self, PyObject *args)
{
  PyObject *s_=NULL;
  PygtsSurface *s=NULL;

#if PYGTS_DEBUG
  if(!pygts_face_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &s_) )
    return NULL;

  /* Convert to PygtsObjects */
  if( pygts_surface_check(s_) ) {
    s = PYGTS_SURFACE(s_);
  }
  else {
    PyErr_SetString(PyExc_TypeError,
		    "expected a Surface");
      return NULL;
  }

  return Py_BuildValue("i",
      gts_face_neighbor_number(GTS_FACE(PYGTS_OBJECT(self)->gtsobj),
			       GTS_SURFACE(PYGTS_OBJECT(s)->gtsobj)));
}


static PyObject*
neighbors(PygtsFace *self, PyObject *args)
{
  PyObject *s_=NULL;
  PygtsSurface *s=NULL;
  guint i,N;
  PyObject *tuple;
  GSList *faces,*f;
  PygtsFace *face;
  GtsSurface *parent;

#if PYGTS_DEBUG
  if(!pygts_face_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &s_) )
    return NULL;

  /* Convert to PygtsObjects */
  if( pygts_surface_check(s_) ) {
    s = PYGTS_SURFACE(s_);
  }
  else {
    PyErr_SetString(PyExc_TypeError,
		    "expected a Surface");
      return NULL;
  }

  N = gts_face_neighbor_number(GTS_FACE(PYGTS_OBJECT(self)->gtsobj),
			       GTS_SURFACE(PYGTS_OBJECT(s)->gtsobj));

  if( (tuple=PyTuple_New(N)) == NULL) {
    PyErr_SetString(PyExc_TypeError,"Could not create tuple");
    return NULL;
  }

  /* Get the neighbors */
  faces = gts_face_neighbors(GTS_FACE(PYGTS_OBJECT(self)->gtsobj),
			     GTS_SURFACE(PYGTS_OBJECT(s)->gtsobj));
  f = faces;
			     
  for(i=0;i<N;i++) {
    if( (face=PYGTS_FACE(g_hash_table_lookup(obj_table,PYGTS_OBJECT(f->data))))
	== NULL ) {

      /* Chain up object allocation */
      args = Py_BuildValue("OOOi",Py_None,Py_None,Py_None,FALSE);
      face = PYGTS_FACE(PygtsFaceType.tp_new(&PygtsFaceType, args, NULL));
      Py_DECREF(args);
      if( face == NULL ) {
	Py_DECREF(tuple);
	PyErr_SetString(PyExc_RuntimeError, "Could not create Face");
	return NULL;
      }

      /* Initialization */
      if( (parent = pygts_face_parent(GTS_FACE(f->data))) == NULL ) {
	Py_DECREF(tuple);
	Py_DECREF(face);
	return NULL;
      }
      PYGTS_OBJECT(face)->gtsobj = GTS_OBJECT(f->data);
      PYGTS_OBJECT(face)->gtsobj_parent = GTS_OBJECT(parent);
    }
    else {
      Py_INCREF(face);
    }

    /* Add Face to the tuple */
    PyTuple_SET_ITEM(tuple, i, (PyObject*)face);
    pygts_object_register(PYGTS_OBJECT(face));

    f = g_slist_next(f);
  }  

  return (PyObject*)tuple;
}


static PyObject*
is_compatible(PygtsFace *self, PyObject *args)
{
  PyObject *o1_=NULL;
  GtsEdge *e=NULL;
  PygtsTriangle *t=NULL;
  PygtsSurface *s=NULL;

#if PYGTS_DEBUG
  if(!pygts_face_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &o1_) )
    return NULL;

  /* Convert to PygtsObjects */
  if( pygts_triangle_check(o1_) ) {
    t = PYGTS_TRIANGLE(o1_);
  }
  else {
    if( pygts_surface_check(o1_) ) {
      s = PYGTS_SURFACE(o1_);
    }
    else {
      PyErr_SetString(PyExc_TypeError,"expected a Triangle or Surface");
      return NULL;
    }
  }

  if(t!=NULL) {
    if( (e = gts_triangles_common_edge(GTS_TRIANGLE(PYGTS_OBJECT(self)->gtsobj),
				       GTS_TRIANGLE(PYGTS_OBJECT(t)->gtsobj))) 
	== NULL ) {
      PyErr_SetString(PyExc_TypeError,"Faces do not share common edge");
      return NULL;
    }
    if(gts_triangles_are_compatible(GTS_TRIANGLE(PYGTS_OBJECT(self)->gtsobj),
				    GTS_TRIANGLE(PYGTS_OBJECT(t)->gtsobj),
				    e)==TRUE) {

      Py_INCREF(Py_True);
      return Py_True;
    }
  }
  else {
    if(gts_face_is_compatible(GTS_FACE(PYGTS_OBJECT(self)->gtsobj),
			      GTS_SURFACE(PYGTS_OBJECT(s)->gtsobj))==TRUE) {
      Py_INCREF(Py_True);
      return Py_True;
    }
  }
  Py_INCREF(Py_False);
  return Py_False;
}


static PyObject*
is_on(PygtsFace *self, PyObject *args)
{
  PyObject *s_=NULL;
  PygtsSurface *s=NULL;

#if PYGTS_DEBUG
  if(!pygts_face_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &s_) )
    return NULL;

  /* Convert to PygtsObjects */
  if( pygts_surface_check(s_) ) {
    s = PYGTS_SURFACE(s_);
  }
  else {
    PyErr_SetString(PyExc_TypeError,
		    "expected a Surface");
      return NULL;
  }

  if( gts_face_has_parent_surface(GTS_FACE(PYGTS_OBJECT(self)->gtsobj),
				  GTS_SURFACE(PYGTS_OBJECT(s)->gtsobj)) ) {
    Py_INCREF(Py_True);
    return Py_True;
  }
  else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}


/* Methods table */
static PyMethodDef methods[] = {
  {"is_ok", (PyCFunction)is_ok,
   METH_NOARGS,
   "True if this Face f is non-degenerate and non-duplicate.\n"
   "False otherwise.\n"
   "\n"
   "Signature: f.is_ok()\n"
  },  

  {"is_unattached", (PyCFunction)is_unattached,
   METH_NOARGS,
   "True if this Face f is not part of any Surface.\n"
   "\n"
   "Signature: f.is_unattached().\n"
  },

  {"neighbor_number", (PyCFunction)neighbor_number,
   METH_VARARGS,
   "Returns the number of neighbors of Face f belonging to Surface s.\n"
   "\n"
   "Signature: f.neighbor_number(s).\n"
  },

  {"neighbors", (PyCFunction)neighbors,
   METH_VARARGS,
   "Returns a tuple of neighbors of this Face f belonging to Surface s.\n"
   "\n"
   "Signature: f.neighbors(s).\n"
  },

  {"is_compatible", (PyCFunction)is_compatible,
   METH_VARARGS,
   "True if Face f is compatible with all neighbors in Surface s.\n"
   "False otherwise.\n"
   "\n"
   "Signature: f.is_compatible(s).\n"
  },  

  {"is_on", (PyCFunction)is_on,
   METH_VARARGS,
   "True if this Face f is on Surface s.  False otherwise.\n"
   "\n"
   "Signature: f.is_on(s).\n"
  },  

  {NULL}  /* Sentinel */
};


/*-------------------------------------------------------------------------*/
/* Python type methods */


static PyObject *
new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  PygtsObject *obj;
  GtsTriangle *tmp;
  GtsObject *face=NULL;
  GtsSurface *parent;

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
		      "Edges in face must have common vertices");
      if(flag) {
	gts_object_destroy(GTS_OBJECT(s1));
	gts_object_destroy(GTS_OBJECT(s2));
	gts_object_destroy(GTS_OBJECT(s3));
      }
      return NULL;
    }

    /* Create the GtsFace */
    face = GTS_OBJECT(gts_face_new(gts_face_class(),
				   GTS_EDGE(s1),
				   GTS_EDGE(s2),
				   GTS_EDGE(s3)));
    if( face == NULL )  {
      PyErr_SetString(PyExc_RuntimeError, "GTS could not create Face");
      if(flag) {
	gts_object_destroy(GTS_OBJECT(s1));
	gts_object_destroy(GTS_OBJECT(s2));
	gts_object_destroy(GTS_OBJECT(s3));
      }
      return NULL;
    }

    /* Check for duplicate */
    tmp = gts_triangle_is_duplicate(GTS_TRIANGLE(face));
    if( tmp != NULL ) {
      gts_object_destroy(face);
      face = GTS_OBJECT(tmp);
    }

    /* If corresponding PyObject found in object table, we are done */
    if( (obj=g_hash_table_lookup(obj_table,face)) != NULL ) {
      Py_INCREF(obj);
      return (PyObject*)obj;
    }
  }
  
  /* Chain up */
  args = Py_BuildValue("OOOi",&o1_,&o2_,&o3_,FALSE);
  obj = PYGTS_OBJECT(PygtsTriangleType.tp_new(type,args,NULL));
  Py_DECREF(args);

  if( alloc_gtsobj ) {

    obj->gtsobj = face;

    /* Create the parent GtsSurface */
    if( (parent = pygts_face_parent(GTS_FACE(obj->gtsobj))) == NULL ) {
      gts_object_destroy(obj->gtsobj);
      obj->gtsobj = NULL;
      return NULL;
    }
    obj->gtsobj_parent = GTS_OBJECT(parent);

    pygts_object_register(PYGTS_OBJECT(obj));
  }

  return (PyObject*)obj;
}


static int
init(PygtsFace *self, PyObject *args, PyObject *kwds)
{
  gint ret;

  /* Chain up */
  if( (ret=PygtsTriangleType.tp_init((PyObject*)self,args,kwds)) != 0 ){
    return ret;
  }

#if PYGTS_DEBUG
  if(!pygts_face_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return -1;
  }
#endif

  return 0;
}


/* Methods table */
PyTypeObject PygtsFaceType = {
    PyObject_HEAD_INIT(NULL)
    0,                       /* ob_size */
    "gts.Face",              /* tp_name */
    sizeof(PygtsFace),       /* tp_basicsize */
    0,                       /* tp_itemsize */
    0,                       /* tp_dealloc */
    0,                       /* tp_print */
    0,                       /* tp_getattr */
    0,                       /* tp_setattr */
    0,                       /* tp_compare */
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
    "Face object",           /* tp_doc */
    0,                       /* tp_traverse */
    0,                       /* tp_clear */
    0,                       /* tp_richcompare */
    0,                       /* tp_weaklistoffset */
    0,                       /* tp_iter */
    0,                       /* tp_iternext */
    methods,                 /* tp_methods */
    0,                       /* tp_members */
    0,                       /* tp_getset */
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
pygts_face_check(PyObject* o)
{
  if(! PyObject_TypeCheck(o, &PygtsFaceType)) {
    return FALSE;
  }
  else {
#if PYGTS_DEBUG
    return pygts_face_is_ok(PYGTS_FACE(o));
#else
    return TRUE;
#endif
  }
}


gboolean 
pygts_face_is_ok(PygtsFace *f)
{
  GSList *parent;
  PygtsObject *obj;

  obj = PYGTS_OBJECT(f);

  if(!pygts_triangle_is_ok(PYGTS_TRIANGLE(f))) return FALSE;

  /* Check for a valid parent */
  g_return_val_if_fail(obj->gtsobj_parent!=NULL,FALSE);
  g_return_val_if_fail(GTS_IS_SURFACE(obj->gtsobj_parent),FALSE);
  parent = g_slist_find(GTS_FACE(obj->gtsobj)->surfaces,
			obj->gtsobj_parent);
  g_return_val_if_fail(parent!=NULL,FALSE);

  return TRUE;
}


GtsSurface*
pygts_face_parent(GtsFace *face) {
  GtsSurface *parent;

  parent = gts_surface_new(gts_surface_class(),
			   gts_face_class(),
			   gts_edge_class(),
			   gts_vertex_class());

  if( parent == NULL )  {
    PyErr_SetString(PyExc_RuntimeError, "GTS could not create Surface");
    return NULL;
  }
  gts_surface_add_face(parent,face);

  return parent;
}
