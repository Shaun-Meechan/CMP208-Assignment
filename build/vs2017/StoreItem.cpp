#include "StoreItem.h"
#include <system\debug_log.h>

StoreItem::StoreItem(const char* pngFileName, gef::Platform* platform, int newCost, string newType, b2World* world, b2Vec2 bodyPos)
{
	icon = CreateTextureFromPNG(pngFileName, *platform);
	
	cost = newCost;

	this->set_width(64.0f);
	this->set_height(64.0f);

	this->set_texture(icon);

	if (newType == "Health")
	{
		type = itemType::Health;
	}
	else if (newType == "Rifleman")
	{
		type = itemType::Rifleman;
	}
	else if (newType == "RepairGuy")
	{
		type = itemType::RepairGuy;
	}
	else if (newType == "Weapon")
	{
		type == itemType::Weapon;
	}
	else
	{
		gef::DebugOut("ERROR: Unable to set type!");
	}

	if (icon == NULL)
	{
		gef::DebugOut("ERROR: Unable to set store Item Icon");
	}

	bodyDef.position.Set(bodyPos.x, bodyPos.y);

	body = world->CreateBody(&bodyDef);

	//this->UpdateFromSimulation(body);

	body->SetUserData(this);

}

int StoreItem::getCost()
{
	return cost;
}
PlayerData StoreItem::run(PlayerData playerData)
{
	switch (type)
	{
	case itemType::Health:
		if (canPlayerAfford(&playerData) == true)
		{
			playerData.addHealth(10);
		}
		else
		{
			break;
		}
		break;
	case itemType::Rifleman:
		if (canPlayerAfford(&playerData) == true)
		{
			playerData.addRiflemen(1);
		}
		else
		{
			break;
		}
		break;
	case itemType::RepairGuy:
		break;
	case itemType::Weapon:
		break;
	default:
		break;
	}

	return playerData;
}

b2Body* StoreItem::getBody()
{
	return body;
}

bool StoreItem::canPlayerAfford(PlayerData* playerData)
{
	if (playerData->getCredits() - cost < 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

