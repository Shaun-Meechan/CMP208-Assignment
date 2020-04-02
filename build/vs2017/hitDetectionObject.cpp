#include "hitDetectionObject.h"

hitDetectionObject::hitDetectionObject(b2World* world, PrimitiveBuilder* pb)
{
	this->set_mesh(pb->GetDefaultCubeMesh());

	bodyDef.type = b2_dynamicBody;

	bodyDef.position = b2Vec2(0,0);

	body = world->CreateBody(&bodyDef);

	shape.SetAsBox(1.0f, 1.0f);

	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;

	body->CreateFixture(&fixtureDef);

	this->set_type(BULLET);

	body->SetUserData(this);

	rotationMatrix.SetIdentity();
	translationMatrix.SetIdentity();
	scaleMatrix.SetIdentity();

}

b2Body* hitDetectionObject::getBody()
{
	return body;
}

void hitDetectionObject::render(gef::Renderer3D* renderer_3d_)
{
	this->set_transform((scaleMatrix * rotationMatrix) * translationMatrix);
	renderer_3d_->DrawMesh(*this);
}

void hitDetectionObject::updateScale(gef::Vector4 scaleVector)
{
	scaleMatrix.Scale(scaleVector);
}

void hitDetectionObject::updateRotationX(float degrees)
{
	rotationMatrix.RotationX(gef::DegToRad(degrees));
}

void hitDetectionObject::updateRotationY(float degrees)
{
	rotationMatrix.RotationY(gef::DegToRad(degrees));
}

void hitDetectionObject::updateRotationZ(float degrees)
{
	rotationMatrix.RotationZ(gef::DegToRad(degrees));
}

void hitDetectionObject::translate(gef::Vector4 translationVector)
{
	translationMatrix.SetIdentity();
	translationMatrix.SetTranslation(translationVector);
}

void hitDetectionObject::update()
{
	translationMatrix.SetIdentity();
	objectTranslation = gef::Vector4(body->GetPosition().x, body->GetPosition().y, 0);
	translationMatrix.SetTranslation(objectTranslation);
}
