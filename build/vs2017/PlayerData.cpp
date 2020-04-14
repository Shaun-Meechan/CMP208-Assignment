#include "PlayerData.h"
#include <system\debug_log.h>

int PlayerData::getHealth()
{
	return health;
}

int PlayerData::getCredits()
{
	return credits;
}

void PlayerData::addCredits(int value)
{
	credits = credits + value;
}

void PlayerData::decrementCredits(int value)
{
	credits = credits - value;
}

void PlayerData::decrementHealth(float time, int value)
{
	if (lastDamageTime + 0.5f <= time)
	{
		//We last took damge 0.5 seconds ago, deal damage.
		health--;
		lastDamageTime = time;
	}
	return;
}

Weapon PlayerData::getActiveWeapon()
{
	return activeWeapon;
}

void PlayerData::addWeapon(Weapon newWeapon)
{
	//Check all the data before we push it
	if (newWeapon.getAmmo() == NULL)
	{
		gef::DebugOut("ERROR: Weapon has a null ammo count!");
		return;
	}
	else if (newWeapon.getCost() == NULL)
	{
		gef::DebugOut("ERROR: Weapon has a null cost!");
		return;
	}
	else if(newWeapon.getDamage() == NULL)
	{
		gef::DebugOut("ERROR: Weapon has a null damage count!");
		return;
	}
	else if (newWeapon.getIcon() == NULL)
	{
		gef::DebugOut("ERROR: Weapon has a null icon!");
		return;
	}
	else if (newWeapon.getReloadTime() == NULL)
	{
		gef::DebugOut("ERROR: Weapon has a null reload time!");
		return;
	}
	else if (newWeapon.getName() == "")
	{
		gef::DebugOut("ERROR: Weapon has a null name!");
		return;
	}
	weapons.push_back(&newWeapon);
	return;
}

void PlayerData::setActiveWeapon(int ID)
{
	if (weapons[ID] == NULL)
	{
		gef::DebugOut("ERROR: No weapon for the ID exists!");
		return;
	}
	else
	{
		activeWeapon = *weapons[ID];
		return;
	}
}

void PlayerData::removeMostRecentWeapon()
{
	weapons.erase(weapons.end()-1);
}

void PlayerData::addHealth(int value)
{
	if (health + value > 100)
	{
		health = 100;
	}
	else
	{
		health = health + value;
	}
}

void PlayerData::addRiflemen(int value)
{
	riflemen = riflemen + value;
}

int PlayerData::getRiflemen()
{
	return riflemen;
}

void PlayerData::addRepairGuys(int value)
{
	repairGuys = repairGuys + value;
}

int PlayerData::getReapirGuys()
{
	return repairGuys;
}

void PlayerData::setLastDamageTime(float value)
{
	lastDamageTime = value;
}

