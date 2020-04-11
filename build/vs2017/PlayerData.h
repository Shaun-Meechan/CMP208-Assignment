#pragma once
#include <vector>
#include <Weapon.h>
class PlayerData
{
public:
	int getHealth();
	int getCredits();
	void addCredits(int value);
	void decrementCredits(int value);
	void decrementHealth(float time, int value);
	Weapon getActiveWeapon();
	void addWeapon(Weapon newWeapon);
	void setActiveWeapon(int ID);
	void removeMostRecentWeapon();
	void addHealth(int value);
	void addRiflemen(int value);
	int getRiflemen();
private:
	int credits = 0;
	std::vector<Weapon*> weapons;
	Weapon activeWeapon;
	int health = 100;
	float lastDamageTime = 0.0f;
	int riflemen = 0;
};

