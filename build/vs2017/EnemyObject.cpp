#include "EnemyObject.h"

EnemyObject::EnemyObject(gef::Scene* sceneFile, b2World* world)
{
	health = 100;
	// setup the mesh for the enemy
	this->set_mesh(getMeshFromSceneAssets(sceneFile));
	// create a physics body for the enemy
	bodyDef.type = b2_dynamicBody;

	//Initialise our spawn points
	spawnPoints[0] = new b2Vec2(-33, 10);
	spawnPoints[1] = new b2Vec2(-33, 0);
	spawnPoints[2] = new b2Vec2(-33, -10);

	int randomNumber = rand() % 3;

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
	default:
		break;
	}

	body = world->CreateBody(&bodyDef);

	// create the shape for the enemy (collider?)
	shape.SetAsBox(0.1f, 0.1f);

	// create the fixture
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;

	// create the fixture on the rigid body
	body->CreateFixture(&fixtureDef);

	this->set_type(ENEMY);
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

gef::Mesh* EnemyObject::getMeshFromSceneAssets(gef::Scene* scene)
{
	gef::Mesh* mesh = NULL;

	// if the scene data contains at least one mesh
	// return the first mesh
	if (scene && scene->meshes.size() > 0)
		mesh = scene->meshes.front();

	return mesh;
}
