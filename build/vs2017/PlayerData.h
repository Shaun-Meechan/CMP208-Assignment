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
	void setActiveWeapon(char* name);
	void removeMostRecentWeapon();
	void addHealth(int value);
	void addRiflemen(int value);
	unsigned short int getRiflemen();
	void addRepairGuys(int value);
	unsigned short int getReapirGuys();
	void setLastDamageTime(float value);
	unsigned int getWeaponsSize();
	bool hasWeapon(char* weaponName);
private:
	int credits = 1300;
	std::vector<Weapon> weapons;
	Weapon activeWeapon;
	int health = 100;
	float lastDamageTime = 0.0f;
	unsigned short int riflemen = 0;
	unsigned short int repairGuys = 0;
};

