// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#include <iostream>
#include <irrlicht.h>
#include <DunGen.h>
#include "MaterialCaveCustom.h"
#include "MaterialCorridorCustom.h"

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(lib, "DunGen.lib")
#endif

int main(int argc, char* argv[])
{
	// check for additional parameters: resolution, anti aliasing
	irr::core::dimension2du resolution = irr::core::dimension2du(1200, 900);
	if (argc>=3)
	{
		resolution.Width = atoi(argv[1]);
		resolution.Height = atoi(argv[2]);
	}
	int antiAlias = 0;
	if (argc>=4)
		antiAlias = atoi(argv[3]);
	char* filename = "dungeon_ascending.xml";
	if (argc>=5)
		filename = argv[4];

	std::cout << "Dungeongenerator Tutorial 03 is started." << std::endl;
	std::cout << "Resolution is: " << resolution.Width << " * " << resolution.Height << " , AntiAliasing: " <<  antiAlias << " , Filename: " <<  filename << std::endl;

	// create Irrlicht device
	irr::SIrrlichtCreationParameters irrlichtParameter;
	irrlichtParameter.DriverType = irr::video::EDT_DIRECT3D9;
	irrlichtParameter.WindowSize = resolution;
	irrlichtParameter.Bits = 32;
	irrlichtParameter.Fullscreen = false;
	irrlichtParameter.Stencilbuffer = false;
	irrlichtParameter.Vsync = false;
	irrlichtParameter.AntiAlias = antiAlias;
	irrlichtParameter.EventReceiver = 0;

	irr::IrrlichtDevice* irrDevice = irr::createDeviceEx(irrlichtParameter);
	if(irrDevice == 0)
	{
		std::cout << "Irrlicht device could not be created, program is terminated." << std::endl;
		return 1;
	}
	irr::scene::ISceneManager* sceneManager = irrDevice->getSceneManager();
	irr::video::IVideoDriver* videoDriver = irrDevice->getVideoDriver();
	irrDevice->setWindowCaption(L"Dungeongenerator Tutorial 03: Custom materials.");

	//************************************************************************
	// Start of dungeon creation
	//************************************************************************

	// create Dungeongenerator instance
	DunGen::CDunGen *dunGen = new DunGen::CDunGen(irrDevice);
	dunGen->StartUp();
	dunGen->SetPrintToConsole(true);

	// read Dungeon from file
	dunGen->ReadDungeonFromFile(filename);

	// set custom materials
	CMaterialCaveCustom* matCave = new CMaterialCaveCustom(irrDevice);
	CMaterialCorridorCustom* matCorridor = new CMaterialCorridorCustom(irrDevice);
	dunGen->MaterialSetCustom(DunGen::SMaterialType::CAVE_CUSTOM, matCave, false, true);
	dunGen->MaterialSetCustom(DunGen::SMaterialType::CORRIDOR_CUSTOM, matCorridor, false, true);

	// assemble dungeon
	dunGen->AddDungeon(sceneManager->getRootSceneNode(),sceneManager);

	// deletes all not used objects from the Dungeongenerator to save memory
	dunGen->ShutDown();

	//************************************************************************
	// Dungeon creation finished
	//************************************************************************

	// create camera
	irr::scene::ICameraSceneNode* camera = sceneManager->addCameraSceneNodeFPS(0,100.0f,0.05f);
	camera->setFarValue(20000.0f);
	camera->setPosition(irr::core::vector3df(0,0,0));
	camera->setTarget(irr::core::vector3df(99999.f, 99999.f, 99999.f));

	irr::s32 lastFPS(0), actFPS(0);
	irr::core::vector3d<int> lastCamPos(-1,-1,-1);
	irr::core::vector3d<int> actCamPos(0,0,0);
	irr::core::vector3df tmp;

	// as long as the engine is active:
	while(irrDevice->run() && videoDriver)
	{
		if (irrDevice->isWindowActive())
		{
			// draw everything
			videoDriver->beginScene(true, true, irr::video::SColor(255,100,101,140));
			sceneManager->drawAll();
			videoDriver->endScene();

			actFPS = videoDriver->getFPS();

			tmp = camera->getAbsolutePosition();
			actCamPos.X = static_cast<int>(tmp.X + 0.5f);
			actCamPos.Y = static_cast<int>(tmp.Y + 0.5f);
			actCamPos.Z = static_cast<int>(tmp.Z + 0.5f);

			if (actFPS != lastFPS || lastCamPos != actCamPos)
			{
				// update FPS and camera position
				// and show them as window name
				lastFPS = actFPS;
				lastCamPos = actCamPos;

				irr::core::stringw str = L"Dungeongenerator Tutorial 03: Custom Materials. - FPS: ";
				str += actFPS;
				str += " - Position: (";
				str += actCamPos.X;
				str += ",";
				str += actCamPos.Y;
				str += ",";
				str += actCamPos.Z;
				str += ")";

				irrDevice->setWindowCaption(str.c_str());
			}
		}
		else
			irrDevice->yield();
	}

	// destroy the engine
	irrDevice->drop();
	delete matCave;
	delete matCorridor;
	return 0;
}