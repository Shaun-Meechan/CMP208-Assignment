#pragma once

#include <game_object.h>
#include <box2d/Box2D.h>

class EnemyObject: public GameObject
{
public:
	EnemyObject();
private:
	b2Body* body;
	b2BodyDef bodyDef;
	b2PolygonShape shape;
	b2FixtureDef fixtureDef;


};

