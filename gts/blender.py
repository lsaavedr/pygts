# pygts - python package for the manipulation of triangulated surfaces
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

"""blender.py - interface between PyGTS and Blender

*** DO NOT USE.  THIS IS EXPERIMENTAL AND HAS NOT BEEN TESTED. ***

"""


import gts
import Blender, bpy


class Surface(gts.Surface):

    def __init__(self,name):
        self.name = name
        self.mesh = None
#        gts.Surface.__init__(self)


    def pull(self):

        # Empty out current Surface
        for face in self:
            self.remove(face)

        if not self.mesh:
            self.mesh = bpy.data.meshes.get(self.name)
            
        
    def push(self):

        # Save Blender's current state
        editmode = Blender.Window.EditMode()
        if editmode: 
            Blender.Window.EditMode(0)

        if not self.mesh:
            
            # Create a new mesh
            self.mesh = bpy.data.meshes.new(self.name)

            # Link mesh to current scene
            scn = bpy.data.scenes.active
            ob = scn.objects.new(self.mesh, 'myObj')

        # Clear out the mesh
        self.mesh.faces.delete([i for i,face in enumerate(self.mesh.faces)])
        self.mesh.verts.delete([i for i,face in enumerate(self.mesh.verts)])

        # Put new vertices and faces into the mesh
        vertices,faces = gts.get_coords_and_face_indices(self)
        self.mesh.verts.extend(coords)
        self.mesh.faces.extend(faces)

        # Return Blender to original state
        if editmode: 
            Blender.Window.EditMode(1)

