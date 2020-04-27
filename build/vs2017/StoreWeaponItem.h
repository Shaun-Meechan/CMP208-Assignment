#pragma once

#include "Weapon.h"
#include <graphics/sprite.h>
#include <load_texture.h>
#include <box2d/box2d.h>
#include "PlayerData.h"

class StoreWeaponItem: public gef::Sprite
{
public:
	StoreWeaponItem(const char* pngFileName, gef::Platform* platform, int newCost, b2World* world, b2Vec2 bodyPos, Weapon weapon);
	int getCost();
	//Do something
	PlayerData run(PlayerData playerData);
	b2Body* getBody();
	bool didPurchaseSucced();
	char* getName();
private:
	gef::Texture* icon;
	int cost = 0;
	b2Body* body;
	b2BodyDef bodyDef;
	bool canPlayerAfford(PlayerData* playerData);
	bool purchaseSuccessful = false;
	char* name = "";
	Weapon linkedWeapon;
};

