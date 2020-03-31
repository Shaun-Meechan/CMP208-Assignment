#pragma once

#include <game_object.h>
#include <primitive_builder.h>

class hitDetectionObject: public GameObject
{
public:
	hitDetectionObject(b2World* world, PrimitiveBuilder* pb);
	b2Body* getBody();
private:
	b2Body* body;
	b2BodyDef bodyDef;
	b2PolygonShape shape;
	b2FixtureDef fixtureDef;
};

