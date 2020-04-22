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
		name = "Health";
	}
	else if (newType == "Rifleman")
	{
		type = itemType::Rifleman;
		name = "Rifleman";
	}
	else if (newType == "RepairGuy")
	{
		type = itemType::RepairGuy;
		name = "Repair Guy";
	}
	else if (newType == "Weapon")
	{
		type = itemType::Weapon;
	}
	else
	{
		gef::DebugOut("ERROR: Unable to set type!\n");
	}

	if (icon == NULL)
	{
		gef::DebugOut("ERROR: Unable to set store Item Icon\n");
	}

	bodyDef.position.Set(bodyPos.x, bodyPos.y);

	body = world->CreateBody(&bodyDef);

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
			purchaseSuccessful = true;
			playerData.decrementCredits(cost);
			break;
		}
		else
		{
			break;
		}
	case itemType::Rifleman:
		if (canPlayerAfford(&playerData) == true)
		{
			playerData.addRiflemen(1);
			purchaseSuccessful = true;
			playerData.decrementCredits(cost);
			break;
		}
		else
		{
			break;
		}
	case itemType::RepairGuy:
		if (canPlayerAfford(&playerData) == true)
		{
			playerData.addRepairGuys(1);
			purchaseSuccessful = true;
			playerData.decrementCredits(cost);
			break;
		}
		else
		{
			break;
		}
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

//Check to see if the purchase was successful.
bool StoreItem::didPurchaseSucced()
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

char* StoreItem::getName()
{
	return name;
}

gef::Texture* StoreItem::getIcon()
{
	return icon;
}

bool StoreItem::canPlayerAfford(PlayerData* playerData)
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