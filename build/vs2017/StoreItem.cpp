#include "StoreItem.h"
#include <system\debug_log.h>

StoreItem::StoreItem(const char* pngFileName, gef::Platform* platform, int newCost)
{
	icon = CreateTextureFromPNG(pngFileName, *platform);
	
	cost = newCost;

	this->set_width(64.0f);
	this->set_height(64.0f);

	this->set_texture(icon);

	if (icon == NULL)
	{
		gef::DebugOut("ERROR: Unable to set store Item Icon");
	}
}

int StoreItem::getCost()
{
	return cost;
}
