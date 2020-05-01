#ifndef _SCENE_APP_H
#define _SCENE_APP_H

#include <system/application.h>
#include <maths/vector2.h>
#include <graphics/mesh_instance.h>
#include <input/input_manager.h>
#include <box2d/Box2D.h>
#include <audio/audio_manager.h>
#include "game_object.h"
#include <graphics/sprite.h>
#include "graphics/scene.h"
#include <vector>
#include "EnemyObject.h"
#include <math.h>
#include "PlayerObject.h"
#include "StoreItem.h"
#include "Weapon.h"
#include <string>
#include "PlayerData.h"
#include "WallObject.h"
#include "StoreWeaponItem.h"
#include "primitive_builder.h"
#include "MainMenuButton.h"
// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
	class AudioManager;
}

using std::string;

class SceneApp : public gef::Application
{
public:
	SceneApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
private:
	//void InitPlayer();
	void InitFont();
	void CleanUpFont();
	void DrawHUD();
	void SetupLights();
	void UpdateSimulation(float frame_time);
    
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::InputManager* input_manager_;
	gef::AudioManager* audioManager;

	//
	// FRONTEND DECLARATIONS
	//
	gef::Texture* button_icon_;
	gef::Texture* backgroundSprite;

	//Splash Declarations
	gef::Texture* SplashBackground;
	//
	// GAME DECLARATIONS
	//
	gef::Renderer3D* renderer_3d_;
	PrimitiveBuilder* PB;
	//Game State declarations
	enum GAMESTATE{INIT, Level1, Store, Fail, Win, Splash};
	GAMESTATE gameState = Splash;
	// create the physics world
	b2World* world_;

	float fps_;

	void FrontendInit();
	void FrontendRelease();
	void FrontendUpdate(float frame_time);
	void FrontendRender();

	void GameInit(int enemiesToMake);
	void GameRelease();
	void GameUpdate(float frame_time);
	void GameRender();

	void StoreInit();
	void StoreRelease();
	void StoreUpdate(float frame_time);
	void StoreRender();

	void FailInit();
	void FailRelease();
	void FailUpdate(float frame_time);
	void FailRender();

	void WinInit();
	void WinRelease();
	void WinUpdate(float frame_time);
	void WinRender();

	void SplashInit();
	void SplashRelease();
	void SplashUpdate(float frame_time);
	void SplashRender();

	//Global variables
	bool playAudio = true;
	bool audioStatusChanged = false;
	unsigned short int roundsToBeat = 10;

	//Splash variables
	unsigned short int splashSfx = 0;

	//Main Menu Variables
	std::vector<MainMenuButton*> mainMenuButtons;
	//Game Variables
	unsigned short int roundCounter = 1;
	float lastRfilemenAttackTime = 0.0f;
	float lastRepairTime = 0.0f;
	gef::Texture* gameBackgroundSprite;
	bool firstRun = true;
	gef::Vector2 touchPosition;
	Int32 activeTouchID = 0;
	unsigned short int gunShotSampleID = 0;
	unsigned short int backgroundSFXID = 0;
	unsigned short int reloadSfx = 0;
	std::vector <EnemyObject*> enemies;
	PlayerObject* Player;
	WallObject* wallObject;
	gef::Scene* enemySceneAsset;
	gef::Scene* playerSceneAsset;
	gef::Scene* wallSceneAsset;
	Weapon activeWeapon;
	float gameTime;
	PlayerData playerData;
	//Used for 2D -> 3D projection
	float ndc_z_min_;
	//Game functions
	void ProcessTouchInput();
	gef::Scene* LoadSceneAssets(gef::Platform& platform, const char* filename);
	gef::Mesh* getMeshFromSceneAssets(gef::Scene* scene);
	void GetScreenPosRay(const gef::Vector2& screen_position, const gef::Matrix44& projection, const gef::Matrix44& view, gef::Vector4& startPoint, gef::Vector4& direction);
	bool RaySphereIntersect(gef::Vector4& startPoint, gef::Vector4& direction, gef::Vector4& sphere_centre, float sphere_radius);

	//Store Variables
	std::vector<StoreItem*> storeItem;
	std::vector<StoreWeaponItem*> storeWeapons;
	Weapon sniper = Weapon();
	Weapon assualtRifle = Weapon();
	Weapon shotgun = Weapon();
	unsigned short int purchaseSfx = 0;
	unsigned short int purchasefailSFX = 0;
	gef::Texture* selectedWeaponTexture;
	// Global Functions
	void updateStateMachine(int newID, int oldID);
};

#endif // _SCENE_APP_H
