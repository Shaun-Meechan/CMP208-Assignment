#include "hitDetectionObject.h"

hitDetectionObject::hitDetectionObject(b2World* world, PrimitiveBuilder* pb)
{
	this->set_mesh(pb->GetDefaultCubeMesh());

	bodyDef.type = b2_dynamicBody;

	body = world->CreateBody(&bodyDef);

	shape.SetAsBox(0.5f, 0.5f);

	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;

	body->CreateFixture(&fixtureDef);

	this->set_type(BULLET);

	body->SetUserData(this);
}

b2Body* hitDetectionObject::getBody()
{
	return body;
}
