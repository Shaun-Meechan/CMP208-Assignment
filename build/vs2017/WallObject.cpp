#include "WallObject.h"

WallObject::WallObject(gef::Scene* sceneFile, b2World* world)
{
	// setup the mesh for the enemy
	this->set_mesh(getMeshFromSceneAssets(sceneFile));
	// create a physics body for the wall
	bodyDef.type = b2_staticBody;

	bodyDef.position.Set(3.0f, -1.5f);

	body = world->CreateBody(&bodyDef);

	this->UpdateFromSimulation(body);

	body->SetUserData(this);

	this->set_type(PLAYER);

	lastDamageTime = 0;

	scaleMatrix.SetIdentity();
	rotationMatrix.SetIdentity();
	translationMatrix.SetIdentity();

	return;
}

b2Body* WallObject::getBody()
{
	return body;
}

void WallObject::updateScale(gef::Vector4 scaleVector)
{
	scaleMatrix.Scale(scaleVector);
	return;
}

void WallObject::updateRotationX(float degrees)
{
	rotationMatrix.RotationX(gef::DegToRad(degrees));
	return;
}

void WallObject::updateRotationY(float degrees)
{
	rotationMatrix.RotationY(gef::DegToRad(degrees));
	return;
}

void WallObject::updateRotationZ(float degrees)
{
	rotationMatrix.RotationZ(gef::DegToRad(degrees));
	return;
}

void WallObject::translate(gef::Vector4 translationVector)
{
	translationMatrix.SetTranslation(translationVector);
	return;
}

void WallObject::update()
{
	objectTranslation = gef::Vector4(body->GetPosition().x, body->GetPosition().y, 0);
	translationMatrix.SetTranslation(objectTranslation);
	return;
}

void WallObject::render(gef::Renderer3D* renderer_3d_)
{
	this->set_transform((scaleMatrix * rotationMatrix) * translationMatrix);
	renderer_3d_->DrawMesh(*this);
	return;
}

gef::Mesh* WallObject::getMeshFromSceneAssets(gef::Scene* scene)
{
	gef::Mesh* mesh = NULL;

	// if the scene data contains at least one mesh
	// return the first mesh
	if (scene && scene->meshes.size() > 0)
		mesh = scene->meshes.front();

	return mesh;
}
