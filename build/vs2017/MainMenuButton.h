#pragma once

#include <graphics/sprite.h>
#include <load_texture.h>
#include <box2d/box2d.h>

namespace gef
{
	class platform;
}

enum  class type
{
	Increase,
	Decrease,
	Play
};

class MainMenuButton: public gef::Sprite
{
public:
	MainMenuButton(const char* pngFileName, gef::Platform* platform, std::string newType, b2World* world, b2Vec2 bodyPos);
	unsigned short int run(unsigned short int value);
	b2Body* getBody();
	gef::Texture* getIcon();
private:
	gef::Texture* icon;
	type buttonType;
	b2Body* body;
	b2BodyDef bodyDef;
};

