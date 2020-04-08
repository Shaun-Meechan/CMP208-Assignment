#include "Weapon.h"

Weapon::Weapon(const char* pngFileName, gef::Platform* platform, int newCost, int newDamage, int newAmmo, float newReloadTime, string newName)
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
