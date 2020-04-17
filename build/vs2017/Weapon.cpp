#include "Weapon.h"

Weapon::Weapon()
{
	cost = 0;
	damage = 0;
	ammo = 0;
	reloadTime = 0.0f;
	name = "";
}

gef::Texture* Weapon::getIcon()
{
	return icon;
}

int Weapon::getCost()
{
	return cost;
}

int Weapon::getDamage()
{
	return damage;
}

int Weapon::getAmmo()
{
	return ammo;
}

float Weapon::getReloadTime()
{
	return reloadTime;
}

string Weapon::getName()
{
	return name;
}

void Weapon::create(const char* pngFileName, gef::Platform* platform, int newCost, int newDamage, int newMaxAmmo, float newReloadTime, char* newName, char* newSfxPath)
{
	icon = CreateTextureFromPNG(pngFileName, *platform);

	cost = newCost;
	damage = newDamage;
	maxAmmo = newMaxAmmo;
	ammo = maxAmmo;
	reloadTime = newReloadTime;
	name = newName;
	sfxPath = newSfxPath;

	this->set_height(64.0f);
	this->set_width(64.0f);

	this->set_texture(icon);
}

void Weapon::decrementAmmo(int value)
{
	ammo = ammo - value;
}

float Weapon::getRanOutOfAmmoTime()
{
	return ranOutOfAmmoTime;
}

void Weapon::setRanOutOfAmmoTime(float newTime)
{
	ranOutOfAmmoTime = newTime;
}

int Weapon::getMaxAmmo()
{
	return maxAmmo;
}

void Weapon::setAmmo(int value)
{
	ammo = value;
}

char* Weapon::getSfxPath()
{
	return sfxPath;
}
