#!/usr/bin/env python3

# Requires http://www.assimp.org/ and pyassimp:
#  pip3 install pyassimp

# To convert verts.obj to verts.h, run:
#  ./verts.py verts.obj > verts.h

import sys
import pyassimp

processing = 0

if False:
  # We want to render triangles, so we remove anything else
  processing |= pyassimp.postprocess.aiProcess_Triangulate
  processing |= pyassimp.postprocess.aiProcess_FindDegenerates
  #processing |= pyassimp.postprocess.aiProcess_SortByPType #FIXME: How to AI_CONFIG_PP_SBP_REMOVE in pyassimp?

# We require UV and Normal for each vertex
processing |= pyassimp.postprocess.aiProcess_GenUVCoords
processing |= pyassimp.postprocess.aiProcess_GenNormals

# We don't have a scene-graph, so pre-transform the mesh
processing |= pyassimp.postprocess.aiProcess_PreTransformVertices

# See https://sourceforge.net/p/assimp/discussion/817654/thread/026e9640/#0f55
processing |= pyassimp.postprocess.aiProcess_JoinIdenticalVertices

# Load the 3D model
scene = pyassimp.load(sys.argv[1], processing=processing)

# We need exactly 1 mesh
assert(len(scene.meshes) == 1)

# Get the mesh
mesh = scene.meshes[0]

# We need at least 1 texcoord
#FIXME: Warn that others will be ignored
assert(len(mesh.texturecoords) >= 1)
if len(mesh.texturecoords) != 1:
  print("Warning: Only first UV set is used", file=sys.stderr)

# We need normals and texcoord for each vertex
assert(len(mesh.normals) == len(mesh.vertices))
assert(len(mesh.texturecoords[0]) == len(mesh.vertices))

# Output all vertices
print("struct Vertex vertices[] = {")
for i, vertex in enumerate(mesh.vertices):
  x, y, z = vertex
  nx, ny, nz = mesh.normals[i]
  u, v, _ = mesh.texturecoords[0][i]
  print("\t{{ %s, %s, %s }, { %s, %s, %s }, { %s, %s }}," % (x, y, z,
                                                             nx, ny, nz,
                                                             u, v))
print("};")
print("")

# Output all indices
print("uint16_t indices[] = {")
for face in mesh.faces:
  assert(len(face) == 3)
  a, b, c = face
  assert(a <= 0xFFFF)
  assert(b <= 0xFFFF)
  assert(c <= 0xFFFF)
  print("\t%d, %d, %d," % (a, b, c))
print("};")

# Unload assimp scene
pyassimp.release(scene)
