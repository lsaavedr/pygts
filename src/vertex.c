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
/* Parent GTS segment for GTS vertices */

/* Define a GtsSegment subclass that can be readily identified as the parent 
 * of an encapsulated GtsVertex.  The pygts_parent_segment_class() function 
 * is defined at the bottom, and is what ultimately allows the distinction 
 * to be made.  This capability is used for vertex replacement operations.
 */
typedef struct _GtsSegment PygtsParentSegment;

#define PYGTS_PARENT_SEGMENT(obj) GTS_OBJECT_CAST(obj,\
					     GtsSegment,\
					     pygts_parent_segment_class())

#define PYGTS_IS_PARENT_SEGMENT(obj)(gts_object_is_from_class (obj,\
                                             pygts_parent_segment_class ()))


static GtsSegmentClass *pygts_parent_segment_class(void);


/* GTS vertex used parent segments */

typedef struct _GtsVertex PygtsParentVertex;

#define PYGTS_PARENT_VERTEX(obj) GTS_OBJECT_CAST(obj,\
						 GtsVertex,\
						 pygts_parent_vertex_class())

#define PYGTS_IS_PARENT_VERTEX(obj)(gts_object_is_from_class (obj,\
                                             pygts_parent_vertex_class ()))


/*-------------------------------------------------------------------------*/
/* Methods exported to python */

static PyObject*
is_ok(PygtsVertex *self, PyObject *args, PyObject *kwds)
{
  if(pygts_vertex_is_ok(self)) {
    Py_INCREF(Py_True);
    return Py_True;
  }
  else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}


static PyObject*
is_unattached(PygtsVertex *self, PyObject *args, PyObject *kwds)
{
  guint n;

#if PYGTS_DEBUG
  if(!pygts_vertex_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Check for attachments other than to the gtsobj_parent */
  n = g_slist_length(GTS_VERTEX(PYGTS_OBJECT(self)->gtsobj)->segments);
  if( n > 1 ) {
    Py_INCREF(Py_False);
    return Py_False;
  }
  else {
    Py_INCREF(Py_True);
    return Py_True;
  }
}


static PyObject*
is_boundary(PygtsVertex* self, PyObject *args)
{
  PyObject *s_;
  PygtsObject *s;

#if PYGTS_DEBUG
  if(!pygts_vertex_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */
  if(! PyArg_ParseTuple(args, "O", &s_) ) {
    return NULL;
  }

  /* Convert to PygtsObjects */
  if(!pygts_surface_check(s_)) {
    PyErr_SetString(PyExc_TypeError,"s is not a Surface");
    return NULL;
  }
  s = PYGTS_OBJECT(s_);

  if( gts_vertex_is_boundary(GTS_VERTEX(PYGTS_OBJECT(self)->gtsobj),
			     GTS_SURFACE(s->gtsobj)) ) {
    Py_INCREF(Py_True);
    return Py_True;
  }
  else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}


static PyObject*
contacts(PygtsVertex* self, PyObject *args)
{
  PyObject *sever_=NULL;
  gboolean sever=FALSE;
  guint n;

#if PYGTS_DEBUG
  if(!pygts_vertex_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */
  if(! PyArg_ParseTuple(args, "|O", &sever_) ) {
    return NULL;
  }

  /* Convert to PygtsObjects */
  if( sever_ != NULL ) {
    if(!PyBool_Check(sever_)) {
      PyErr_SetString(PyExc_TypeError,"sever is not a Boolean");
      return NULL;
    }
    if( sever_ == Py_True ) {
      sever = TRUE;
    }
  }

  n = gts_vertex_is_contact(GTS_VERTEX(PYGTS_OBJECT(self)->gtsobj),sever);
  return Py_BuildValue("i",n);
}


static PyObject*
is_connected(PygtsVertex *self, PyObject *args, PyObject *kwds)
{
  PyObject *v_;
  PygtsVertex *v;

#if PYGTS_DEBUG
  if(!pygts_vertex_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &v_) ) {
    return NULL;
  }

  /* Convert to PygtsObjects */
  if(!pygts_vertex_check(v_)) {
    PyErr_SetString(PyExc_TypeError,"expected a Vertex");
    return NULL;
  }
  v = PYGTS_VERTEX(v_);

  if(gts_vertices_are_connected(GTS_VERTEX(PYGTS_OBJECT(self)->gtsobj),
				GTS_VERTEX(PYGTS_OBJECT(v)->gtsobj))!=NULL) {
    Py_INCREF(Py_True);
    return Py_True;
  }
  else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}


static PyObject*
replace(PygtsVertex *self, PyObject *args, PyObject *kwds)
{
  PyObject *p2_;
  PygtsVertex *p2;
  GSList *parents=NULL, *i, *cur;

#if PYGTS_DEBUG
  if(!pygts_vertex_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &p2_) ) {
    return NULL;
  }

  /* Convert to PygtsObjects */
  if(!pygts_vertex_check(p2_)) {
    PyErr_SetString(PyExc_TypeError,"expected a Vertex");
    return NULL;
  }
  p2 = PYGTS_VERTEX(p2_);

  if(PYGTS_OBJECT(self)->gtsobj!=PYGTS_OBJECT(p2)->gtsobj) {
    /* (Ignore self-replacement) */

    /* Detach and save any parent segments */
    i = GTS_VERTEX(PYGTS_OBJECT(self)->gtsobj)->segments;
    while(i!=NULL) {
      cur = i;
      i = i->next;
      if(PYGTS_IS_PARENT_SEGMENT(cur->data)) {
	GTS_VERTEX(PYGTS_OBJECT(self)->gtsobj)->segments = 
	  g_slist_remove_link(GTS_VERTEX(PYGTS_OBJECT(self)->gtsobj)->segments,
			      cur);
	parents = g_slist_prepend(parents,cur->data);
	g_slist_free_1(cur);
      }
    }

    /* Perform the replace operation */
    gts_vertex_replace(GTS_VERTEX(PYGTS_OBJECT(self)->gtsobj),
		       GTS_VERTEX(PYGTS_OBJECT(p2)->gtsobj));

    /* Reattach the parent segments */
    i = parents;
    while(i!=NULL) {
      GTS_VERTEX(PYGTS_OBJECT(self)->gtsobj)->segments = 
	g_slist_prepend(GTS_VERTEX(PYGTS_OBJECT(self)->gtsobj)->segments,
			i->data);
      i = i->next;
    }
    g_slist_free(parents);
  }

#if PYGTS_DEBUG
  if(!pygts_vertex_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject*
neighbors(PygtsVertex* self, PyObject *args)
{
  PyObject *s_=NULL;
  GtsSurface *s=NULL;
  GSList *vertices,*v;
  PygtsVertex *vertex;
  PygtsSegment *parent;
  PyObject *tuple;
  guint n,N;

#if PYGTS_DEBUG
  if(!pygts_vertex_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */
  if(! PyArg_ParseTuple(args, "|O", &s_) ) {
    return NULL;
  }

  /* Convert */
  if( s_ != NULL ) {
    if(!pygts_surface_check(s_)) {
      PyErr_SetString(PyExc_TypeError,"s is not a Surface");
      return NULL;
    }
    s = GTS_SURFACE(PYGTS_OBJECT(s_)->gtsobj);
  }

  /* Get the neighbors */
  vertices = gts_vertex_neighbors(GTS_VERTEX(PYGTS_OBJECT(self)->gtsobj),
				  NULL,s);
  N = g_slist_length(vertices);

  /* Create the tuple */
  if( (tuple=PyTuple_New(N)) == NULL) {
    PyErr_SetString(PyExc_TypeError,"Could not create tuple");
    return NULL;
  }

  /* Put PygtsVertex objects into the tuple */
  v = vertices;
  for(n=0;n<N;n++) {

    /* Skip this vertex if it is a parent */
    while( v!=NULL && PYGTS_IS_PARENT_VERTEX(GTS_VERTEX(v->data)) ) {
      v = g_slist_next(v);      
    }
    if( v==NULL ) break;

    /* Get object from table (if available); otherwise chain-up allocation
     * and attach parent.
     */
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
	Py_DECREF((PyObject*)tuple);
	return NULL;
      }

      PYGTS_OBJECT(vertex)->gtsobj = GTS_OBJECT(v->data);

      /* Create the parent GtsSegment */
      if( (parent=PYGTS_SEGMENT(pygts_vertex_parent(
		      GTS_VERTEX(PYGTS_OBJECT(vertex)->gtsobj)))) == NULL ) {
	Py_DECREF(vertex);
	Py_DECREF(tuple);
	return NULL;
      }
      PYGTS_OBJECT(vertex)->gtsobj_parent = GTS_OBJECT(parent);
    }      
    /* Add Vertex to the tuple */
    PyTuple_SET_ITEM(tuple, n, (PyObject*)vertex);
    pygts_object_register(PYGTS_OBJECT(vertex));
    
    v = g_slist_next(v);
  }

  if(_PyTuple_Resize(&tuple,n)!=0) {
    Py_DECREF(tuple);
    return NULL;
  }

  return tuple;
}


static PyObject*
faces(PygtsVertex* self, PyObject *args)
{
  PyObject *s_=NULL;
  GtsSurface *s=NULL;
  GSList *faces,*f;
  PygtsFace *face;
  GtsSurface *parent;
  PyObject *tuple;
  guint n,N;

#if PYGTS_DEBUG
  if(!pygts_vertex_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */
  if(! PyArg_ParseTuple(args, "|O", &s_) ) {
    return NULL;
  }

  /* Convert */
  if( s_ != NULL ) {
    if(!pygts_surface_check(s_)) {
      PyErr_SetString(PyExc_TypeError,"s is not a Surface");
      return NULL;
    }
    s = GTS_SURFACE(PYGTS_OBJECT(s_)->gtsobj);
  }

  /* Get the faces */
  faces = gts_vertex_faces(GTS_VERTEX(PYGTS_OBJECT(self)->gtsobj),s,NULL);
  N = g_slist_length(faces);

  /* Create the tuple */
  if( (tuple=PyTuple_New(N)) == NULL) {
    PyErr_SetString(PyExc_TypeError,"Could not create tuple");
    return NULL;
  }

  /* Put PygtsVertex objects into the tuple */
  f = faces;
  for(n=0;n<N;n++) {

    /* Get object from table (if available); otherwise chain-up allocation
     * and attach parent.
     */
    if( (face = PYGTS_VERTEX(g_hash_table_lookup(obj_table,
						 GTS_OBJECT(f->data))
			     )) !=NULL ) {
      Py_INCREF(face);
    }
    else {
      /* Chain up object allocation */
      args = Py_BuildValue("OOOi",Py_None,Py_None,Py_None,FALSE);
      face = PYGTS_FACE(PygtsFaceType.tp_new(&PygtsFaceType, 
					     args, NULL));
      Py_DECREF(args);
      if( face == NULL ) {
	PyErr_SetString(PyExc_TypeError,"Could not create Face");
	Py_DECREF((PyObject*)tuple);
	return NULL;
      }

      PYGTS_OBJECT(face)->gtsobj = GTS_OBJECT(f->data);

      /* Create the parent */
      if( (parent=pygts_face_parent(
		      GTS_FACE(PYGTS_OBJECT(face)->gtsobj))) == NULL ) {
	Py_DECREF(face);
	return NULL;
      }
      PYGTS_OBJECT(face)->gtsobj_parent = GTS_OBJECT(parent);
    }      
    /* Add Vertex to the tuple */
    PyTuple_SET_ITEM(tuple, n, (PyObject*)face);
    pygts_object_register(PYGTS_OBJECT(face));
    
    f = g_slist_next(f);
  }

  return tuple;
}


static PyObject*
encroaches(PygtsVertex *self, PyObject *args, PyObject *kwds)
{
  PyObject *e_;
  PygtsEdge *e;

#if PYGTS_DEBUG
  if(!pygts_vertex_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &e_) ) {
    return NULL;
  }

  /* Convert to PygtsObjects */
  if(!pygts_edge_check(e_)) {
    PyErr_SetString(PyExc_TypeError,"expected an Edge");
    return NULL;
  }
  e = PYGTS_VERTEX(e_);

  if(gts_vertex_encroaches_edge(GTS_VERTEX(PYGTS_OBJECT(self)->gtsobj),
				GTS_EDGE(PYGTS_OBJECT(e)->gtsobj))) {
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
   "True if this Vertex v is OK.  False otherwise.\n"
   "This method is useful for unit testing and debugging.\n"
   "\n"
   "Signature: v.is_ok().\n"
  },  

  {"is_unattached", (PyCFunction)is_unattached,
   METH_NOARGS,
   "True if this Vertex v is not the endpoint of any Segment.\n"
   "\n"
   "Signature: v.is_unattached().\n"
  },

  {"is_boundary", (PyCFunction)is_boundary,
   METH_VARARGS,
   "True if this Vertex v is used by a boundary Edge of Surface s.\n"
   "\n"
   "Signature: v.is_boundary().\n"
  },

  {"contacts", (PyCFunction)contacts,
   METH_VARARGS,
   "Returns the number of sets of connected Triangles sharing this\n"
   "Vertex v.\n"
   "\n"
   "Signature: v.contacts().\n"
   "\n"
   "If sever is True (default: False) and v is a contact vertex then\n"
   "the vertex is replaced in each Triangle with clones.\n"
  },

  {"is_connected", (PyCFunction)is_connected,
   METH_VARARGS,
   "Return True if this Vertex v1 is connected to Vertex v2\n"
   "by a Segment.\n"
   "\n"
   "Signature: v1.is_connected().\n"
  },

  {"replace", (PyCFunction)replace,
   METH_VARARGS,
   "Replaces this Vertex v1 with Vertex v2 in all segments that have v1.\n"
   "Vertex v1 itself is left unchanged.\n"
   "\n"
   "Signature: v1.replace(v2).\n"
  },

  {"neighbors", (PyCFunction)neighbors,
   METH_VARARGS,
   "Returns a tuple of Vertices attached to this Vertex v\n"
   "by a Segment.\n"
   "\n"
   "If a Surface s is given, only Vertices on s are considered.\n"
   "\n"
   "Signature: v.neighbors() or v.neighbors(s).\n"
  },

  {"faces", (PyCFunction)faces,
   METH_VARARGS,
   "Returns a tuple of Faces that have this Vertex v.\n"
   "\n"
   "If a Surface s is given, only Vertices on s are considered.\n"
   "\n"
   "Signature: v.faces() or v.faces(s).\n"
  },

  {"encroaches", (PyCFunction)encroaches,
   METH_VARARGS,
   "Returns True if this Vertex v is strictly contained in the\n"
   "diametral circle of Edge e.  False otherwise.\n"
   "\n"
   "Only the projection onto the x-y plane is considered.\n"
   "\n"
   "Signature: v.encroaches(e)\n"
  },

  {NULL}  /* Sentinel */
};


/*-------------------------------------------------------------------------*/
/* Python type methods */

static PyObject *
new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  PygtsObject *obj;
  GtsSegment *parent;
  gdouble x=0,y=0,z=0;
  guint alloc_gtsobj = TRUE;
  static char *kwlist[] = {"x", "y", "z", "alloc_gtsobj", NULL};


  /* Parse the args */
  if(args != NULL) {
    if(! PyArg_ParseTupleAndKeywords(args, kwds, "|dddi", kwlist, &x,&y,&z,
				     &alloc_gtsobj)) {
      return NULL;
    }
  }

  /* Chain up */
  args = Py_BuildValue("dddi",x,y,z,FALSE);
  obj = PYGTS_OBJECT(PygtsPointType.tp_new(type,args,NULL));
  Py_DECREF(args);

  /* Allocate the gtsobj (if needed) */
  if( alloc_gtsobj ) {
    obj->gtsobj = GTS_OBJECT(gts_vertex_new(gts_vertex_class(),0,0,0));
    if( obj->gtsobj == NULL )  {
      PyErr_SetString(PyExc_RuntimeError, "GTS could not create Vertex");
      return NULL;
    }

    /* Create the parent GtsSegment */
    if( (parent=pygts_vertex_parent(GTS_VERTEX(obj->gtsobj))) == NULL ) {
      gts_object_destroy(obj->gtsobj);
      obj->gtsobj = NULL;
      return NULL;
    }
    obj->gtsobj_parent = GTS_OBJECT(parent);

    pygts_object_register(obj);
  }

  return (PyObject*)obj;
}


static int
init(PygtsVertex *self, PyObject *args, PyObject *kwds)
{
  gint ret;

  /* Chain up */
  if( (ret=PygtsPointType.tp_init((PyObject*)self,args,kwds)) != 0 ) {
    return ret;
  }

#if PYGTS_DEBUG
  if(!pygts_vertex_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return -1;
  }
#endif

  return 0;
}


/* Methods table */
PyTypeObject PygtsVertexType = {
    PyObject_HEAD_INIT(NULL)
    0,                       /* ob_size */
    "gts.Vertex",            /* tp_name */
    sizeof(PygtsVertex),     /* tp_basicsize */
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
    "Vertex object",         /* tp_doc */
    0,                       /* tp_traverse */
    0,                       /* tp_clear */
    0,                       /* tp_richcompare */
    0,                       /* tp_weaklistoffset */
    0,                       /* tp_iter */
    0,                       /* tp_iternext */
    methods,                 /* tp_methods */
    0,                       /* tp_members */
    0,                       /* tp_getset */
    0,                       /* tp_base: attached in pygts.c */
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
pygts_vertex_check(PyObject* o)
{
  if(! PyObject_TypeCheck(o, &PygtsVertexType)) {
    return FALSE;
  }
  else {

#if PYGTS_DEBUG
    return pygts_vertex_is_ok(PYGTS_VERTEX(o));
#else
    return TRUE;
#endif
  }
}


gboolean 
pygts_vertex_is_ok(PygtsVertex *v)
{
  GSList *parent;
  PygtsObject *obj;

  obj = PYGTS_OBJECT(v);

  if(!pygts_point_is_ok(PYGTS_POINT(v))) return FALSE;

  /* Check for a valid parent */
  g_return_val_if_fail(obj->gtsobj_parent!=NULL,FALSE);
  parent = g_slist_find(GTS_VERTEX(obj->gtsobj)->segments,
			obj->gtsobj_parent);
  g_return_val_if_fail(parent!=NULL,FALSE);

  return TRUE;
}


GtsSegment*
pygts_vertex_parent(GtsVertex *v1) {
  GtsPoint *p1;
  GtsVertex *v2;
  GtsSegment *parent;

  /* Create another Vertex */
  p1 = GTS_POINT(v1);
  if( (v2 = gts_vertex_new(pygts_parent_vertex_class(),p1->x,p1->y,p1->z+1)) 
    == NULL ) {
    PyErr_SetString(PyExc_RuntimeError, "GTS could not create Vertex");
    return NULL;
  }

  /* Create and return the parent */
  if( (parent = gts_segment_new(pygts_parent_segment_class(),v1,v2))
      == NULL ) {
    PyErr_SetString(PyExc_RuntimeError, "GTS could not create Segment");
    gts_object_destroy(GTS_OBJECT(v2));
    return NULL;
  }

  return parent;
}


static GtsSegmentClass *pygts_parent_segment_class(void)
{
  static GtsSegmentClass *klass = NULL;
  GtsObjectClass *super = NULL;

  if (klass == NULL) {

    super = GTS_OBJECT_CLASS(gts_segment_class());

    GtsObjectClassInfo pygts_parent_segment_info = {
      "PygtsParentSegment",
      sizeof(PygtsParentSegment),
      sizeof(GtsSegmentClass),
      (GtsObjectClassInitFunc)(super->info.class_init_func),
      (GtsObjectInitFunc)(super->info.object_init_func),
      (GtsArgSetFunc) NULL,
      (GtsArgGetFunc) NULL
    };
    klass = gts_object_class_new(gts_object_class(),
				 &pygts_parent_segment_info);
  }

  return klass;
}


GtsVertexClass *pygts_parent_vertex_class(void)
{
  static GtsVertexClass *klass = NULL;
  GtsObjectClass *super = NULL;

  if (klass == NULL) {

    super = GTS_OBJECT_CLASS(gts_vertex_class());

    GtsObjectClassInfo pygts_parent_vertex_info = {
      "PygtsParentVertex",
      sizeof(PygtsParentVertex),
      sizeof(GtsVertexClass),
      (GtsObjectClassInitFunc)(super->info.class_init_func),
      (GtsObjectInitFunc)(super->info.object_init_func),
      (GtsArgSetFunc) NULL,
      (GtsArgGetFunc) NULL
    };
    klass = gts_object_class_new(gts_object_class(),
				 &pygts_parent_vertex_info);
  }

  return klass;
}
