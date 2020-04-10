#pragma once
#include <game_object.h>
#include <box2d/box2d.h>
#include <maths/vector4.h>
#include <maths/math_utils.h>
#include <graphics/scene.h>
#include <graphics/renderer_3d.h>
namespace gef
{
	class Renderer3D;
}

class WallObject : public GameObject
{
public:
	WallObject(gef::Scene* sceneFile, b2World* world);
	b2Body* getBody();
	//Transform functions
	void updateScale(gef::Vector4 scaleVector);
	void updateRotationX(float degrees);
	void updateRotationY(float degrees);
	void updateRotationZ(float degrees);
	void translate(gef::Vector4 translationVector);
	void update();
	void render(gef::Renderer3D* renderer_3d_);
private:
	gef::Mesh* getMeshFromSceneAssets(gef::Scene* scene);
	b2Body* body;
	b2BodyDef bodyDef;
	float lastDamageTime;
	gef::Vector4 objectTranslation;
	gef::Matrix44 scaleMatrix;
	gef::Matrix44 rotationMatrix;
	gef::Matrix44 translationMatrix;
};

