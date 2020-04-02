#pragma once

#include <game_object.h>
#include <primitive_builder.h>
#include <graphics/renderer_3d.h>
#include <maths/math_utils.h>

namespace gef
{
	class Renderer3D;
}
class hitDetectionObject: public GameObject
{
public:
	hitDetectionObject(b2World* world, PrimitiveBuilder* pb);
	b2Body* getBody();
	void render(gef::Renderer3D* renderer_3d_);
	//Transform functions
	void updateScale(gef::Vector4 scaleVector);
	void updateRotationX(float degrees);
	void updateRotationY(float degrees);
	void updateRotationZ(float degrees);
	void translate(gef::Vector4 translationVector);
	void update();
private:
	b2Body* body;
	b2BodyDef bodyDef;
	b2PolygonShape shape;
	b2FixtureDef fixtureDef;
	gef::Vector4 objectTranslation;
	gef::Matrix44 scaleMatrix;
	gef::Matrix44 rotationMatrix;
	gef::Matrix44 translationMatrix;
};

