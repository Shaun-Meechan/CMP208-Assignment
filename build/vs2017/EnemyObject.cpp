#include "EnemyObject.h"
#include <system\debug_log.h>

EnemyObject::EnemyObject(gef::Scene* sceneFile, b2World* world, float xSpawnValue, gef::Mesh* mesh)
{
	health = 100;
	// setup the mesh for the enemy
	this->set_mesh(mesh);
	// create a physics body for the enemy
	bodyDef.type = b2_dynamicBody;

	//Initialise our spawn points
	spawnPoints[0] = new b2Vec2(xSpawnValue, 2);
	spawnPoints[1] = new b2Vec2(xSpawnValue, 0.5);
	spawnPoints[2] = new b2Vec2(xSpawnValue, -1);
	spawnPoints[3] = new b2Vec2(xSpawnValue, -3.5);
	spawnPoints[4] = new b2Vec2(xSpawnValue, -5);

	int randomNumber = rand() % 5;

	switch (randomNumber)
	{
	case 0:
		bodyDef.position = *spawnPoints[0];
		break;
	case 1:
		bodyDef.position = *spawnPoints[1];
		break;
	case 2:
		bodyDef.position = *spawnPoints[2];
		break;
	case 3:
		bodyDef.position = *spawnPoints[3];
		break;
	case 4:
		bodyDef.position = *spawnPoints[4];
		break;
	default:
		gef::DebugOut("ERROR: unable to set enemy spawn points in EnemyObject.cpp");
		break;
	}

	body = world->CreateBody(&bodyDef);

	// create the shape for the enemy
	shape.SetAsBox(0.1f, 0.1f);

	// create the fixture
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;

	// create the fixture on the rigid body
	body->CreateFixture(&fixtureDef);

	this->set_type(ENEMY);

	body->SetUserData(this);

	rotationMatrix.SetIdentity();
	translationMatrix.SetIdentity();
	scaleMatrix.SetIdentity();

	scaleMatrix.Scale(gef::Vector4(0.2f, 0.2f, 0.2f));
	rotationMatrix.RotationY(gef::DegToRad(90));
}

EnemyObject::~EnemyObject()
{
	//delete body;
	//delete bodyDef;
	//delete shape;
	//delete fixtureDef;
	//delete enemyMesh;
	//delete spawnPoints;
}

b2Body* EnemyObject::getBody()
{
	return body;
}

int EnemyObject::getHealth()
{
	return health;
}

void EnemyObject::decrementHealth(int value)
{
	health = health - value;
}

void EnemyObject::render(gef::Renderer3D* renderer_3d_)
{
	this->set_transform((scaleMatrix * rotationMatrix) * translationMatrix);
	renderer_3d_->DrawMesh(*this);
}

void EnemyObject::updateScale(gef::Vector4 scaleVector)
{
	scaleMatrix.Scale(scaleVector);
}

void EnemyObject::updateRotationX(float degrees)
{
	rotationMatrix.RotationX(gef::DegToRad(degrees));
}

void EnemyObject::updateRotationY(float degrees)
{
	rotationMatrix.RotationY(gef::DegToRad(degrees));
}

void EnemyObject::updateRotationZ(float degrees)
{
	rotationMatrix.RotationZ(gef::DegToRad(degrees));
}

void EnemyObject::translate(gef::Vector4 translationVector)
{
	translationMatrix.SetTranslation(translationVector);
}

void EnemyObject::update()
{
	objectTranslation = gef::Vector4(body->GetPosition().x, body->GetPosition().y, 0);
	translationMatrix.SetTranslation(objectTranslation);
}