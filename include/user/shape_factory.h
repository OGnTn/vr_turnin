#ifndef SHAPE_FACTORY_H
#define SHAPE_FACTORY_H
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include "user/mesh.h"

class ShapeFactory
{
public:
    static btConvexHullShape *create_convex_hull_shape(std::vector<Mesh> meshes);
    static btBvhTriangleMeshShape *create_bvh_triangle_mesh_shape(std::vector<Mesh> meshes);
};

#endif