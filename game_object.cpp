#include "game_object.h"
#include <system/debug_log.h>
#include <dos.h>

//
// UpdateFromSimulation
// 
// Update the transform of this object from a physics rigid body
//
void GameObject::UpdateFromSimulation(const b2Body* body)
{
	if (body)
	{
		// setup object rotation
		gef::Matrix44 object_rotation;
		object_rotation.RotationZ(body->GetAngle());

		// setup the object translation
		gef::Vector4 object_translation(body->GetPosition().x, body->GetPosition().y, 0.0f);

		// build object transformation matrix
		gef::Matrix44 object_transform = object_rotation;
		object_transform.SetTranslation(object_translation);
		set_transform(object_transform);
	}
}

void GameObject::MyCollisionResponse()
{
	//gef::DebugOut("A collision has happened.\n");
}

Player::Player()
{
	set_type(PLAYER);
	health = 100;
	lastDamageTime = 0;
}

void Player::DecrementHealth(float time)
{
	if (lastDamageTime + 1 <= time)
	{
		//We last took damge 5 seconds ago, take damage again.
		health--;
		lastDamageTime = time;
	}
}

int Player::getHealth()
{
	return health;
}

Enemy::Enemy()
{
	set_type(ENEMY);
}

void Enemy::DecrementHealth()
{
	//gef::DebugOut("Enemy has taken damage.\n");
}
