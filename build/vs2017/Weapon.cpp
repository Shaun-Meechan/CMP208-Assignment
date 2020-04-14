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

float Weapon::getReload()
{
	return reloadTime;
}

string Weapon::getName()
{
	return name;
}

void Weapon::create(const char* pngFileName, gef::Platform* platform, int newCost, int newDamage, int newAmmo, float newReloadTime, char* newName)
{
	icon = CreateTextureFromPNG(pngFileName, *platform);

	cost = newCost;
	damage = newDamage;
	ammo = newAmmo;
	reloadTime = newReloadTime;
	name = newName;

	this->set_height(64.0f);
	this->set_width(64.0f);

	this->set_texture(icon);
}
