#pragma once

#include <graphics/sprite.h>
#include <load_texture.h>
#include <box2d/box2d.h>
#include "PlayerData.h"

namespace gef
{
	class Platform;
}

enum class itemType
{
	Health,
	Rifleman,
	RepairGuy,
	Weapon
};

class StoreItem: public gef::Sprite
{
public:
	StoreItem(const char* pngFileName, gef::Platform* platform, int newCost, string newType, b2World* world, b2Vec2 bodyPos);
	int getCost();
	//Do something
	PlayerData run(PlayerData playerData);
	b2Body* getBody();
	bool didPurchaseSucced();
	char* getName();
private:
	gef::Texture* icon;
	int cost = 0;
	itemType type;
	b2Body* body;
	b2BodyDef bodyDef;
	bool canPlayerAfford(PlayerData* playerData);
	bool purchaseSuccessful = false;
	char* name = "";
};

