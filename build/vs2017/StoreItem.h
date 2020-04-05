#pragma once

#include <graphics/sprite.h>
#include <load_texture.h>

namespace gef
{
	class Platform;
}
class StoreItem: public gef::Sprite
{
public:
	StoreItem(const char* pngFileName, gef::Platform* platform, int newCost);
	int getCost();
private:
	gef::Texture* icon;
	unsigned int cost;
};

