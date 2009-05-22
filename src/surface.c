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
is_ok(PygtsSurface *self, PyObject *args)
{
  if(pygts_surface_is_ok(self)) {
    Py_INCREF(Py_True);
    return Py_True;
  }
  else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}


static PyObject*
add(PygtsSurface *self, PyObject *args)
{
  PyObject *f_;
  PygtsFace *f;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &f_) )
    return NULL;

  /* Convert to PygtsObjects */
  if(!pygts_face_check(f_)) {
    PyErr_SetString(PyExc_TypeError,"expected a Face");
    return NULL;
  }
  f = PYGTS_FACE(f_);

  gts_surface_add_face(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),
		       GTS_FACE(PYGTS_OBJECT(f)->gtsobj));

  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject*
pygts_remove(PygtsSurface *self, PyObject *args)
{
  PyObject *f_;
  PygtsFace *f;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &f_) )
    return NULL;

  /* Convert to PygtsObjects */
  if(!pygts_face_check(f_)) {
    PyErr_SetString(PyExc_TypeError,"expected a Face");
    return NULL;
  }
  f = PYGTS_FACE(f_);

  gts_surface_remove_face(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),
			  GTS_FACE(PYGTS_OBJECT(f)->gtsobj));

  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject*
copy(PygtsSurface *self, PyObject *args)
{
  PyObject *s_;
  PygtsSurface *s;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &s_) )
    return NULL;

  /* Convert to PygtsObjects */
  if(!pygts_surface_check(s_)) {
    PyErr_SetString(PyExc_TypeError,"expected a Surface");
    return NULL;
  }
  s = PYGTS_SURFACE(s_);

  gts_surface_copy(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),
		   GTS_SURFACE(PYGTS_OBJECT(s)->gtsobj));

  Py_INCREF((PyObject*)self);
  return (PyObject*)self;
}


static PyObject*
merge(PygtsSurface *self, PyObject *args)
{
  PyObject *s_;
  PygtsSurface *s;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &s_) )
    return NULL;

  /* Convert to PygtsObjects */
  if(!pygts_surface_check(s_)) {
    PyErr_SetString(PyExc_TypeError,"expected a Surface");
    return NULL;
  }
  s = PYGTS_SURFACE(s_);

  gts_surface_merge(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),
		    GTS_SURFACE(PYGTS_OBJECT(s)->gtsobj));

  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject*
pygts_read(PygtsSurface *self, PyObject *args)
{
  PyObject *f_;
  FILE *f;
  GtsFile *fp;
  guint lineno;
  GtsSurface *m;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &f_) )
    return NULL;

  /* Convert to PygtsObjects */
  if(!PyFile_Check(f_)) {
    PyErr_SetString(PyExc_TypeError,"expected a File");
    return NULL;
  }
  f = PyFile_AsFile(f_);

  if(feof(f)) {
    PyErr_SetString(PyExc_EOFError,"End of File");
    return NULL;
  }

  /* Create a temporary surface to read into */
  if( (m = gts_surface_new (gts_surface_class(),
			    gts_face_class(),
			    gts_edge_class(),
	   GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj)->vertex_class)) == NULL ) {
    PyErr_SetString(PyExc_TypeError,"Could not create Surface");
    return NULL;
  }

  /* Read from the file */
  fp = gts_file_new(f);
  if( (lineno = gts_surface_read(m,fp)) != 0 ) {
    PyErr_SetString(PyExc_RuntimeError,fp->error);
    gts_file_destroy(fp);
    return NULL;
  }
  gts_file_destroy(fp);

  /* Clean up the surface */
  gts_surface_merge(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),m);
  gts_object_destroy(GTS_OBJECT(m));
  pygts_edge_cleanup(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj));
  pygts_triangle_cleanup(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj));

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject*
is_manifold(PygtsSurface *self, PyObject *args)
{
#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  if(gts_surface_is_manifold(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj))) {
    Py_INCREF(Py_True);
    return Py_True;
  }
  else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}


static PyObject*
is_orientable(PygtsSurface *self, PyObject *args)
{
#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  if(gts_surface_is_orientable(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj))) {
    Py_INCREF(Py_True);
    return Py_True;
  }
  else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}


static PyObject*
is_closed(PygtsSurface *self, PyObject *args)
{
#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  if(gts_surface_is_closed(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj))) {
    Py_INCREF(Py_True);
    return Py_True;
  }
  else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}


static PyObject*
area(PygtsSurface *self, PyObject *args)
{
  GtsSurface *s;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  s = GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj);
  return Py_BuildValue("d",gts_surface_area(s));
}


static PyObject*
volume(PygtsSurface *self, PyObject *args)
{
  GtsSurface *s;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  s = GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj);

  if(!gts_surface_is_closed(s)) {
    PyErr_SetString(PyExc_RuntimeError,"Surface is not closed");
    return NULL;
  }

  if(!gts_surface_is_orientable(s)) {
    PyErr_SetString(PyExc_RuntimeError,"Surface is not orientable");
    return NULL;
  }

  return Py_BuildValue("d",gts_surface_volume(s));
}


static PyObject*
center_of_mass(PygtsSurface *self, PyObject *args)
{
  GtsSurface *s;
  GtsVector cm;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  s = GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj);
  gts_surface_center_of_mass(s,cm);
  return Py_BuildValue("ddd",cm[0],cm[1],cm[2]);
}


static PyObject*
center_of_area(PygtsSurface *self, PyObject *args)
{
  GtsSurface *s;
  GtsVector cm;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  s = GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj);
  gts_surface_center_of_area(s,cm);
  return Py_BuildValue("ddd",cm[0],cm[1],cm[2]);
}


static PyObject*
pygts_write(PygtsSurface *self, PyObject *args)
{
  PyObject *f_;
  FILE *f;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &f_) )
    return NULL;

  /* Convert to PygtsObjects */
  if(!PyFile_Check(f_)) {
    PyErr_SetString(PyExc_TypeError,"expected a File");
    return NULL;
  }
  f = PyFile_AsFile(f_);

  /* Write to the file */
  gts_surface_write(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),f);

  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject*
split(PygtsSurface *self, PyObject *args)
{
  GSList *surfaces, *s;
  PyObject *tuple;
  PygtsSurface *surface;
  guint n,N;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  surfaces = gts_surface_split(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj));
  
  /* Create a tuple to put the Surfaces into */
  N = g_slist_length(surfaces);
  if( (tuple=PyTuple_New(N)) == NULL) {
    PyErr_SetString(PyExc_TypeError,"Could not create tuple");
    return NULL;
  }

  /* Put PygtsSurface objects into the tuple */
  s = surfaces;
  for(n=0;n<N;n++) {
    /* Chain up object allocation */
    args = Py_BuildValue("(i)",FALSE);
    surface = PYGTS_SURFACE(PygtsSurfaceType.tp_new(&PygtsSurfaceType, 
						    args, NULL));
    Py_DECREF(args);
    if( surface == NULL ) {
      Py_DECREF((PyObject*)tuple);
      PyErr_SetString(PyExc_RuntimeError, "Could not create Surface");
      return NULL;
    }

    /* Initialization */
    PYGTS_OBJECT(surface)->gtsobj = GTS_OBJECT(s->data);
    surface->traverse = NULL;

    /* Add Surface to the tuple */
    PyTuple_SET_ITEM(tuple, n, (PyObject*)surface);
    pygts_object_register(PYGTS_OBJECT(surface));

    s = g_slist_next(s);
  }

  return tuple;
}


/* Helper function for vertices() */
static void get_vertex(GtsVertex *vertex, GtsVertex ***v)
{
  **v = vertex;
  *v += 1;
}

static PyObject*
vertices(PygtsSurface *self, PyObject *args)
{
  PyObject *tuple;
  PygtsVertex *vertex;
  PygtsVertex **vertices,**v;
  GtsSegment *parent;
  PygtsObject *obj;
  guint i,N=0;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Get the number of vertices */
  N = gts_surface_vertex_number(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj));

  /* Retrieve all of the vertex pointers into a temporary array */
  if( (vertices = (PygtsVertex**)malloc(N*sizeof(PygtsVertex*))) == NULL ) {
    PyErr_SetString(PyExc_TypeError,"Could not create array");
    return NULL;
  }
  v = vertices;

  gts_surface_foreach_vertex(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),
			     (GtsFunc)get_vertex,&v);

  /* Create a tuple to put the vertices into */
  if( (tuple=PyTuple_New(N)) == NULL) {
    PyErr_SetString(PyExc_TypeError,"Could not create tuple");
    return NULL;
  }

  /* Put PygtsVertex objects into the tuple */
  v = vertices;
  for(i=0;i<N;i++) {

    /* Get object from table (if available) */
    if( (vertex = g_hash_table_lookup(obj_table,*v)) != NULL ) {
      Py_INCREF(vertex);
    }
    else {
      /* Chain up object allocation */
      args = Py_BuildValue("dddi",0,0,0,FALSE);
      vertex = PYGTS_VERTEX(PygtsVertexType.tp_new(&PygtsVertexType,
						   args, NULL));
      Py_DECREF(args);
      if( vertex == NULL ) {
	Py_DECREF((PyObject*)tuple);
	PyErr_SetString(PyExc_RuntimeError, "Could not create Vertex");
	return NULL;
      }

      /* Initialization */
      obj = PYGTS_OBJECT(vertex);
      obj->gtsobj = GTS_OBJECT(*v);
      if( (parent=pygts_vertex_parent(GTS_VERTEX(obj->gtsobj))) == NULL ) {
	Py_DECREF((PyObject*)tuple);
	Py_DECREF((PyObject*)vertex);
	PyErr_SetString(PyExc_RuntimeError, "Could not create Segment");
	return NULL;
      }
      obj->gtsobj_parent = GTS_OBJECT(parent);
      pygts_object_register(PYGTS_OBJECT(vertex));
    }
    PyTuple_SET_ITEM(tuple, i, (PyObject*)vertex);    
    v += 1;
  }

  free(vertices);
  return tuple;
}


/* Helper for face_indices() */
typedef struct {
  PyObject *vertices,*indices;  /* Vertex and indices tuples */
  guint Nv,Ni;                  /* Number of vertices and indices */
  guint n;                      /* Current face index */
  gboolean errflag;
} IndicesData;

/* Helper for face_indices() */
static void get_indices(GtsFace *face, IndicesData *data)
{
  PyObject *t;
  GtsVertex *v[3];
  guint i,j;
  gboolean flag;

  if(data->errflag) return;

  /* Put the vertex pointers in an array */
  gts_triangle_vertices( GTS_TRIANGLE(face), &(v[0]), &(v[1]), &(v[2]) );

  /* Create a tuple to put the indices into */
  if( (t=PyTuple_New(3)) == NULL) {
    PyErr_SetString(PyExc_TypeError,"Could not create tuple");
    data->errflag = TRUE;
    return;
  }
  PyTuple_SET_ITEM(data->indices, data->n, t);

  /* Determine the indices */
  for(i=0;i<3;i++) {
    flag = FALSE;
    for(j=0;j<data->Nv;j++) {
      if( GTS_VERTEX(PYGTS_OBJECT(PyTuple_GET_ITEM(data->vertices,j))->gtsobj)
	  ==v[i] ) {
	PyTuple_SET_ITEM(t, i, PyInt_FromLong(j));
	flag = TRUE;
	break;
      }
    }
    if(!flag) {
      PyErr_SetString(PyExc_TypeError,
		      "Could not initialize tuple (internal error)");
      data->errflag = TRUE;
      return;
    }
  }
  data->n += 1;
}


static PyObject*
face_indices(PygtsSurface *self, PyObject *args)
{
  PyObject *vertices,*indices;
  IndicesData data;
  guint Nv,Nf;
  guint i;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &vertices) )
    return NULL;

  /* Make sure that the tuple contains only vertices */
  Nv = PyTuple_Size(vertices);
  for(i=0;i<Nv;i++) {
    if(!pygts_vertex_check(PyTuple_GetItem(vertices,i))){
      PyErr_SetString(PyExc_TypeError,"Tuple has objects other than Vertices");
      return NULL;
    }
  }

  /* Get the number of faces in this surface */
  Nf = gts_surface_face_number(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj));

  /* Create a tuple to put the index tuples into */
  if( (indices=PyTuple_New(Nf)) == NULL) {
    PyErr_SetString(PyExc_TypeError,"Could not create tuple");
    return NULL;
  }

  /* Initialize the IndicesData struct.  This is used to maintain state as each 
   * face is processed.
   */
  data.vertices = vertices;
  data.indices = indices;
  data.Nv = Nv;
  data.Ni = Nf;
  data.n = 0;
  data.errflag = FALSE;

  /* Process each face */
  gts_surface_foreach_face(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),
			   (GtsFunc)get_indices,&data);
  if(data.errflag) {
    Py_DECREF(data.indices);
    return NULL;
  }

  return indices;
}


static PyObject*
strip(PygtsSurface *self, PyObject *args)
{
  GSList *strips, *s, *f;
  PyObject *tuple, **tuples;
  guint i,j,n,N;
  PygtsFace *face=NULL;
  GtsSurface *parent;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  strips = gts_surface_strip(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj));
  
  /* Create tuples to put the Faces into */
  N = g_slist_length(strips);

  if( (tuple=PyTuple_New(N)) == NULL) {
    PyErr_SetString(PyExc_TypeError,"Could not create tuple");
    return NULL;
  }
  if( (tuples = (PyObject**)malloc(N*sizeof(PyObject*))) == NULL ) {
    PyErr_SetString(PyExc_TypeError,"Could not create array");
    Py_DECREF(tuple);
    return NULL;
  }
  s = strips;
  for(i=0;i<N;i++) {
    f = (GSList*)(s->data);
    n = g_slist_length(f);
    if( (tuples[i]=PyTuple_New(n)) == NULL) {
      PyErr_SetString(PyExc_TypeError,"Could not create tuple");
      Py_DECREF(tuple);
      free(tuples);
      return NULL;
    }
    PyTuple_SET_ITEM(tuple, i, tuples[i]);
    s = g_slist_next(s);
  }

  /* Put PygtsFace objects into the tuple */
  s = strips;
  for(i=0;i<N;i++) {
    f = (GSList*)(s->data);
    n = g_slist_length(f);
    for(j=0;j<n;j++) {
      if( (face=PYGTS_FACE(g_hash_table_lookup(obj_table,face))) == NULL ) {

	/* Chain up object allocation */
	args = Py_BuildValue("OOOi",Py_None,Py_None,Py_None,FALSE);
	face = PYGTS_FACE(PygtsFaceType.tp_new(&PygtsFaceType, args, NULL));
	Py_DECREF(args);
	if( face == NULL ) {
	  Py_DECREF(tuple);
	  free(tuples);
	  PyErr_SetString(PyExc_RuntimeError, "Could not create Face");
	  return NULL;
	}

	/* Initialization */
	if( (parent = pygts_face_parent(GTS_FACE(f->data))) == NULL ) {
	  Py_DECREF(tuple);
	  Py_DECREF(face);
	  free(tuples);
	  return NULL;
	}

	PYGTS_OBJECT(face)->gtsobj = GTS_OBJECT(f->data);
	PYGTS_OBJECT(face)->gtsobj_parent = GTS_OBJECT(parent);
      }
      else {
	Py_INCREF(face);
      }

      /* Add Face to the tuple */
      PyTuple_SET_ITEM(tuples[i], j, (PyObject*)face);
      pygts_object_register(PYGTS_OBJECT(face));

      f = g_slist_next(f);
    }
    s = g_slist_next(s);
  }

  free(tuples);
  return tuple;
}


static PyObject*
stats(PygtsSurface *self, PyObject *args)
{
  GtsSurfaceStats stats;
  PyObject *dict, *edges_per_vertex, *faces_per_edge;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Make the call */
  gts_surface_stats(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),&stats);

  /* Create the dictionaries */
  if( (dict = PyDict_New()) == NULL ) {
    PyErr_SetString(PyExc_RuntimeError,"cannot create dict");
    return NULL;
  }
  if( (edges_per_vertex = PyDict_New()) == NULL ) {
    PyErr_SetString(PyExc_RuntimeError,"cannot create dict");
    Py_DECREF(dict);
    return NULL;
  }
  if( (faces_per_edge = PyDict_New()) == NULL ) {
    PyErr_SetString(PyExc_RuntimeError,"cannot create dict");
    Py_DECREF(dict);
    Py_DECREF(edges_per_vertex);
    return NULL;
  }

  /* Populate the edges_per_vertex dict */
  PyDict_SetItemString(edges_per_vertex,"min", 
		       Py_BuildValue("d",stats.edges_per_vertex.min));
  PyDict_SetItemString(edges_per_vertex,"max", 
		       Py_BuildValue("d",stats.edges_per_vertex.max));
  PyDict_SetItemString(edges_per_vertex,"sum", 
		       Py_BuildValue("d",stats.edges_per_vertex.sum));
  PyDict_SetItemString(edges_per_vertex,"sum2", 
		       Py_BuildValue("d",stats.edges_per_vertex.sum2));
  PyDict_SetItemString(edges_per_vertex,"mean", 
		       Py_BuildValue("d",stats.edges_per_vertex.mean));
  PyDict_SetItemString(edges_per_vertex,"stddev", 
		       Py_BuildValue("d",stats.edges_per_vertex.stddev));
  PyDict_SetItemString(edges_per_vertex,"n", 
		       Py_BuildValue("i",stats.edges_per_vertex.n));

  /* Populate the faces_per_edge dict */
  PyDict_SetItemString(faces_per_edge,"min", 
		       Py_BuildValue("d",stats.faces_per_edge.min));
  PyDict_SetItemString(faces_per_edge,"max", 
		       Py_BuildValue("d",stats.faces_per_edge.max));
  PyDict_SetItemString(faces_per_edge,"sum", 
		       Py_BuildValue("d",stats.faces_per_edge.sum));
  PyDict_SetItemString(faces_per_edge,"sum2", 
		       Py_BuildValue("d",stats.faces_per_edge.sum2));
  PyDict_SetItemString(faces_per_edge,"mean", 
		       Py_BuildValue("d",stats.faces_per_edge.mean));
  PyDict_SetItemString(faces_per_edge,"stddev", 
		       Py_BuildValue("d",stats.faces_per_edge.stddev));
  PyDict_SetItemString(faces_per_edge,"n", 
		       Py_BuildValue("i",stats.faces_per_edge.n));

  /* Populate the main dict */
  PyDict_SetItemString(dict,"n_faces", Py_BuildValue("i",stats.n_faces));
  PyDict_SetItemString(dict,"n_incompatible_faces", 
		       Py_BuildValue("i",stats.n_incompatible_faces));
  PyDict_SetItemString(dict,"n_boundary_edges", 
		       Py_BuildValue("i",stats.n_boundary_edges));
  PyDict_SetItemString(dict,"n_non_manifold_edges", 
		       Py_BuildValue("i",stats.n_non_manifold_edges));
  PyDict_SetItemString(dict,"edges_per_vertex", edges_per_vertex);
  PyDict_SetItemString(dict,"faces_per_edge", faces_per_edge);

  return dict;
}


static PyObject*
quality_stats(PygtsSurface *self, PyObject *args)
{
  GtsSurfaceQualityStats stats;
  PyObject *dict, *face_quality, *face_area, *edge_length, *edge_angle;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Make the call */
  gts_surface_quality_stats(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),&stats);

  /* Create the dictionaries */
  if( (dict = PyDict_New()) == NULL ) {
    PyErr_SetString(PyExc_RuntimeError,"cannot create dict");
    return NULL;
  }
  if( (face_quality = PyDict_New()) == NULL ) {
    PyErr_SetString(PyExc_RuntimeError,"cannot create dict");
    Py_DECREF(dict);
    return NULL;
  }
  if( (face_area = PyDict_New()) == NULL ) {
    PyErr_SetString(PyExc_RuntimeError,"cannot create dict");
    Py_DECREF(dict);
    Py_DECREF(face_quality);
    return NULL;
  }
  if( (edge_length = PyDict_New()) == NULL ) {
    PyErr_SetString(PyExc_RuntimeError,"cannot create dict");
    Py_DECREF(dict);
    Py_DECREF(face_quality);
    Py_DECREF(face_area);
    return NULL;
  }
  if( (edge_angle = PyDict_New()) == NULL ) {
    PyErr_SetString(PyExc_RuntimeError,"cannot create dict");
    Py_DECREF(dict);
    Py_DECREF(face_quality);
    Py_DECREF(face_area);
    Py_DECREF(edge_length);
    return NULL;
  }

  /* Populate the face_quality dict */
  PyDict_SetItemString(face_quality,"min", 
		       Py_BuildValue("d",stats.face_quality.min));
  PyDict_SetItemString(face_quality,"max", 
		       Py_BuildValue("d",stats.face_quality.max));
  PyDict_SetItemString(face_quality,"sum", 
		       Py_BuildValue("d",stats.face_quality.sum));
  PyDict_SetItemString(face_quality,"sum2", 
		       Py_BuildValue("d",stats.face_quality.sum2));
  PyDict_SetItemString(face_quality,"mean", 
		       Py_BuildValue("d",stats.face_quality.mean));
  PyDict_SetItemString(face_quality,"stddev", 
		       Py_BuildValue("d",stats.face_quality.stddev));
  PyDict_SetItemString(face_quality,"n", 
		       Py_BuildValue("i",stats.face_quality.n));

  /* Populate the face_area dict */
  PyDict_SetItemString(face_area,"min", 
		       Py_BuildValue("d",stats.face_area.min));
  PyDict_SetItemString(face_area,"max", 
		       Py_BuildValue("d",stats.face_area.max));
  PyDict_SetItemString(face_area,"sum", 
		       Py_BuildValue("d",stats.face_area.sum));
  PyDict_SetItemString(face_area,"sum2", 
		       Py_BuildValue("d",stats.face_area.sum2));
  PyDict_SetItemString(face_area,"mean", 
		       Py_BuildValue("d",stats.face_area.mean));
  PyDict_SetItemString(face_area,"stddev", 
		       Py_BuildValue("d",stats.face_area.stddev));
  PyDict_SetItemString(face_area,"n", 
		       Py_BuildValue("i",stats.face_area.n));

  /* Populate the edge_length dict */
  PyDict_SetItemString(edge_length,"min", 
		       Py_BuildValue("d",stats.edge_length.min));
  PyDict_SetItemString(edge_length,"max", 
		       Py_BuildValue("d",stats.edge_length.max));
  PyDict_SetItemString(edge_length,"sum", 
		       Py_BuildValue("d",stats.edge_length.sum));
  PyDict_SetItemString(edge_length,"sum2", 
		       Py_BuildValue("d",stats.edge_length.sum2));
  PyDict_SetItemString(edge_length,"mean", 
		       Py_BuildValue("d",stats.edge_length.mean));
  PyDict_SetItemString(edge_length,"stddev", 
		       Py_BuildValue("d",stats.edge_length.stddev));
  PyDict_SetItemString(edge_length,"n", 
		       Py_BuildValue("i",stats.edge_length.n));

  /* Populate the edge_angle dict */
  PyDict_SetItemString(edge_angle,"min", 
		       Py_BuildValue("d",stats.edge_angle.min));
  PyDict_SetItemString(edge_angle,"max", 
		       Py_BuildValue("d",stats.edge_angle.max));
  PyDict_SetItemString(edge_angle,"sum", 
		       Py_BuildValue("d",stats.edge_angle.sum));
  PyDict_SetItemString(edge_angle,"sum2", 
		       Py_BuildValue("d",stats.edge_angle.sum2));
  PyDict_SetItemString(edge_angle,"mean", 
		       Py_BuildValue("d",stats.edge_angle.mean));
  PyDict_SetItemString(edge_angle,"stddev", 
		       Py_BuildValue("d",stats.edge_angle.stddev));
  PyDict_SetItemString(edge_angle,"n", 
		       Py_BuildValue("i",stats.edge_angle.n));

  /* Populate the main dict */
  PyDict_SetItemString(dict,"face_quality", face_quality);
  PyDict_SetItemString(dict,"face_area", face_area);
  PyDict_SetItemString(dict,"edge_length", edge_length);
  PyDict_SetItemString(dict,"edge_angle", edge_angle);

  return dict;
}


static PyObject*
tessellate(PygtsSurface *self, PyObject *args)
{
#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  gts_surface_tessellate(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),NULL,NULL);

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  Py_INCREF(Py_None);
  return Py_None;
}


/* Helper function for inter() */
void
get_largest_coord(GtsVertex *v,gdouble *val) {
  if( fabs(GTS_POINT(v)->x) > *val ) *val = fabs(GTS_POINT(v)->x);
  if( fabs(GTS_POINT(v)->y) > *val ) *val = fabs(GTS_POINT(v)->y);
  if( fabs(GTS_POINT(v)->z) > *val ) *val = fabs(GTS_POINT(v)->z);
}

/* Helper function for intersection operations */
static PyObject*
inter(PygtsSurface *self, PyObject *args, GtsBooleanOperation op1,
      GtsBooleanOperation op2)
{
  PyObject *s_;
  PygtsSurface *s;
  PygtsObject *ret;
  GtsVector cm1, cm2;
  gdouble area1, area2;
  GtsSurfaceInter *si;
  GNode *tree1, *tree2;
  gboolean is_open1, is_open2, closed;
  gdouble eps=0.;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */  
  if(! PyArg_ParseTuple(args, "O", &s_) )
    return NULL;

  /* Convert to PygtsObjects */
  if(!pygts_surface_check(s_)) {
    PyErr_SetString(PyExc_TypeError,"expected a Surface");
    return NULL;
  }
  s = PYGTS_SURFACE(s_);

  /* Make sure that we don't have two pointers to the same surface */
  if( (GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj) == 
       GTS_SURFACE(PYGTS_OBJECT(s)->gtsobj)) ) {
      PyErr_SetString(PyExc_RuntimeError,
		      "can't determine intersection with self");
      return NULL;
  }


  /* *** ATTENTION ***
   * Eliminate any active gts traverse objects.  They appear to interfere
   * with the intersection calculation.  I would guess that this is due
   * to the use of the "reserved" field (i.e., it doesn't get properly
   * reset until the traverse is destroyed).
   *
   * I don't expect this to cause problems here, but a bug report should be
   * filed.
   */
  if(self->traverse!=NULL) {
    gts_surface_traverse_destroy(self->traverse);
    self->traverse = NULL;
  }
  if(s->traverse!=NULL) {
    gts_surface_traverse_destroy(s->traverse);
    s->traverse = NULL;
  }
  /* *** ATTENTION *** */


  /* Check for self-intersections in either surface */
  if( gts_surface_is_self_intersecting(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj))
      != NULL ) {
    PyErr_SetString(PyExc_RuntimeError,"Surface is self-intersecting");
    return NULL;
  }
  if( gts_surface_is_self_intersecting(GTS_SURFACE(PYGTS_OBJECT(s)->gtsobj))
      != NULL ) {
    PyErr_SetString(PyExc_RuntimeError,"Surface is self-intersecting");
    return NULL;
  }

  /* Avoid complete self-intersection of two surfaces*/
  if( (gts_surface_face_number(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj)) ==
      gts_surface_face_number(GTS_SURFACE(PYGTS_OBJECT(s)->gtsobj))) &&
      (gts_surface_edge_number(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj)) ==
       gts_surface_edge_number(GTS_SURFACE(PYGTS_OBJECT(s)->gtsobj))) &&
      (gts_surface_vertex_number(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj)) ==
       gts_surface_vertex_number(GTS_SURFACE(PYGTS_OBJECT(s)->gtsobj))) &&
      (gts_surface_area(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj)) ==
       gts_surface_area(GTS_SURFACE(PYGTS_OBJECT(s)->gtsobj))) ) {

    area1 = \
      gts_surface_center_of_area(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),cm1);

    area2 = \
      gts_surface_center_of_area(GTS_SURFACE(PYGTS_OBJECT(s)->gtsobj),cm2);

    if( (area1==area2) && (cm1[0]==cm2[0]) && (cm1[1]==cm2[1]) && 
	(cm1[2]==cm2[2]) ) {
      PyErr_SetString(PyExc_RuntimeError,"Surfaces mutually intersect");
      return NULL;
    }
  }

  /* Get bounding boxes */
  if( (tree1=gts_bb_tree_surface(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj)))
      ==NULL ) {
    PyErr_SetString(PyExc_RuntimeError,"GTS could not create tree");
    return NULL;
  }
  is_open1 = !gts_surface_is_closed(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj));
  if( (tree2=gts_bb_tree_surface(GTS_SURFACE(PYGTS_OBJECT(s)->gtsobj)))
      ==NULL ) {
    gts_bb_tree_destroy(tree1, TRUE);
    PyErr_SetString(PyExc_RuntimeError,"GTS could not create tree");
    return NULL;
  }
  is_open2 = !gts_surface_is_closed(GTS_SURFACE(PYGTS_OBJECT(s)->gtsobj));

  /* Get the surface intersection object */
  if( (si = gts_surface_inter_new(gts_surface_inter_class(),
				  GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),
				  GTS_SURFACE(PYGTS_OBJECT(s)->gtsobj),
				  tree1, tree2, is_open1, is_open2))==NULL) {
    gts_bb_tree_destroy(tree1, TRUE);
    gts_bb_tree_destroy(tree2, TRUE);
    PyErr_SetString(PyExc_RuntimeError,"GTS could not create GtsSurfaceInter");
    return NULL;
  }

  gts_bb_tree_destroy(tree1, TRUE);
  gts_bb_tree_destroy(tree2, TRUE);
  gts_surface_inter_check(si,&closed);
  if( closed == FALSE ) {
    gts_object_destroy(GTS_OBJECT(si));
    PyErr_SetString(PyExc_RuntimeError,"result is not closed");
    return NULL;
  }

  /* Create the return surface */
  args = Py_BuildValue("(i)",FALSE);
  ret = PYGTS_OBJECT(PygtsSurfaceType.tp_new(&PygtsSurfaceType, args, NULL));
  Py_DECREF(args);
  if( ret == NULL ) {
    return NULL;
  }
  ret->gtsobj = GTS_OBJECT(gts_surface_new(gts_surface_class(),
					   gts_face_class(),
					   gts_edge_class(),
					   gts_vertex_class()));
  if( ret->gtsobj == NULL )  {
    Py_DECREF(ret);
    PyErr_SetString(PyExc_RuntimeError, "GTS could not create Surface");
    return NULL;
  }

  /* Calculate the result */
  gts_surface_inter_boolean(si, GTS_SURFACE(ret->gtsobj),op1);
  gts_surface_inter_boolean(si, GTS_SURFACE(ret->gtsobj),op2);
  gts_object_destroy(GTS_OBJECT(si));

  /* Clean up the result */
  gts_surface_foreach_vertex(GTS_SURFACE(ret->gtsobj),
			     (GtsFunc)get_largest_coord,&eps);
  eps *= pow(2.,-50);
  pygts_vertex_cleanup(GTS_SURFACE(ret->gtsobj),eps);
  pygts_edge_cleanup(GTS_SURFACE(ret->gtsobj));
  pygts_triangle_cleanup(GTS_SURFACE(ret->gtsobj));

  /* Check for self-intersection */
  if( gts_surface_is_self_intersecting(GTS_SURFACE(ret->gtsobj)) != NULL ) {
    Py_DECREF(ret);
    PyErr_SetString(PyExc_RuntimeError,"result is self-intersecting surface");
    return NULL;
  }

  pygts_object_register(PYGTS_OBJECT(ret));

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)ret)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with ret object (internal error)");
    return NULL;
  }
#endif

  return (PyObject*)ret;
}


static PyObject*
intersection(PygtsSurface *self, PyObject *args, GtsBooleanOperation op1,
	     GtsBooleanOperation op2)
{
#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  return inter(self,args,GTS_1_IN_2,GTS_2_IN_1);
}


static PyObject*
pygts_union(PygtsSurface *self, PyObject *args)
{
#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  return inter(self,args,GTS_1_OUT_2,GTS_2_OUT_1);
}


static PyObject*
difference(PygtsSurface *self, PyObject *args)
{
#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  return inter(self,args,GTS_1_OUT_2,GTS_2_IN_1);
}


/* Helper for rotate(), scale() and translate() transforms */
typedef struct {
  double dx, dy, dz, a;
  gboolean errflag;
} TransformData;

/* Helper for rotate() */
static void
rotate_point(GtsPoint *p, TransformData *data)
{
  if(data->errflag) return;

  if(pygts_point_rotate(p,data->dx,data->dy,data->dz,data->a)==-1)
    data->errflag=TRUE;
}

static PyObject*
rotate(PygtsSurface* self, PyObject *args, PyObject *keywds)
{
  TransformData data;
  static char *kwlist[] = {"dx", "dy", "dz", "a", NULL};

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  data.dx=0; data.dy=0; data.dz=0; data.a=0;
  data.errflag = FALSE;

  /* Parse the args */
  if(! PyArg_ParseTupleAndKeywords(args, keywds,"|dddd", kwlist,
				   &(data.dx), &(data.dy), &(data.dz), 
				   &(data.a)) ) {
    return NULL;
  }

  gts_surface_foreach_vertex(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),
			     (GtsFunc)rotate_point,&data);

  if(data.errflag) return NULL;

  Py_INCREF(Py_None);
  return Py_None;
}


/* Helper for scale() */
static void
scale_point(GtsPoint *p, TransformData *data)
{
  if(data->errflag) return;

  if(pygts_point_scale(p,data->dx,data->dy,data->dz)==-1)
    data->errflag=TRUE;
}

static PyObject*
scale(PygtsSurface* self, PyObject *args, PyObject *keywds)
{
  TransformData data;
  static char *kwlist[] = {"dx", "dy", "dz", NULL};

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  data.dx=1; data.dy=1; data.dz=1; data.a=0;
  data.errflag = FALSE;

  /* Parse the args */
  if(! PyArg_ParseTupleAndKeywords(args, keywds,"|ddd", kwlist,
				   &(data.dx), &(data.dy), &(data.dz)) ) {
    return NULL;
  }

  gts_surface_foreach_vertex(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),
			     (GtsFunc)scale_point,&data);

  if(data.errflag) return NULL;

  Py_INCREF(Py_None);
  return Py_None;
}


/* Helper for translate() */
static void
translate_point(GtsPoint *p, TransformData *data)
{
  if(data->errflag) return;

  if(pygts_point_translate(p,data->dx,data->dy,data->dz)==-1)
    data->errflag=TRUE;
}

static PyObject*
translate(PygtsSurface* self, PyObject *args, PyObject *keywds)
{
  TransformData data;
  static char *kwlist[] = {"dx", "dy", "dz", NULL};

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  data.dx=0; data.dy=0; data.dz=0; data.a=0;
  data.errflag = FALSE;

  /* Parse the args */
  if(! PyArg_ParseTupleAndKeywords(args, keywds,"|ddd", kwlist,
				   &(data.dx), &(data.dy), &(data.dz)) ) {
    return NULL;
  }

  gts_surface_foreach_vertex(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),
			     (GtsFunc)translate_point,&data);

  if(data.errflag) return NULL;

  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject*
is_self_intersecting(PygtsSurface *self, PyObject *args)
{
  GtsSurface *s;
  gboolean ret = FALSE;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  if( (s=gts_surface_is_self_intersecting(
             GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj))) != NULL) {
    gts_object_destroy(GTS_OBJECT(s));
    ret = TRUE;
  }

  if(ret) {
    Py_INCREF(Py_True);
    return Py_True;
  }
  else {
    Py_INCREF(Py_False);
    return Py_False;
  }  
}


static PyObject*
cleanup(PygtsSurface *self, PyObject *args)
{
  GtsSurface *s;
  gdouble threshold = 0.;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */
  if(! PyArg_ParseTuple(args,"|d", &threshold) ) {
    return NULL;
  }

  s = GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj);

  /* Do the cleanup */
  if( threshold != 0. ) {
    pygts_vertex_cleanup(s,threshold);
  }
  pygts_edge_cleanup(s);
  pygts_triangle_cleanup(s);

  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject*
coarsen(PygtsSurface *self, PyObject *args)
{
  GtsSurface *s;
  guint n;
  gdouble amin=0.;
  GtsVolumeOptimizedParams params = {0.5,0.5,1.e-10};

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  /* Parse the args */
  if(! PyArg_ParseTuple(args,"i|d", &n, &amin) ) {
    return NULL;
  }

  gts_surface_coarsen(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),
		      (GtsKeyFunc)gts_volume_optimized_cost, &params,
		      (GtsCoarsenFunc)gts_volume_optimized_vertex, &params,
		      (GtsStopFunc)gts_coarsen_stop_number, &n, amin);

  Py_INCREF(Py_None);
  return Py_None;
}


/* Methods table */
static PyMethodDef methods[] = {
  {"is_ok", (PyCFunction)is_ok,
   METH_NOARGS,
   "True if this Surface s is OK.  False otherwise.\n"
   "\n"
   "Signature: s.is_ok()\n"
  },  

  {"add", (PyCFunction)add,
   METH_VARARGS,
   "Adds Face f to Surface s.\n"
   "\n"
   "Signature: s.add(f)\n"
  },  

  {"remove", (PyCFunction)pygts_remove,
   METH_VARARGS,
   "Removes Face f to Surface s.\n"
   "\n"
   "Signature: s.remove(f)\n"
  },  

  {"copy", (PyCFunction)copy,
   METH_VARARGS,
   "Copys all Faces, Edges and Vertices of Surface s2 to Surface s1.\n"
   "\n"
   "Signature: s1.copy(s2)\n"
   "\n"
   "Returns s1.\n"
  },  

  {"merge", (PyCFunction)merge,
   METH_VARARGS,
   "Adds faces of Surface s2 which do not belong to Surface s1 to s1.\n"
   "\n"
   "Signature: s1.merge(s2)\n"
  },  

  {"read", (PyCFunction)pygts_read,
   METH_VARARGS,
   "Adds to Surface s the data read from File f.\n"
   "The File data must be stored in GTS format (e.g., by using.\n"
   "s.write())\n"
   "\n"
   "Signature: s.read(f)\n"
  },  

  {"is_manifold", (PyCFunction)is_manifold,
   METH_NOARGS,
   "True if Surface s is a manifold, False otherwise.\n"
   "\n"
   "Signature: s.is_manifold()\n"
  },  

  {"is_orientable", (PyCFunction)is_orientable,
   METH_NOARGS,
   "True if Faces in Surface s have compatible orientation,\n"
   "False otherwise.\n"
   "Note that a closed surface is also a manifold.  Note that an\n"
   "orientable surface is also a manifold.\n"
   "\n"
   "Signature: s.is_orientable()\n"
  },  

  {"is_closed", (PyCFunction)is_closed,
   METH_NOARGS,
   "True if Surface s is closed, False otherwise.\n"
   "Note that a closed Surface is also a manifold.\n"
   "\n"
   "Signature: s.is_closed()\n"
  },  

  {"area", (PyCFunction)area,
   METH_NOARGS,
   "Returns the area of Surface s.\n"
   "The area is taken as the sum of the signed areas of the Faces of s.\n"
   "\n"
   "Signature: s.area()\n"
  },  

  {"volume", (PyCFunction)volume,
   METH_NOARGS,
   "Returns the signed volume of the domain bounded by the Surface s.\n"
   "\n"
   "Signature: s.volume()\n"
  },  

  {"center_of_mass", (PyCFunction)center_of_mass,
   METH_NOARGS,
   "Returns the coordinates of the center of mass of Surface s.\n"
   "\n"
   "Signature: s.center_of_mass()\n"
  },  

  {"center_of_area", (PyCFunction)center_of_area,
   METH_NOARGS,
   "Returns the coordinates of the center of area of Surface s.\n"
   "\n"
   "Signature: s.center_of_area()\n"
  },  

  {"write", (PyCFunction)pygts_write,
   METH_VARARGS,
   "Saves Surface s to File f in GTS ascii format.\n"
   "All the lines beginning with #! are ignored.\n"
   "\n"
   "Signature: s.write(f)\n"
  },  

  {"split", (PyCFunction)split,
   METH_NOARGS,
   "Splits a surface into a tuple of connected and manifold components.\n"
   "\n"
   "Signature: s.split()\n"
  },  

  {"strip", (PyCFunction)strip,
   METH_NOARGS,
   "Returns a tuple of strips, where each strip is a tuple of Faces\n"
   "that are successive and have one edge in common.\n"
   "\n"
   "Signature: s.split()\n"
  },  

  {"stats", (PyCFunction)stats,
   METH_NOARGS,
   "Returns statistics for this Surface f in a dict.\n"
   "The stats include n_faces, n_incompatible_faces,, n_boundary_edges,\n"
   "n_non_manifold_edges, and the statisics {min, max, sum, sum2, mean,\n"
   "stddev, and n} for populations of edges_per_vertex and\n"
   "faces_per_edge.  Each of these names are dictionary keys.\n"
   "\n"
   "Signature: s.stats()\n"
  },  

  {"quality_stats", (PyCFunction)quality_stats,
   METH_NOARGS,
   "Returns quality statistics for this Surface f in a dict.\n"
   "The statistics include the {min, max, sum, sum2, mean, stddev,\n"
   "and n} for populations of face_quality, face_area, edge_length,\n"
   "and edge_angle.  Each of these names are dictionary keys.\n"
   "See Triangle.quality() for an explanation of the face_quality.\n"
   "\n"
   "Signature: s.quality_stats()\n"
  },  

  {"tessellate", (PyCFunction)tessellate,
   METH_NOARGS,
   "Tessellate each face of this Surface s with 4 triangles.\n"
   "The number of triangles is increased by a factor of 4.\n"
   "\n"
   "Signature: s.tessellate()\n"
  },  

  {"vertices", (PyCFunction)vertices,
   METH_NOARGS,
   "Returns a tuple containing the vertices of Surface s.\n"
   "\n"
   "Signature: s.vertices()\n"
  },  

  {"face_indices", (PyCFunction)face_indices,
   METH_VARARGS,
   "Returns a tuple of 3-tuples containing Vertex indices for each Face\n"
   "in Surface s.  The index for each Vertex in a face corresponds to\n"
   "where it is found in the Vertex tuple vs.\n"
   "\n"
   "Signature: s.face_indices(vs)\n"
  },  

  {"intersection", (PyCFunction)intersection,
   METH_VARARGS,
   "Returns the intersection of this Surface s1 with Surface s2.\n"
   "\n"
   "Signature: s1.intersection(s2)\n"
  },  

  {"union", (PyCFunction)pygts_union,
   METH_VARARGS,
   "Returns the union of this Surface s1 with Surface s2.\n"
   "\n"
   "Signature: s1.union(s2)\n"
  },  

  {"difference", (PyCFunction)difference,
   METH_VARARGS,
   "Returns the difference of this Surface s1 with Surface s2.\n"
   "\n"
   "Signature: s1.difference(s2)\n"
  },  

  {"rotate", (PyCFunction)rotate,
   METH_VARARGS | METH_KEYWORDS,
   "Rotates Surface s about vector dx,dy,dz and angle a.\n"
   "The sense of the rotation is given by the right-hand-rule.\n"
   "\n"
   "Signature: s.rotate(dx,dy,dz,a)\n"
  },  

  {"scale", (PyCFunction)scale,
   METH_VARARGS | METH_KEYWORDS,
   "Scales Surface s by vector dx,dy,dz.\n"
   "\n"
   "Signature: s.scale(dx=1,dy=1,dz=1)\n"
  },  

  {"translate", (PyCFunction)translate,
   METH_VARARGS | METH_KEYWORDS,
   "Translates Surface s by vector dx,dy,dz.\n"
   "\n"
   "Signature: s.translate(dx=0,dy=0,dz=0)\n"
  },  

  {"is_self_intersecting", (PyCFunction)is_self_intersecting,
   METH_NOARGS,
   "Returns True if this Surface s is self-intersecting.\n"
   "False otherwise.\n"
   "\n"
   "Signature: s.is_self_intersecting()\n"
  },  

  {"cleanup", (PyCFunction)cleanup,
   METH_VARARGS,
   "Cleans up the Vertices, Edges, and Faces on a Surface s.\n"
   "\n"
   "Signature: s.cleanup() or s.cleanup(threhold)\n"
   "\n"
   "If threhold is given, then Vertices that are spaced less than\n"
   "the threshold are merged.  Degenerate Edges and Faces are also\n"
   "removed.\n"
  },  

  {"coarsen", (PyCFunction)coarsen,
   METH_VARARGS,
   "Reduces the number of vertices on Surface s.\n"
   "\n"
   "Signature: s.coarsen(n) and s.coarsen(amin)\n"
   "\n"
   "n is the smallest number of desired edges (but you may get fewer).\n"
   "amin is the smallest angle between Faces.\n"
  },  

  {NULL}  /* Sentinel */
};


/*-------------------------------------------------------------------------*/
/* Attributes exported to python */

static PyObject *
get_Nvertices(PygtsSurface *self, void *closure)
{
#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  return Py_BuildValue("i",
      gts_surface_vertex_number(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj)));

}


static PyObject *
get_Nedges(PygtsSurface *self, void *closure)
{
#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  return Py_BuildValue("i",
      gts_surface_edge_number(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj)));

}


static PyObject *
get_Nfaces(PygtsSurface *self, void *closure)
{
#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return NULL;
  }
#endif

  return Py_BuildValue("i",
      gts_surface_face_number(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj)));
}

/* Methods table */
static PyGetSetDef getset[] = {
  { "Nvertices", (getter)get_Nvertices, NULL, 
    "The number of unique vertices", NULL
  },

  { "Nedges", (getter)get_Nedges, NULL, 
    "The number of unique edges", NULL
  },

  { "Nfaces", (getter)get_Nfaces, NULL, 
    "The number of unique faces", NULL
  },

  {NULL}  /* Sentinel */
};


/*-------------------------------------------------------------------------*/
/* Python type methods */

static void
dealloc(PygtsSurface* self)
{
  if(self->traverse!=NULL) {
    gts_surface_traverse_destroy(self->traverse);
  }
  self->traverse = NULL;

  /* Chain up */
  PygtsObjectType.tp_dealloc((PyObject*)self);
}


static PyObject *
new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  PygtsObject *obj;
  guint alloc_gtsobj = TRUE;
  static char *kwlist[] = {"alloc_gtsobj", NULL};

  /* Parse the args */
  if( args != NULL ) {
    if(! PyArg_ParseTupleAndKeywords(args, kwds, "|i", kwlist, 
				     &alloc_gtsobj)) {
      return NULL;
    }
  }

  /* Chain up */
  args = Py_BuildValue("(i)",FALSE);
  obj = PYGTS_OBJECT(PygtsObjectType.tp_new(type,args,NULL));
  Py_DECREF(args);

  PYGTS_SURFACE(obj)->traverse = NULL;

  /* Allocate the gtsobj (if needed) */
  if( alloc_gtsobj ) {
    obj->gtsobj = GTS_OBJECT(gts_surface_new(gts_surface_class(),
					     gts_face_class(),
					     gts_edge_class(),
					     gts_vertex_class()));

    if( obj->gtsobj == NULL )  {
      PyErr_SetString(PyExc_RuntimeError, "GTS could not create Surface");
      return NULL;
    }

    pygts_object_register(obj);
  }

  return (PyObject*)obj;
}


static int
init(PygtsSurface *self, PyObject *args, PyObject *kwds)
{
  gint ret;

  if( (ret = PygtsObjectType.tp_init((PyObject*)self,args,kwds)) != 0 ) {
    return ret;
  }

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object (internal error)");
    return -1;
  }
#endif

  return 0;
}


/* Helper function for iter */
static void 
get_f0(GtsFace *f,GtsFace **f0)
{
  if(*f0==NULL) *f0 = f;
}

PyObject* 
iter(PygtsSurface *self)
{
  GtsFace* f0=NULL;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object in iter() (internal error)");
    return NULL;
  }
#endif

  if(self->traverse!=NULL) {
    gts_surface_traverse_destroy(self->traverse);
    self->traverse = NULL;
  }

  /* Assign a "first" face */
  gts_surface_foreach_face(GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),
			   (GtsFunc)get_f0,&f0);
  if(f0==NULL) {
    PyErr_SetString(PyExc_RuntimeError, "No faces to traverse");
    return NULL;
  }

  if( (self->traverse=gts_surface_traverse_new(
           GTS_SURFACE(PYGTS_OBJECT(self)->gtsobj),f0)) == NULL ) {
    PyErr_SetString(PyExc_RuntimeError, "GTS could not create Traverse");
    return NULL;
  }

  Py_INCREF((PyObject*)self);
  return (PyObject*)self;
}


PyObject* 
iternext(PygtsSurface *self)
{
  PygtsObject *obj;
  GtsSurface *parent;
  GtsObject *face;
  PyObject *args;

#if PYGTS_DEBUG
  if(!pygts_surface_check((PyObject*)self)) {
    PyErr_SetString(PyExc_TypeError,
		    "problem with self object in iternext() (internal error)");
    return NULL;
  }
#endif

  if( self->traverse == NULL ) {
    PyErr_SetString(PyExc_RuntimeError, "Iterator not initialized");
    return NULL;
  }

  /* Get the next face */
  if( (face = GTS_OBJECT(gts_surface_traverse_next(self->traverse,NULL)))
      == NULL ) {
    gts_surface_traverse_destroy(self->traverse);
    self->traverse = NULL;
    PyErr_SetString(PyExc_StopIteration, "No more faces");
    return NULL;
  }

  /* If corresponding PyObject found in object table, we are done */
  if( (obj=g_hash_table_lookup(obj_table,face)) != NULL ) {
    Py_INCREF(obj);
    return (PyObject*)obj;
  }

  /* Chain up object allocation */
  args = Py_BuildValue("OOOi",Py_None,Py_None,Py_None,FALSE);
  obj = PYGTS_OBJECT(PygtsFaceType.tp_new(&PygtsFaceType, args, NULL));
  Py_DECREF(args);
  if( obj == NULL ) return NULL;
  if( (parent = pygts_face_parent(GTS_FACE(face))) == NULL ) {
    gts_surface_traverse_destroy(self->traverse);
    self->traverse = NULL;
    Py_DECREF(obj);
    return NULL;
  }
  obj->gtsobj = face;
  obj->gtsobj_parent = GTS_OBJECT(parent);

  pygts_object_register(obj);
  return (PyObject*)obj;
}


/* Methods table */
PyTypeObject PygtsSurfaceType = {
    PyObject_HEAD_INIT(NULL)
    0,                       /* ob_size */
    "gts.Surface",           /* tp_name */
    sizeof(PygtsSurface),    /* tp_basicsize */
    0,                       /* tp_itemsize */
    (destructor)dealloc,     /* tp_dealloc */
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
      Py_TPFLAGS_BASETYPE |
      Py_TPFLAGS_HAVE_ITER,  /* tp_flags */
    "Surface object",        /* tp_doc */
    0,                       /* tp_traverse */
    0,                       /* tp_clear */
    0,                       /* tp_richcompare */
    0,                       /* tp_weaklistoffset */
    (getiterfunc)iter,       /* tp_iter */
    (iternextfunc)iternext,  /* tp_iternext */
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
pygts_surface_check(PyObject* o)
{
  if(! PyObject_TypeCheck(o, &PygtsSurfaceType)) {
    return FALSE;
  }
  else {
#if PYGTS_DEBUG
    return pygts_surface_is_ok(PYGTS_SURFACE(o));
#else
    return TRUE;
#endif
  }
}


/* Helper function */
static void 
face_is_ok(GtsFace *f,gboolean *ret)
{
  if( !pygts_gts_triangle_is_ok(GTS_TRIANGLE(f)) ) {
    *ret = FALSE;
  }
}


gboolean 
pygts_surface_is_ok(PygtsSurface *s)
{
  PygtsObject *obj;
  gboolean ret=TRUE;

  obj = PYGTS_OBJECT(s);

  if(!pygts_object_is_ok(PYGTS_OBJECT(s))) return FALSE;
  g_return_val_if_fail(obj->gtsobj_parent==NULL,FALSE);

  /* Check all of the faces this surface contains */
  gts_surface_foreach_face(GTS_SURFACE(obj->gtsobj),(GtsFunc)face_is_ok,&ret);
  if( ret == FALSE ) return FALSE;

  return TRUE;
}
