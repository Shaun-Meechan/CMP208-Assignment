#pragma once

#include <game_object.h>
#include <box2d/Box2D.h>
#include "graphics/scene.h"
#include <maths/vector4.h>

class EnemyObject: public GameObject
{
public:
	EnemyObject(gef::Scene* sceneFile, b2World* world);
	b2Body* getBody();
	int getHealth();
	void decrementHealth(int value);
private:
	b2Body* body;
	b2BodyDef bodyDef;
	b2PolygonShape shape;
	b2FixtureDef fixtureDef;
	gef::MeshInstance enemyMesh;
	b2Vec2* spawnPoints[3];
	gef::Mesh* getMeshFromSceneAssets(gef::Scene* scene);
	int health;
};

