#include "user/shape_factory.h"

btConvexHullShape *ShapeFactory::create_convex_hull_shape(std::vector<Mesh> meshes)
{
    btConvexHullShape *convex_hull_shape = new btConvexHullShape();
    for (const auto &mesh : meshes)
    {
        for (const auto &vertex : mesh.vertices)
        {
            convex_hull_shape->addPoint(btVector3(vertex.position.x, vertex.position.y, vertex.position.z));
        }
    }
    return convex_hull_shape;
}
btBvhTriangleMeshShape *ShapeFactory::create_bvh_triangle_mesh_shape(std::vector<Mesh> meshes)
{
    btTriangleMesh *triangle_mesh = new btTriangleMesh();
    for (const auto &mesh : meshes)
    {
        for (size_t i = 0; i < mesh.indices.size(); i += 3)
        {
            btVector3 v0(mesh.vertices[mesh.indices[i]].position.x, mesh.vertices[mesh.indices[i]].position.y, mesh.vertices[mesh.indices[i]].position.z);
            btVector3 v1(mesh.vertices[mesh.indices[i + 1]].position.x, mesh.vertices[mesh.indices[i + 1]].position.y, mesh.vertices[mesh.indices[i + 1]].position.z);
            btVector3 v2(mesh.vertices[mesh.indices[i + 2]].position.x, mesh.vertices[mesh.indices[i + 2]].position.y, mesh.vertices[mesh.indices[i + 2]].position.z);
            triangle_mesh->addTriangle(v0, v1, v2);
        }
    }

    btBvhTriangleMeshShape *bvh_triangle_mesh_shape = new btBvhTriangleMeshShape(triangle_mesh, true);
    return bvh_triangle_mesh_shape;
}
