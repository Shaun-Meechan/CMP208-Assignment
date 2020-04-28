#pragma once

#include <game_object.h>
#include <box2d/Box2D.h>
#include "graphics/scene.h"
#include <maths/vector4.h>
#include <graphics/renderer_3d.h>
#include <maths/math_utils.h>

namespace gef
{
	class Renderer3D;
}

class EnemyObject: public GameObject
{
public:
	EnemyObject(gef::Scene* sceneFile, b2World* world, float xSpawnValue, gef::Mesh* mesh);
	~EnemyObject();
	b2Body* getBody();
	int getHealth();
	void decrementHealth(int value);
	void render(gef::Renderer3D* renderer_3d_);
	//Transform functions
	void updateScale(gef::Vector4 scaleVector);
	void updateRotationX(float degrees);
	void updateRotationY(float degrees);
	void updateRotationZ(float degrees);
	void translate(gef::Vector4 translationVector);
	void update();
	void setHit(bool value);
	bool getHit();
	void setStoppedMoving(bool value);
	bool getStoppedMoving();
	void setCollidingWithEnemy(bool value);
	bool getCollidingWithEnemy();
	void setCollidingWithPlayer(bool value);
	bool getCollidingWithPlayer();
private:
	b2Body* body;
	b2BodyDef bodyDef;
	b2PolygonShape shape;
	b2FixtureDef fixtureDef;
	bool stoppedMoving = false;
	bool collidingWithEnemy = false;
	bool collidingWithPlayer = false;
	bool hit = false;
	b2Vec2* spawnPoints[5];
	int health;
	gef::Vector4 objectTranslation;
	gef::Matrix44 scaleMatrix;
	gef::Matrix44 rotationMatrix;
	gef::Matrix44 translationMatrix;
};

