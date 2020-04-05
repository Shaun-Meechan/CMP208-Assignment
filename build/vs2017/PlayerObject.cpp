#include "PlayerObject.h"

void PlayerObject::init(gef::Scene* sceneFile, b2World* world)
{
	credits = 0;
	health = 100;
	// setup the mesh for the enemy
	this->set_mesh(getMeshFromSceneAssets(sceneFile));
	// create a physics body for the enemy
	bodyDef.type = b2_staticBody;
 
	bodyDef.position.Set(9.2f, -1.5f); //Use this to move the player

	body = world->CreateBody(&bodyDef);

	// create the shape for the player
	shape.SetAsBox(7.0f, 100.0f);

	// create the fixture
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;

	// create the fixture on the rigid body
	body->CreateFixture(&fixtureDef);

	this->UpdateFromSimulation(body);

	body->SetUserData(this);

	this->set_type(PLAYER);

	lastDamageTime = 0;

	scaleMatrix.SetIdentity();
	rotationMatrix.SetIdentity();
	translationMatrix.SetIdentity();
}

b2Body* PlayerObject::getBody()
{
	return body;
}

int PlayerObject::getHealth()
{
	return health;
}

void PlayerObject::decrementHealth(float time)
{
	if (lastDamageTime + 1 <= time)
	{
		//We last took damge 5 seconds ago, take damage again.
		health--;
		lastDamageTime = time;
	}
}

int PlayerObject::getCredits()
{
	return credits;
}

void PlayerObject::addCredits(int value)
{
	credits = credits + value;
}

void PlayerObject::decrementCredits(int value)
{
	credits = credits - value;
}

gef::Mesh* PlayerObject::getMeshFromSceneAssets(gef::Scene* scene)
{
	gef::Mesh* mesh = NULL;

	// if the scene data contains at least one mesh
	// return the first mesh
	if (scene && scene->meshes.size() > 0)
		mesh = scene->meshes.front();

	return mesh;
}

void PlayerObject::updateScale(gef::Vector4 scaleVector)
{
	scaleMatrix.Scale(scaleVector);
}

void PlayerObject::updateRotationX(float degrees)
{
	rotationMatrix.RotationX(gef::DegToRad(degrees));
}

void PlayerObject::updateRotationY(float degrees)
{
	rotationMatrix.RotationY(gef::DegToRad(degrees));
}

void PlayerObject::updateRotationZ(float degrees)
{
	rotationMatrix.RotationZ(gef::DegToRad(degrees));
}

void PlayerObject::translate(gef::Vector4 translationVector)
{
	translationMatrix.SetTranslation(translationVector);
}

void PlayerObject::update()
{
	objectTranslation = gef::Vector4(body->GetPosition().x, body->GetPosition().y, 0);
	translationMatrix.SetTranslation(objectTranslation);
}

void PlayerObject::render(gef::Renderer3D* renderer_3d_)
{
	this->set_transform((scaleMatrix * rotationMatrix) * translationMatrix);
	renderer_3d_->DrawMesh(*this);
}
