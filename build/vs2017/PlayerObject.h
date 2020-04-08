#pragma once

#include <game_object.h>
#include <box2d/box2d.h>
#include "graphics/scene.h"
#include <maths/vector4.h>
#include <maths/math_utils.h>
#include <graphics/renderer_3d.h>
#include <Weapon.h>

namespace gef
{
	class Renderer3D;
}

class PlayerObject: public GameObject
{
public:
	PlayerObject(gef::Scene* sceneFile, b2World* world, Weapon* weapon);
	b2Body* getBody();
	int getHealth();
	void decrementHealth(float time);
	int getCredits();
	void addCredits(int value);
	void decrementCredits(int value);
	//Transform functions
	void updateScale(gef::Vector4 scaleVector);
	void updateRotationX(float degrees);
	void updateRotationY(float degrees);
	void updateRotationZ(float degrees);
	void translate(gef::Vector4 translationVector);
	void update();
	void render(gef::Renderer3D* renderer_3d_);
	//Weapons functions
	Weapon getActiveWeapon();
	void addWeapon(Weapon newWeapon);
private:
	gef::Mesh* getMeshFromSceneAssets(gef::Scene* scene);
	b2Body* body;
	b2BodyDef bodyDef;
	b2PolygonShape shape;
	b2FixtureDef fixtureDef;
	int health;
	float lastDamageTime;
	gef::Vector4 objectTranslation;
	gef::Matrix44 scaleMatrix;
	gef::Matrix44 rotationMatrix;
	gef::Matrix44 translationMatrix;
	unsigned int credits;
	std::vector<Weapon*> weapons;
	Weapon* activeWeapon;
};

