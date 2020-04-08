#pragma once
#include <graphics/texture.h>
#include <string>
#include <load_texture.h>
#include <graphics/sprite.h>

using std::string;

namespace gef
{
	class Platform;
}

class Weapon: public gef::Sprite
{
public:
	Weapon();
	gef::Texture* getIcon();
	int getCost();
	int getDamage();
	int getAmmo();
	float getReload();
	string getName();
	void create(const char* pngFileName, gef::Platform* platform, int newCost, int newDamage, int newAmmo, float newReloadTime, string newName);
private:
	gef::Texture* icon = NULL;
	unsigned int cost = 0;
	unsigned int damage = 0;
	unsigned int ammo = 0;
	float reloadTime = 0.0f;
	string name;
};

