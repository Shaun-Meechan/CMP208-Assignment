#include "EnemyObject.h"

EnemyObject::EnemyObject()
{
	// setup the mesh for the enemy
	this->set_mesh(getMeshFromSceneAssets(enemySceneAsset));//<- does't have access to that func
	// create a physics body for the enemy
	bodyDef.type = b2_dynamicBody;

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

	//enemyBody = world_->CreateBody(&enemy_body_def); <- Could be done in game init.

	// create the shape for the enemy (collider?)
	shape.SetAsBox(0.5f, 0.5f);

	// create the fixture
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;

	// create the fixture on the rigid body
	body->CreateFixture(&fixtureDef);

	// update visuals from simulation data
	//enemy.UpdateFromSimulation(enemyBody); <- Could be done in game init.

	//Connect body to the game object
	//enemyBody->SetUserData(&enemy); <- Could be done in game init.

	this->set_type(ENEMY);

	gef::DebugOut("Created enemy\n");
}
