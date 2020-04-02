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
	primitive_builder_(NULL),
	input_manager_(NULL),
	font_(NULL),
	world_(NULL),
	button_icon_(NULL),
	backgroundSprite(NULL),
	audioManager(NULL),
	activeTouchID(-1),
	enemySceneAsset(NULL),
	playerSceneAsset(NULL),
	hitDetection(NULL)
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
				updateStateMachine(1);
				break;
			case SceneApp::Level1:
				updateStateMachine(0);
				break;
			default:
				break;
			}
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
	Player.UpdateFromSimulation(Player.getBody());

	for (int i = 0; i < enemies.size(); i++)
	{
		enemies[i]->UpdateFromSimulation(enemies[i]->getBody());
	}

	hitDetection->UpdateFromSimulation(hitDetection->getBody());

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
			hitDetectionObject* hitDectection = NULL;
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
				player->decrementHealth(gameTime);
			}

			if (enemy && hitDectection)
			{
				enemy->DecrementHealth();
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
	audioManager->PlayMusic();
}

void SceneApp::FrontendRelease()
{
	delete button_icon_;
	button_icon_ = NULL;

	delete backgroundSprite;
	backgroundSprite = NULL;

	audioManager->StopMusic();
	audioManager->UnloadMusic();
}

void SceneApp::FrontendUpdate(float frame_time)
{
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);
}

void SceneApp::FrontendRender()
{
	sprite_renderer_->Begin();

	//Render our background 
	gef::Sprite background;
	background.set_texture(backgroundSprite);
	background.set_position(gef::Vector4(platform_.width() - 480.f, platform_.height() - 273.f, 0.f));
	background.set_height(544.f);
	background.set_width(960.f);
	sprite_renderer_->DrawSprite(background);

	// Render Title Text
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f - 270.f , 0.f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"Save The Home!");

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

void SceneApp::GameInit()
{
	//Initalize our time variable
	gameTime = 0;
	//Define how many enemies to make, if it is < 0 program will not respond
	unsigned int enemiesToMake = 8;
	// Make sure there is a panel to detect touch, activate if it exists
	if (input_manager_ && input_manager_->touch_manager() && (input_manager_->touch_manager()->max_num_panels() > 0))
	{
		input_manager_->touch_manager()->EnablePanel(0);
	}

	//Load our enemy asset
	const char* sceneAssetFilename = "stickman.scn";
	enemySceneAsset = LoadSceneAssets(platform_, sceneAssetFilename);
	if (!enemySceneAsset)
	{
		gef::DebugOut("Failed to load enemy scene file. %s", sceneAssetFilename);
	}
	sceneAssetFilename = "NewHouse.scn";
	playerSceneAsset = LoadSceneAssets(platform_, sceneAssetFilename);
	if (!playerSceneAsset)
	{
		gef::DebugOut("Failed to load player scene file. %s", sceneAssetFilename);
	}

	// create the renderer for draw 3D geometry
	renderer_3d_ = gef::Renderer3D::Create(platform_);

	// initialise primitive builder to make create some 3D geometry easier
	primitive_builder_ = new PrimitiveBuilder(platform_);

	//Create our touch sprite
	touchSprite.set_position(platform_.width() * 0.5f, platform_.height() * 0.5f, 0.0f);
	touchSprite.set_width(64.0f);
	touchSprite.set_height(64.0f);

	//Load our audio samples
	gunShotSampleID = audioManager->LoadSample("gunShot.wav", platform_);

	SetupLights();

	// initialise the physics world
	b2Vec2 gravity(0.0f,0.0f);
	world_ = new b2World(gravity);

	//Setup player
	Player.init(playerSceneAsset, world_);
	Player.updateScale(gef::Vector4(0.1f, 0.2f, 0.1f));
	Player.updateRotationY(90);

	for (int i = 0; i < enemiesToMake; i++)
	{
		enemies.push_back(new EnemyObject(enemySceneAsset,world_));
	}

	//Move alive enemeies
	for (int i = 0; i < enemies.size(); i++)
	{
		enemies[i]->getBody()->ApplyForceToCenter(b2Vec2(3, 0), true);
	}

	//Create our hit detection object
	hitDetection = new hitDetectionObject(world_,primitive_builder_);
	hitDetection->updateScale(gef::Vector4(2.0f, 2.0f, 1.0f));
}

void SceneApp::GameRelease()
{
	// destroying the physics world also destroys all the objects within it
	delete world_;
	world_ = NULL;

	delete primitive_builder_;
	primitive_builder_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	enemies.clear();

	gameTime = 0;

	audioManager->UnloadSample(gunShotSampleID);
}

void SceneApp::GameUpdate(float frame_time)
{
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);

	gameTime = gameTime + frame_time;
	Player.update();
	hitDetection->update();

	//check all the alive enemies to see if they need to be killed
	for (int i = 0; i < enemies.size(); i++)
	{
		enemies[i]->update();
		if (enemies[i]->getHealth() <= 0)
		{
			enemies.erase(enemies.begin() + i);//Remove the now dead enemy
		}
	}

	ProcessTouchInput();

	UpdateSimulation(frame_time);
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
	Player.render(renderer_3d_);	

	//Draw our hit detection object
	testRender = false; //Can be used for debugging renderer (use as a condition = true)
	hitDetection->render(renderer_3d_);

	//Draw enemy
	for (int i = 0; i < enemies.size(); i++)
	{
		enemies[i]->render(renderer_3d_);
	}

	renderer_3d_->End();

	// start drawing sprites, but don't clear the frame buffer
	sprite_renderer_->Begin(false);
	sprite_renderer_->DrawSprite(touchSprite);
	DrawHUD();

	// Render Title Text
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f - 270.f, 0.f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"Health %i", Player.getHealth());

	sprite_renderer_->End();
}

void SceneApp::updateStateMachine(int ID)
{
	switch (ID)
	{
	case 0:
		GameRelease();
		FrontendInit();
		gameState = INIT;
		break;
	case 1:
		FrontendRelease();
		GameInit();
		gameState = Level1;
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

					//move touch sprite
					touchSprite.set_position(touch->position.x, touch->position.y, 0);

					hitDetection->translate(gef::Vector4(touch->position.x, touch->position.y, 1));
					testRender = true;
					//audioManager->PlaySample(gunShotSampleID,false);
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
