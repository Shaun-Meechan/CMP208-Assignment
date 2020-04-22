#include "scene_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include <maths/math_utils.h>
#include <input/sony_controller_input_manager.h>
#include <input/keyboard.h>
#include <input/touch_input_manager.h>
#include <graphics/sprite.h>
#include "load_texture.h"
#include <iostream>

SceneApp::SceneApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	renderer_3d_(NULL),
	input_manager_(NULL),
	font_(NULL),
	world_(NULL),
	button_icon_(NULL),
	backgroundSprite(NULL),
	audioManager(NULL),
	activeTouchID(-1),
	enemySceneAsset(NULL),
	playerSceneAsset(NULL)
{
}

void SceneApp::Init()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	InitFont();

	// initialise input manager
	input_manager_ = gef::InputManager::Create(platform_);

	// Initialise our audio manager
	audioManager = gef::AudioManager::Create();

	FrontendInit();

	//Seed a new seed for the random number generator
	srand(time(NULL));
}

void SceneApp::CleanUp()
{
	delete input_manager_;
	input_manager_ = NULL;

	CleanUpFont();

	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	audioManager->UnloadAllSamples();
	audioManager->UnloadMusic();

	delete audioManager;
	audioManager = NULL;
}

bool SceneApp::Update(float frame_time)
{
	audioStatusChanged = false;

	fps_ = 1.0f / frame_time;

	input_manager_->Update();

	gef::Keyboard* keyboard = input_manager_->keyboard();
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);

	if (keyboard)
	{
		if (keyboard->IsKeyPressed(gef::Keyboard::KC_RETURN))
		{
			switch (gameState)
			{
			case SceneApp::INIT:
				updateStateMachine(1,0);
				break;
			case SceneApp::Store:
				updateStateMachine(1,2);
				break;
			case SceneApp::Fail:
				updateStateMachine(1, 3);
				break;
			case SceneApp::Win:
				updateStateMachine(0, 4);
				break;
			default:
				break;
			}
		}

		if (keyboard->IsKeyPressed(gef::Keyboard::KC_M))
		{
			switch (playAudio)
			{
			case true:
				playAudio = false;
				audioManager->StopMusic();
				break;
			case false:
				playAudio = true;
				break;
			default:
				break;
			}
			audioStatusChanged = true;
		}
	}

	switch (gameState)
	{
	case SceneApp::INIT:
		FrontendUpdate(frame_time);
		break;
	case SceneApp::Level1:
		GameUpdate(frame_time);
		break;
	case SceneApp::Store:
		StoreUpdate(frame_time);
		break;
	case SceneApp::Fail:
		FailUpdate(frame_time);
		break;
	case SceneApp::Win:
		WinUpdate(frame_time);
		break;
	default:
		break;
	}

	return true;
}

void SceneApp::Render()
{
	switch (gameState)
	{
	case SceneApp::INIT:
		FrontendRender();
		break;
	case SceneApp::Level1:
		GameRender();
		break;
	case SceneApp::Store:
		StoreRender();
		break;
	case SceneApp::Fail:
		FailRender();
		break;
	case SceneApp::Win:
		WinRender();
		break;
	default:
		break;
	}
}

void SceneApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void SceneApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void SceneApp::DrawHUD()
{
	if(font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
	}
}

void SceneApp::SetupLights()
{
	// grab the data for the default shader used for rendering 3D geometry
	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();

	// set the ambient light
	default_shader_data.set_ambient_light_colour(gef::Colour(0.25f, 0.25f, 0.25f, 1.0f));

	// add a point light that is almost white, but with a blue tinge
	// the position of the light is set far away so it acts light a directional light
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-500.0f, 400.0f, 700.0f));
	default_shader_data.AddPointLight(default_point_light);
}

void SceneApp::UpdateSimulation(float frame_time)
{
	// update physics world
	float timeStep = 1.0f / 60.f;

	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	world_->Step(timeStep, velocityIterations, positionIterations);

	// update object visuals from simulation data
	Player->UpdateFromSimulation(Player->getBody());

	for (int i = 0; i < enemies.size(); i++)
	{
		enemies[i]->UpdateFromSimulation(enemies[i]->getBody());
	}

	wallObject->UpdateFromSimulation(wallObject->getBody());

	// collision detection
	// get the head of the contact list
	b2Contact* contact = world_->GetContactList();
	// get contact count
	int contact_count = world_->GetContactCount();

	for (int contact_num = 0; contact_num<contact_count; ++contact_num)
	{
		if (contact->IsTouching())
		{
			// get the colliding bodies
			b2Body* bodyA = contact->GetFixtureA()->GetBody();
			b2Body* bodyB = contact->GetFixtureB()->GetBody();

			// DO COLLISION RESPONSE HERE
			PlayerObject* player = NULL;
			Enemy* enemy = NULL;
			GameObject* gameObjectA = NULL;
			GameObject* gameObjectB = NULL;

			gameObjectA = (GameObject*)bodyA->GetUserData();
			gameObjectB = (GameObject*)bodyB->GetUserData();

			if (gameObjectA)
			{
				if (gameObjectA->type() == PLAYER)
				{
					player = (PlayerObject*)bodyA->GetUserData();
				}
				else if (gameObjectA->type() == ENEMY)
				{
					enemy = (Enemy*)bodyA->GetUserData();
				}
			}

			if (gameObjectB)
			{
				if (gameObjectB->type() == PLAYER)
				{
					player = (PlayerObject*)bodyB->GetUserData();
				}
				else if (gameObjectB->type() == ENEMY)
				{
					enemy = (Enemy*)bodyB->GetUserData();
				}
			}

			if (player && enemy)
			{
				gef::DebugOut("Player and enemy collision!\n");

				playerData.decrementHealth(gameTime,1);

				if (gameObjectA->type() == ENEMY)
				{
					bodyA->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
				}
				else if (gameObjectB->type() == ENEMY)
				{
					bodyB->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
				}
			}

			if (enemy)
			{
				if (gameObjectA->type() == ENEMY && gameObjectB->type() == ENEMY)
				{
					gef::DebugOut("Two enemies have collided!\n");
					bodyA->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
					bodyB->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
				}
			}
		}
		// Get next contact point
		contact = contact->GetNext();
	}
}

void SceneApp::FrontendInit()
{
	button_icon_ = CreateTextureFromPNG("playbuttonWhite.png", platform_);
	backgroundSprite = CreateTextureFromPNG("mainMenuBackground.png", platform_);
	audioManager->LoadMusic("MainMenuMusic.wav", platform_);

	if (playAudio == true)
	{
		audioManager->PlayMusic();
	}
}

void SceneApp::FrontendRelease()
{
	button_icon_->~Texture();
	delete button_icon_;
	button_icon_ = NULL;

	backgroundSprite->~Texture();
	delete backgroundSprite;
	backgroundSprite = NULL;

	audioManager->StopMusic();
	audioManager->UnloadMusic();
}

void SceneApp::FrontendUpdate(float frame_time)
{
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);

	if (audioStatusChanged == true)
	{
		if (playAudio == true)
		{
			audioManager->PlayMusic();
		}
	}
}

void SceneApp::FrontendRender()
{
	sprite_renderer_->Begin();

	//Render our background 
	gef::Sprite background;
	background.set_texture(backgroundSprite);
	background.set_position(gef::Vector4(platform_.width() - 480.f, platform_.height() - 273.f, 0.f));
	background.set_height(platform_.height());
	background.set_width(platform_.width());
	sprite_renderer_->DrawSprite(background);

	// Render Title Text
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f - 270.f , 0.f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"Save The Home!");

	//Render audio text
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f + 240.f, 0.f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"Press 'm' at any time to mute/unmute audio.");

	// Render button icon
	gef::Sprite button;
	button.set_texture(button_icon_);
	button.set_position(gef::Vector4(platform_.width()*0.5f, platform_.height()*0.5f, 0.f));
	button.set_height(128.0f);
	button.set_width(128.0f);
	sprite_renderer_->DrawSprite(button);

	DrawHUD();
	sprite_renderer_->End();
}

void SceneApp::GameInit(int enemiesToMake)
{
	const char* sceneAssetFilename;
	// initialise the physics world
	b2Vec2 gravity(0.0f,0.0f);
	world_ = new b2World(gravity);

	//Initalize our time variable
	gameTime = 0;

	//Reset our player damage time
	playerData.setLastDamageTime(0.0f);
	//Reset our rifleman attack time
	lastRfilemenAttackTime = 0;
	//Rest our repair time
	lastRepairTime = 0;
	// Make sure there is a panel to detect touch, activate if it exists
	if (input_manager_ && input_manager_->touch_manager() && (input_manager_->touch_manager()->max_num_panels() > 0))
	{
		input_manager_->touch_manager()->EnablePanel(0);
	}

	//Create the first weapons if it's not been made yet.
	if (firstRun == true)
	{
		Weapon handgun = Weapon();
		handgun.create("handgun.png", &platform_, 100, 30, 10, 2.5f, "Handgun","handgunSfx.wav");
		playerData.addWeapon(handgun);
		playerData.setActiveWeapon("Handgun");
		firstRun = false;
	}

	sceneAssetFilename = "NewHouse.scn";
	playerSceneAsset = LoadSceneAssets(platform_, sceneAssetFilename);
	if (!playerSceneAsset)
	{
		gef::DebugOut("Failed to load player scene file. %s", sceneAssetFilename);
	}

	//Load our enemy asset
	sceneAssetFilename = "stickman.scn";
	enemySceneAsset = LoadSceneAssets(platform_, sceneAssetFilename);
	if (!enemySceneAsset)
	{
		gef::DebugOut("Failed to load enemy scene file. %s", sceneAssetFilename);
	}

	sceneAssetFilename = "wall.scn";
	wallSceneAsset = LoadSceneAssets(platform_, sceneAssetFilename);
	if (!wallSceneAsset)
	{
		gef::DebugOut("ERROR: Failed to load wall scene file, %s", sceneAssetFilename);
	}

	// create the renderer for draw 3D geometry
	renderer_3d_ = gef::Renderer3D::Create(platform_);

	//Load our audio samples
	gunShotSampleID = audioManager->LoadSample(playerData.getActiveWeapon().getSfxPath(), platform_);
	backgroundSFXID = audioManager->LoadMusic("gamebackgroundsfx.wav", platform_);
	reloadSfx = audioManager->LoadSample("ReloadSfx.wav", platform_);

	//start our background sfx
	if (playAudio == true)
	{
		audioManager->PlayMusic();
	}

	SetupLights();

	//Setup player
	Player = new PlayerObject(playerSceneAsset, world_);
	Player->updateScale(gef::Vector4(0.1f, 0.2f, 0.1f));
	Player->updateRotationY(80);

	//Setup wall
	wallObject = new WallObject(wallSceneAsset, world_);
	wallObject->updateScale(gef::Vector4(0.55f, 0.1f, 0.1f));
	wallObject->updateRotationZ(90);

	gef::Mesh* enemyMesh = getMeshFromSceneAssets(enemySceneAsset);

	for (unsigned int i = 0; i < enemiesToMake; i++)
	{
		enemies.push_back(new EnemyObject(enemySceneAsset,world_,-10.0f - (i), enemyMesh));
	}

	//Move alive enemeies
	for (int i = 0; i < enemies.size(); i++)
	{
		enemies[i]->getBody()->ApplyForceToCenter(b2Vec2(5, 0), true);
	}

	activeWeapon = playerData.getActiveWeapon();

	gameBackgroundSprite = CreateTextureFromPNG("groundSprite.png", platform_);
}

void SceneApp::GameRelease()
{
	// destroying the physics world also destroys all the objects within it
	delete world_;
	world_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete enemySceneAsset;
	enemySceneAsset = NULL;

	delete playerSceneAsset;
	playerSceneAsset = NULL;

	delete wallSceneAsset;
	wallSceneAsset = NULL;

	delete Player;
	Player = NULL;

	delete wallObject;
	wallObject = NULL;

	gameBackgroundSprite->~Texture();
	delete gameBackgroundSprite;
	gameBackgroundSprite = NULL;

	//We really shouldn't need to run this code or the claer. This is just a failsafe in case the user somehow finishes the level without killing all enemies.
	for (unsigned int i = 0; i < enemies.size(); i++)
	{
		delete enemies[i];
	}
	enemies.clear();


	enemies.shrink_to_fit();

	gameTime = 0;

	//Audio unload
	audioManager->StopMusic();
	audioManager->UnloadMusic();
	audioManager->StopPlayingSampleVoice(gunShotSampleID);
	audioManager->StopPlayingSampleVoice(backgroundSFXID);
	audioManager->UnloadAllSamples();
	gunShotSampleID = NULL;
	backgroundSFXID = NULL;
	reloadSfx = NULL;
	roundCounter += 1;
}

void SceneApp::GameUpdate(float frame_time)
{
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);

	gameTime = gameTime + frame_time;
	Player->update();
	wallObject->update();

	//check all the alive enemies to see if they need to be killed
	for (int i = 0; i < enemies.size(); i++)
	{
		enemies[i]->update();
		if (enemies[i]->getHealth() <= 0)
		{
			world_->DestroyBody(enemies[i]->getBody());
			delete enemies[i];
			enemies.erase(enemies.begin() + i);//Remove the now dead enemy
			playerData.addCredits(10);
		}
	}

	//Use any riflemen the player has
	if (lastRfilemenAttackTime + 2 <= gameTime)
	{
		for (int i = 0; i < playerData.getRiflemen(); i++)
		{
			if (i < enemies.size())
			{
				enemies[i]->decrementHealth(5);
				audioManager->PlaySample(gunShotSampleID);
			}
		}
		lastRfilemenAttackTime = gameTime;
	}

	//Use any Repair Guys the player has
	if (lastRepairTime + 5 <= gameTime)
	{
		for (int i = 0; i < playerData.getReapirGuys(); i++)
		{
			playerData.addHealth(2);
		}
		lastRepairTime = gameTime;
	}

	ProcessTouchInput();

	if (activeWeapon.getAmmo() <= 0)
	{
		if (gameTime >= activeWeapon.getReloadTime() + activeWeapon.getRanOutOfAmmoTime())
		{
			activeWeapon.setAmmo(activeWeapon.getMaxAmmo());
		}
	}

	UpdateSimulation(frame_time);

	if (enemies.size() == 0)
	{
		updateStateMachine(2,1);
		return;
	}

	if (playerData.getHealth() <= 0)
	{
		updateStateMachine(3, 1);
		return;
	}

	if (audioStatusChanged == true)
	{
		if (playAudio == true)
		{
			audioManager->PlayMusic();
		}
	}
}

void SceneApp::GameRender()
{
	// setup camera

	// projection
	float fov = gef::DegToRad(45.0f);
	float aspect_ratio = (float)platform_.width() / (float)platform_.height();
	gef::Matrix44 projection_matrix;
	projection_matrix = platform_.PerspectiveProjectionFov(fov, aspect_ratio, 0.1f, 100.0f);
	renderer_3d_->set_projection_matrix(projection_matrix);

	// view
	gef::Vector4 camera_eye(-2.0f, 2.0f, 15.0f);
	gef::Vector4 camera_lookat(0.0f, 0.0f, 0.0f);
	gef::Vector4 camera_up(0.0f, 1.0f, 0.0f);
	gef::Matrix44 view_matrix;
	view_matrix.LookAt(camera_eye, camera_lookat, camera_up);
	renderer_3d_->set_view_matrix(view_matrix);

	// draw 3d geometry
	renderer_3d_->Begin();

	// draw player
	Player->render(renderer_3d_);	

	//Draw enemy
	for (int i = 0; i < enemies.size(); i++)
	{
		enemies[i]->render(renderer_3d_);
	}

	wallObject->render(renderer_3d_);

	renderer_3d_->End();

	// start drawing sprites, but don't clear the frame buffer
	sprite_renderer_->Begin(false);

	DrawHUD();

	// Render Title Text
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f + 400.0f, platform_.height() * 0.5f - 270.f, 0.f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"Health %i", playerData.getHealth());

	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f + 400.0f, platform_.height() * 0.5f - 250.0f, 0.0f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"Credits: %i", playerData.getCredits());

	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f + 400.0f, platform_.height() * 0.5f - 230.0f, 0.0f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"Riflemen: %i", playerData.getRiflemen());

	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f + 400.0f, platform_.height() * 0.5f - 210.0f, 0.0f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"RepairGuys: %i", playerData.getReapirGuys());

	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.15f, platform_.height() * 0.05f, 0.0f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"Ammo count: %i", activeWeapon.getAmmo());

	activeWeapon.set_position(gef::Vector4(platform_.width() * 0.03f, platform_.height() * 0.05f , 0));
	sprite_renderer_->DrawSprite(activeWeapon);

	gef::Sprite background;
	background.set_texture(gameBackgroundSprite);
	background.set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f, 1.0f));
	background.set_height(platform_.height());
	background.set_width(platform_.width());
	sprite_renderer_->DrawSprite(background);

	sprite_renderer_->End();
}

void SceneApp::StoreInit()
{
	b2Vec2 gravity(0.0f, 0.0f);
	world_ = new b2World(gravity);
	// create the renderer for draw 3D geometry
	renderer_3d_ = gef::Renderer3D::Create(platform_);

	purchaseSfx = audioManager->LoadSample("purchasemade.wav", platform_);
	purchasefailSFX = audioManager->LoadSample("purchasefail.wav", platform_);

	audioManager->LoadMusic("StoreMusic.wav", platform_);
	if (playAudio == true)
	{
		audioManager->PlayMusic();
	}

	//Healthpack
	storeItem.push_back(new StoreItem("healthpackicon.png", &platform_, 50, "Health", world_, b2Vec2(-9,5)));
	storeItem[0]->set_position(gef::Vector4(platform_.width() * 0.05f, platform_.height() * 0.1f,0));
	
	//Rifeman
	storeItem.push_back(new StoreItem("on-sight.png", &platform_, 100, "Rifleman", world_, b2Vec2(-9, 2.5f)));
	storeItem[1]->set_position(gef::Vector4(platform_.width() * 0.05f, platform_.height() * 0.3f,0));

	//Repair guy
	storeItem.push_back(new StoreItem("hammer-nails.png", &platform_, 100, "RepairGuy", world_, b2Vec2(-9, 0.0f)));
	storeItem[2]->set_position(gef::Vector4(platform_.width() * 0.05f, platform_.height() * 0.5f,0));

	//Weapons
	//Sniper
	Weapon sniper = Weapon();
	sniper.create("sniper_icon_2.png", &platform_, 250, 40, 1, 1.0f, "Sniper","sniperSfx.wav");
	storeWeapons.push_back(new StoreWeaponItem("sniper_icon_2.png", &platform_, 250, world_, b2Vec2(0, 5),sniper));
	storeWeapons[0]->set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.1, 0));
	sniper.getIcon()->~Texture();
	//Assault rifle
	Weapon assualtRifle = Weapon();
	assualtRifle.create("assault_rifle_icon_1.png", &platform_, 200, 20, 25, 3.0f, "AssaultRifle", "AssaultRifleSfx.wav");
	storeWeapons.push_back(new StoreWeaponItem("assault_rifle_icon_1.png", &platform_, 200, world_, b2Vec2(4, 5), assualtRifle));
	storeWeapons[1]->set_position(gef::Vector4(platform_.width() * 0.7f, platform_.height() * 0.1, 0));
	assualtRifle.getIcon()->~Texture();
	//Shotgun
	Weapon shotgun = Weapon();
	shotgun.create("shotgun_icon_2.png", &platform_, 300, 50, 2, 1.5f, "shotgun", "shotgunSfx.wav");
	storeWeapons.push_back(new StoreWeaponItem("shotgun_icon_2.png", &platform_, 300, world_, b2Vec2(0, 2.25), shotgun));
	storeWeapons[2]->set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.3, 0));
	shotgun.getIcon()->~Texture();

	selectedWeaponTexture = CreateTextureFromPNG("SelectedWeaponSprite.png", platform_);
}

void SceneApp::StoreRelease()
{
	for (unsigned int i = 0; i < storeItem.size(); i++)
	{
		storeItem[i]->getIcon()->~Texture();
		delete storeItem[i];
	}

	for (unsigned int i = 0; i < storeWeapons.size(); i++)
	{
		storeWeapons[i]->getIcon()->~Texture();
		delete storeWeapons[i];
	}

	selectedWeaponTexture->~Texture();

	storeItem.clear();
	storeItem.shrink_to_fit();

	storeWeapons.clear();
	storeWeapons.shrink_to_fit();

	audioManager->StopMusic();
	audioManager->StopPlayingSampleVoice(purchaseSfx);
	audioManager->StopPlayingSampleVoice(purchasefailSFX);
	audioManager->UnloadMusic();
	audioManager->UnloadAllSamples();

	purchaseSfx = NULL;
	purchasefailSFX = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete world_;
	world_ = NULL;
}

void SceneApp::StoreUpdate(float frame_time)
{
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);

	ProcessTouchInput();

	if (audioStatusChanged == true)
	{
		if (playAudio == true)
		{
			audioManager->PlayMusic();
		}
	}
}

void SceneApp::StoreRender()
{
	// projection
	float fov = gef::DegToRad(45.0f);
	float aspect_ratio = (float)platform_.width() / (float)platform_.height();
	gef::Matrix44 projection_matrix;
	projection_matrix = platform_.PerspectiveProjectionFov(fov, aspect_ratio, 0.1f, 100.0f);
	renderer_3d_->set_projection_matrix(projection_matrix);

	// view
	gef::Vector4 camera_eye(-2.0f, 2.0f, 15.0f);
	gef::Vector4 camera_lookat(0.0f, 0.0f, 0.0f);
	gef::Vector4 camera_up(0.0f, 1.0f, 0.0f);
	gef::Matrix44 view_matrix;
	view_matrix.LookAt(camera_eye, camera_lookat, camera_up);
	renderer_3d_->set_view_matrix(view_matrix);

	sprite_renderer_->Begin();

	for (int i = 0; i < storeItem.size(); i++)
	{
		sprite_renderer_->DrawSprite(*storeItem[i]);

		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(storeItem[i]->position().x(), storeItem[i]->position().y() + 25.0f, 0.0f),
			1.0f,
			0xffffffff,
			gef::TJ_CENTRE,
			"%i", storeItem[i]->getCost());

		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(storeItem[i]->position().x() + 90.0f, storeItem[i]->position().y(), 0.0f),
			1.0f,
			0xffffffff,
			gef::TJ_CENTRE,
			storeItem[i]->getName());
	}

	//Draw weapons
	for (int i = 0; i < storeWeapons.size(); i++)
	{
		sprite_renderer_->DrawSprite(*storeWeapons[i]);

		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(storeWeapons[i]->position().x(), storeWeapons[i]->position().y(),0.0f),
			1.0f,
			0xffffffff,
			gef::TJ_CENTRE,
			"%i", storeWeapons[i]->getCost());
	}

	//Draw our weapon selector icon
	if (playerData.getActiveWeapon().getName() != "Handgun")
	{
		gef::Sprite selectedWeaponSprite;
		selectedWeaponSprite.set_texture(selectedWeaponTexture);
		if (playerData.getActiveWeapon().getName() == "Sniper")
		{
			selectedWeaponSprite.set_position(gef::Vector4(storeWeapons[0]->position().x(), storeWeapons[0]->position().y(), 0.0f));
		}
		else if (playerData.getActiveWeapon().getName() == "AssaultRife")
		{
			selectedWeaponSprite.set_position(gef::Vector4(storeWeapons[1]->position().x(), storeWeapons[1]->position().y(), 0.0f));
		}
		else if (playerData.getActiveWeapon().getName() == "shotgun")
		{
			selectedWeaponSprite.set_position(gef::Vector4(storeWeapons[2]->position().x(), storeWeapons[2]->position().y(), 0.0f));
		}
		else
		{
			gef::DebugOut("ERROR: Unable to get player active weapon and set selected weapon sprite position!\n");
		}
		selectedWeaponSprite.set_height(64.0f);
		selectedWeaponSprite.set_width(64.0f);
		sprite_renderer_->DrawSprite(selectedWeaponSprite);
	}

	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.9f, platform_.height() * 0.01, 0.0f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"Health: %i", playerData.getHealth());

	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width()* 0.9f, platform_.height() * 0.05f, 0.0f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"Credits: %i", playerData.getCredits());

	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.9f, platform_.height() * 0.09f, 0.0f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"Riflemen: %i", playerData.getRiflemen());

	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.9f, platform_.height() * 0.13f, 0.0f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"RepairGuys: %i", playerData.getReapirGuys());


	sprite_renderer_->End();
}

void SceneApp::FailInit()
{
}

void SceneApp::FailRelease()
{
}

void SceneApp::FailUpdate(float frame_time)
{
}

void SceneApp::FailRender()
{
	sprite_renderer_->Begin();

	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f, 0.0f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"You have been defeated, your house is yours no longer.");

	sprite_renderer_->End();
}

void SceneApp::WinInit()
{
}

void SceneApp::WinRelease()
{
}

void SceneApp::WinUpdate(float frame_time)
{
}

void SceneApp::WinRender()
{
	sprite_renderer_->Begin();

	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f, 0.0f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"Victory! Your house is now safe.");

	sprite_renderer_->End();
}

//New ID represent where we want to go. Old represent where we came from.
void SceneApp::updateStateMachine(int newID, int oldID)
{
	switch (newID)
	{
	case 0://Front end
		WinRelease();
		FrontendInit();
		gameState = INIT;
		break;
	case 1://Game
		if (oldID == 0)
		{
			FrontendRelease();
		}
		if (oldID == 2)
		{
			StoreRelease();
		}
		if (oldID == 3)
		{
			FailRelease();
		}
		GameInit(roundCounter * 2);
		gameState = Level1;
		break;
	case 2://Store
		GameRelease();
		StoreInit();
		gameState = Store;
		break;
	case 3://Fail
		GameRelease();
		FailInit();
		gameState = Fail;
		break;
	case 4://Win
		GameRelease();
		WinInit();
		gameState = Win;
		break;
	default:
		break;
	}
	return;
}

void SceneApp::ProcessTouchInput()
{
	const gef::TouchInputManager* touchInput = input_manager_->touch_manager();

	if (touchInput && (touchInput->max_num_panels() > 0))
	{
		//Get the active touches for this panel
		const gef::TouchContainer& panelTouches = touchInput->touches(0);

		//Go through the touches
		for (gef::ConstTouchIterator touch = panelTouches.begin(); touch != panelTouches.end(); ++touch )
		{
			//If active touch ID is -1, then we are not currently processing a touch
			if (activeTouchID == -1)
			{
				//Check for the start of a new touch
				if (touch->type == gef::TT_NEW)
				{
					activeTouchID = touch->id;

					//Do any processing for a new touch here

					// convert the touch position to a ray that starts on the camera near plane
					// and shoots into the camera view frustum
					gef::Vector2 screen_position = touch->position;
					gef::Vector4 ray_start_position, ray_direction;
					GetScreenPosRay(screen_position, renderer_3d_->projection_matrix(), renderer_3d_->view_matrix(), ray_start_position, ray_direction);
					
					switch (gameState)
					{
					case SceneApp::Level1:
						if (activeWeapon.getAmmo() <= 0)
						{
							break;
						}
						activeWeapon.decrementAmmo(1);

						if (activeWeapon.getAmmo() <= 0)
						{
							activeWeapon.setRanOutOfAmmoTime(gameTime);
							if (playAudio == true)
							{
								audioManager->PlaySample(reloadSfx, false);
							}
						}
						//Here we need to loop through all the enemy bodies and see if the player hits them.
						for (int i = 0; i < enemies.size(); i++)
						{
							if (enemies[i]->getBody())
							{
								// Create a sphere around the position of the enemy body
								// the radius can be changed for larger objects
								// radius= 0.5f is a sensible value for a 1x1x1 cube
								gef::Vector4 sphere_centre(enemies[i]->getBody()->GetPosition().x, enemies[i]->getBody()->GetPosition().y, 0.0f);
								float  sphere_radius = 0.9f;

								// check to see if the ray intersects with the bound sphere that is around the player
								if (RaySphereIntersect(ray_start_position, ray_direction, sphere_centre, sphere_radius))
								{
									//Player touched an enemy do something
									enemies[i]->decrementHealth(activeWeapon.getDamage()); //Lower this by the damage of the current weapon
								}
							}
						}
						if (playAudio == true)
						{
							audioManager->PlaySample(gunShotSampleID, false);
						}
						break;
					case SceneApp::Store:
						//Here we need to loop through all the store items and see if the player interacts with them.
						for (int i = 0; i < storeItem.size(); i++)
						{
							if (storeItem[i])
							{
								gef::Vector4 sphere_centre(storeItem[i]->getBody()->GetPosition().x, storeItem[i]->getBody()->GetPosition().y, 0.0f);
								float  sphere_radius = 1.0f;

								// check to see if the ray intersects with the bound sphere that is around the player
								if (RaySphereIntersect(ray_start_position, ray_direction, sphere_centre, sphere_radius))
								{
									//Player touched an enemy do something
									playerData = storeItem[i]->run(playerData); //Lower this by the damage of the current weapon
									if (storeItem[i]->didPurchaseSucced() == true)
									{
										if (playAudio == true)
										{
											audioManager->PlaySample(purchaseSfx,false);
										}
									}
									else
									{
										if (playAudio == true)
										{
											audioManager->PlaySample(purchasefailSFX, false);
										}
									}
								}
							}
						}

						for (int i = 0; i < storeWeapons.size(); i++)
						{
							if (storeWeapons[i])
							{
								gef::Vector4 sphere_centre(storeWeapons[i]->getBody()->GetPosition().x, storeWeapons[i]->getBody()->GetPosition().y, 0.0f);
								float  sphere_radius = 1.0f;

								// check to see if the ray intersects with the bound sphere that is around the player
								if (RaySphereIntersect(ray_start_position, ray_direction, sphere_centre, sphere_radius))
								{
									//Player touched an enemy do something
									playerData = storeWeapons[i]->run(playerData); //Lower this by the damage of the current weapon
									if (storeWeapons[i]->didPurchaseSucced() == true)
									{
										if (playAudio == true)
										{
											audioManager->PlaySample(purchaseSfx, false);
										}
									}
									else
									{
										if (playAudio == true)
										{
											audioManager->PlaySample(purchasefailSFX, false);
										}
									}
								}
							}
						}

						break;
					default:
						break;
					}

				}
			}
			else if (activeTouchID == touch->id)
			{
				if (touch->type == gef::TT_RELEASED)
				{
					activeTouchID = -1;
				}
			}
		}
	}
}

gef::Scene* SceneApp::LoadSceneAssets(gef::Platform& platform, const char* filename)
{
	gef::Scene* scene = new gef::Scene();

	if (scene->ReadSceneFromFile(platform, filename))
	{
		// if scene file loads successful
		// create material and mesh resources from the scene data
		scene->CreateMaterials(platform);
		scene->CreateMeshes(platform);
	}
	else
	{
		delete scene;
		scene = NULL;
	}

	return scene;
}

gef::Mesh* SceneApp::getMeshFromSceneAssets(gef::Scene* scene)
{
	gef::Mesh* mesh = NULL;

	// if the scene data contains at least one mesh
	// return the first mesh
	if (scene && scene->meshes.size() > 0)
		mesh = scene->meshes.front();

	return mesh;
}

//Code from Polishing a Game from MLS
void SceneApp::GetScreenPosRay(const gef::Vector2& screen_position, const gef::Matrix44& projection, const gef::Matrix44& view, gef::Vector4& startPoint, gef::Vector4& direction)
{
	gef::Vector2 ndc;

	float hw = platform_.width() * 0.5f;
	float hh = platform_.height() * 0.5f;

	ndc.x = (static_cast<float>(screen_position.x) - hw) / hw;
	ndc.y = (hh - static_cast<float>(screen_position.y)) / hh;

	gef::Matrix44 projectionInverse;
	projectionInverse.Inverse(view * projection);

	gef::Vector4 nearPoint, farPoint;

	nearPoint = gef::Vector4(ndc.x, ndc.y, ndc_z_min_, 1.0f).TransformW(projectionInverse);
	farPoint = gef::Vector4(ndc.x, ndc.y, 1.0f, 1.0f).TransformW(projectionInverse);

	nearPoint /= nearPoint.w();
	farPoint /= farPoint.w();

	startPoint = gef::Vector4(nearPoint.x(), nearPoint.y(), nearPoint.z());
	direction = farPoint - nearPoint;
	direction.Normalise();
}

//Code from Polishing a Game from MLS
bool SceneApp::RaySphereIntersect(gef::Vector4& startPoint, gef::Vector4& direction, gef::Vector4& sphere_centre, float sphere_radius)
{
	gef::Vector4 m = startPoint - sphere_centre;
	float b = m.DotProduct(direction);
	float c = m.LengthSqr() - sphere_radius * sphere_radius;

	// Exit if rays origin outside sphere (c > 0) and ray pointing away from sphere (b > 0) 
	if (c > 0.0f && b > 0.0f)
		return false;
	float discr = b * b - c;

	// A negative discriminant corresponds to ray missing sphere 
	if (discr < 0.0f)
		return false;

	// Ray now found to intersect sphere, compute smallest t value of intersection
	float t = -b - sqrtf(discr);

	// If t is negative, ray started inside sphere so clamp t to zero 
	if (t < 0.0f)
		t = 0.0f;

	gef::Vector4 hitpoint = startPoint + direction * t;

	return true;
}
