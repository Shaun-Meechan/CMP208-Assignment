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
	float getReloadTime();
	string getName();
	void create(const char* pngFileName, gef::Platform* platform, int newCost, int newDamage, int newMaxAmmo, float newReloadTime, char* newName);
	void decrementAmmo(int value);
	float getRanOutOfAmmoTime();
	void setRanOutOfAmmoTime(float newTime);
	int getMaxAmmo();
	void setAmmo(int value);
private:
	gef::Texture* icon = NULL;
	unsigned int cost = 0;
	unsigned int damage = 0;
	unsigned int ammo = 0;
	float reloadTime = 0.0f;
	float ranOutOfAmmoTime = 0.0f;
	int maxAmmo = 0;
	char* name;
};

