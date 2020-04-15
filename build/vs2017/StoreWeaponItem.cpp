#include "StoreWeaponItem.h"
#include <system\debug_log.h>

StoreWeaponItem::StoreWeaponItem(const char* pngFileName, gef::Platform* platform, int newCost, b2World* world, b2Vec2 bodyPos, Weapon weapon){
	icon = CreateTextureFromPNG(pngFileName, *platform);

	cost = newCost;

	this->set_width(64.0f);
	this->set_height(64.0f);

	this->set_texture(icon);

	if (icon == NULL)
	{
		gef::DebugOut("ERROR: Unable to set Weapon Item Icon\n");
	}

	bodyDef.position.Set(bodyPos.x, bodyPos.y);

	body = world->CreateBody(&bodyDef);

	body->SetUserData(this);

	linkedWeapon = weapon;
}

int StoreWeaponItem::getCost()
{
	return cost;
}

PlayerData StoreWeaponItem::run(PlayerData playerData)
{
	if (canPlayerAfford(&playerData) == true)
	{
		playerData.addWeapon(linkedWeapon);
		purchaseSuccessful = true;
		playerData.decrementCredits(cost);
	}
	return playerData;
}

b2Body* StoreWeaponItem::getBody()
{
	return body;
}

bool StoreWeaponItem::didPurchaseSucced()
{
	if (purchaseSuccessful == true)
	{
		purchaseSuccessful = false;
		return true;
	}
	else
	{
		purchaseSuccessful = false;
		return false;
	}
}

char* StoreWeaponItem::getName()
{
	return name;
}

bool StoreWeaponItem::canPlayerAfford(PlayerData* playerData)
{
	if ((playerData->getCredits() - cost) < 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}
