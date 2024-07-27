// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#include <iostream>
#include <irrlicht.h>
#include "EventReceiver.h"

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

	std::cout << "Dungeongenerator Tool: Random Caves is started." << std::endl;
	std::cout << "Resolution is: " << resolution.Width << " * " << resolution.Height << " , AntiAliasing: " <<  antiAlias << std::endl;

	// create Irrlicht device
	CEventReceiver eventReceiver;
	irr::SIrrlichtCreationParameters irrlichtParameter;
	irrlichtParameter.DriverType = irr::video::EDT_DIRECT3D9;
	irrlichtParameter.WindowSize = resolution;
	irrlichtParameter.Bits = 32;
	irrlichtParameter.Fullscreen = false;
	irrlichtParameter.Stencilbuffer = false;
	irrlichtParameter.Vsync = false;
	irrlichtParameter.AntiAlias = antiAlias;
	irrlichtParameter.EventReceiver = &eventReceiver;

	irr::IrrlichtDevice* irrDevice = irr::createDeviceEx(irrlichtParameter);
	if(irrDevice == 0)
	{
		std::cout << "Irrlicht device could not be created, program is terminated." << std::endl;
		return 1;
	}
	irr::scene::ISceneManager* sceneManager = irrDevice->getSceneManager();
	irr::video::IVideoDriver* videoDriver = irrDevice->getVideoDriver();
	eventReceiver.Intialize(irrDevice);

	irrDevice->setWindowCaption(L"Dungeongenerator Tool: Random Caves.");

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

			tmp = eventReceiver.GetCamera()->getAbsolutePosition();
			actCamPos.X = static_cast<int>(tmp.X + 0.5f);
			actCamPos.Y = static_cast<int>(tmp.Y + 0.5f);
			actCamPos.Z = static_cast<int>(tmp.Z + 0.5f);

			if (actFPS != lastFPS || lastCamPos != actCamPos)
			{
				// update FPS and camera position
				// and show them as window name
				lastFPS = actFPS;
				lastCamPos = actCamPos;

				irr::core::stringw str = L"Dungeongenerator Tool: Random Caves. - FPS: ";
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
	return 0;
}