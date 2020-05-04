#include "MainMenuButton.h"
#include <system\debug_log.h>

MainMenuButton::MainMenuButton(const char* pngFileName, gef::Platform* platform, std::string newType, b2World* world, b2Vec2 bodyPos)
{
	icon = CreateTextureFromPNG(pngFileName, *platform);

	this->set_width(64.0f);
	this->set_height(64.0f);

	this->set_texture(icon);

	if (newType == "Increase")
	{
		buttonType = type::Increase;
	}
	else if (newType == "Decrease")
	{
		buttonType = type::Decrease;
	}
	else if (newType == "Play")
	{
		buttonType = type::Play;
	}
	else
	{
		gef::DebugOut("ERROR: Unable to set main menu button type!\n");
	}

	bodyDef.position.Set(bodyPos.x, bodyPos.y);

	body = world->CreateBody(&bodyDef);

	body->SetUserData(this);
}

unsigned short int MainMenuButton::run(unsigned short int value)
{

	int number = value;

	if (buttonType == type::Increase)
	{
		number += 1;
		return number;
	}
	else if (buttonType == type::Decrease)
	{
		if (value == 1)
		{
			return value;
		}
		else
		{
			number -= 1;
			return number;
		}
	}
	else if (buttonType == type::Play)
	{
		number = 0;
		return number;
	}
	else
	{
		gef::DebugOut("ERROR: Unable to run main menu button\n");
	}
}

b2Body* MainMenuButton::getBody()
{
	return body;
}

gef::Texture* MainMenuButton::getIcon()
{
	return icon;
}
