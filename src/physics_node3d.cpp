#include "user/physics_node3d.h"

PhysicsNode3D::PhysicsNode3D(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Shader &shader, std::string model_file_path, float mass, bool invert_normals) : Node3D(position, rotation, scale, shader, model_file_path, invert_normals)
{
    this->mass = mass;
    if (this->mass != 0.0f)
    {
        btCollisionShape *collision_shape = ShapeFactory::create_convex_hull_shape(this->meshes);
        this->rigid_body = create_rigid_body(collision_shape);
    }
    else
    {
        btCollisionShape *collision_shape = ShapeFactory::create_bvh_triangle_mesh_shape(this->meshes);
        this->rigid_body = create_rigid_body(collision_shape);
    }
    update_scale();
}

PhysicsNode3D::PhysicsNode3D(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Shader &shader, std::string model_file_path, float mass, btCollisionShape *collision_shape) : Node3D(position, rotation, scale, shader, model_file_path)
{
    this->mass = mass;
}

btRigidBody *PhysicsNode3D::create_rigid_body(btCollisionShape *collision_shape)
{
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(this->position.x, this->position.y, this->position.z));
    transform.setRotation(btQuaternion(this->rotation.x, this->rotation.y, this->rotation.z));

    btVector3 localInertia(0, 0, 0);
    if (this->mass != 0.0f)
    {
        btScalar _mass(this->mass);
        collision_shape->calculateLocalInertia(_mass, localInertia);
    }
    // localInertia /= 500.0f;
    if (localInertia.length() >= 3000.0f)
    {
        localInertia /= 1000.0f;
    }

    // localInertia /= 10.0f;
    std::cout << "Local inertia: " << localInertia.getX() << ", " << localInertia.getY() << ", " << localInertia.getZ() << std::endl;
    btDefaultMotionState *motionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo rigid_bodyCI(this->mass, motionState, collision_shape, localInertia);
    btRigidBody *rigid_body = new btRigidBody(rigid_bodyCI);
    return rigid_body;
}

void PhysicsNode3D::update()
{
    btTransform transform;
    this->rigid_body->getMotionState()->getWorldTransform(transform);
    btVector3 origin = transform.getOrigin();
    // std::cout << "Position: " << origin.getX() << ", " << origin.getY() << ", " << origin.getZ() << std::endl;
    this->set_position(glm::vec3(origin.getX(), origin.getY(), origin.getZ()));
    btQuaternion rotation = transform.getRotation();
    float x, y, z;
    rotation.getEulerZYX(z, y, x);
    // this->set_rotation(glm::vec3(x, y, z));
    this->set_rotation(glm::vec3(glm::degrees(x), glm::degrees(y), glm::degrees(z)));

    // std::cout << "Rotation: " << rotation.getX() << ", " << rotation.getY() << ", " << rotation.getZ() << std::endl;
}

void PhysicsNode3D::update_scale()
{
    this->rigid_body->getCollisionShape()->setLocalScaling(btVector3(this->scale.x, this->scale.y, this->scale.z));
}