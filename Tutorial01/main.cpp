// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#include <iostream>
#include <irrlicht.h>
#include <DunGen.h>

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

	std::cout << "Dungeongenerator Tutorial 01 is started." << std::endl;
	std::cout << "Resolution is: " << resolution.Width << " * " << resolution.Height << " , AntiAliasing: " <<  antiAlias << std::endl;

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
	irrDevice->setWindowCaption(L"Dungeongenerator Tutorial 01: Create dungeon by code.");

	//************************************************************************
	// Start of dungeon creation
	//************************************************************************

	// create Dungeongenerator instance
	DunGen::CDunGen *dunGen = new DunGen::CDunGen(irrDevice);
	dunGen->StartUp();
	dunGen->SetPrintToConsole(true);

	// load room patterns and set materials
	if (!antiAlias)
	{
		dunGen->RoomPatternLoad("data/roompattern_sphere.irr");
		dunGen->RoomPatternLoad("data/roompattern_square1.irr");
		dunGen->RoomPatternLoad("data/roompattern_square2.irr");
		dunGen->RoomPatternLoad("data/roompattern_square4.irr");
		dunGen->MaterialSetCorridor("data/corridor_texture.jpg", false, false);
		dunGen->MaterialSetCaveMultiColor(false, false);

	}
	else
	{
		dunGen->RoomPatternLoad("data/roompattern_sphere_aa.irr");
		dunGen->RoomPatternLoad("data/roompattern_square1_aa.irr");
		dunGen->RoomPatternLoad("data/roompattern_square2_aa.irr");
		dunGen->RoomPatternLoad("data/roompattern_square4_aa.irr");
		dunGen->MaterialSetCorridor("data/corridor_texture.jpg", false, true);
		dunGen->MaterialSetCaveMultiColor(false, true);
	}

	// create L-system
	dunGen->LSystemSetStart("YYFYF");
	dunGen->LSystemAddRule('F',"F-YX-X---");
	dunGen->LSystemAddRule('X',"F$F++F-X");
	dunGen->LSystemAddRule('Y',"oYX--XX++");
	dunGen->LSystemSetParameter(DunGen::ELSystemParameter::ANGLE_YAW, 250.0);
	dunGen->LSystemSetParameter(DunGen::ELSystemParameter::ANGLE_PITCH, 1.0);
	dunGen->LSystemSetParameter(DunGen::ELSystemParameter::ANGLE_ROLL, 0.0);
	dunGen->LSystemSetParameter(DunGen::ELSystemParameter::RADIUS_DECREMENT, 0.0);
	dunGen->LSystemSetParameter(DunGen::ELSystemParameter::RADIUS_FACTOR, 1.0);
	dunGen->LSystemSetParameter(DunGen::ELSystemParameter::RADIUS_START, 14.0);
	dunGen->CreateLSystemDerivation(8);

	// create voxel cave
	dunGen->CreateVoxelCave();
	dunGen->ErodeVoxelCave(0.5);

	// place rooms
	dunGen->CreateRoom(0,irr::core::vector3d<double>(379.0,239.0,497.0),irr::core::vector3d<double>(0.0,180.0,0.0),irr::core::vector3d<double>(1.0,1.0,1.0));	//0
	dunGen->CreateRoom(0,irr::core::vector3d<double>(14.0,260.0,121.0),irr::core::vector3d<double>(0.0,0.0,0.0),irr::core::vector3d<double>(1.0,1.0,1.0));		//1

	dunGen->CreateRoom(3,irr::core::vector3d<double>(539.0,281.0,378.0),irr::core::vector3d<double>(0.0,180.0,0.0),irr::core::vector3d<double>(1.0,1.0,1.0));	//2
	dunGen->CreateRoom(3,irr::core::vector3d<double>(312.0,272.0,-80.0),irr::core::vector3d<double>(0.0,270.0,0.0),irr::core::vector3d<double>(1.0,1.0,1.0));	//3
	dunGen->CreateRoom(3,irr::core::vector3d<double>(72.0,202.0,557.0),irr::core::vector3d<double>(0.0,80.0,0.0),irr::core::vector3d<double>(1.0,1.0,1.0));		//4

	dunGen->CreateRoom(2,irr::core::vector3d<double>(404.0,259.0,651.0),irr::core::vector3d<double>(0.0,90.0,0.0),irr::core::vector3d<double>(1.0,1.0,1.0));	//5
	dunGen->CreateRoom(2,irr::core::vector3d<double>(145.0,260.0,0.0),irr::core::vector3d<double>(0.0,200.0,0.0),irr::core::vector3d<double>(1.0,1.0,1.0));		//6
	dunGen->CreateRoom(2,irr::core::vector3d<double>(700.0,200,250.0),irr::core::vector3d<double>(0.0,305.0,0.0),irr::core::vector3d<double>(1.0,1.0,1.0));		//7

	dunGen->CreateRoom(1,irr::core::vector3d<double>(440.0,277.0,803.0),irr::core::vector3d<double>(0.0,90.0,0.0),irr::core::vector3d<double>(1.0,1.0,1.0));	//8
	dunGen->CreateRoom(1,irr::core::vector3d<double>(287.0,270.0,-245.0),irr::core::vector3d<double>(0.0,270.0,0.0),irr::core::vector3d<double>(1.0,1.0,1.0));	//9
	dunGen->CreateRoom(1,irr::core::vector3d<double>(-129.0,211.0,584.0),irr::core::vector3d<double>(0.0,20.0,0.0),irr::core::vector3d<double>(1.0,1.0,1.0));	//10
	dunGen->CreateRoom(1,irr::core::vector3d<double>(-100.0,300.0,-70.0),irr::core::vector3d<double>(0.0,300.0,0.0),irr::core::vector3d<double>(1.0,1.0,1.0));	//11

	// set corridor parameters
	dunGen->CorrdidorAddPoint(-4.0,-4.0, 1.4);
	dunGen->CorrdidorAddPoint(-4.0, 0.0, 0.9);
	dunGen->CorrdidorAddPoint(-3.0, 2.0, 0.6);
	dunGen->CorrdidorAddPoint(-2.0, 3.0, 0.3);
	dunGen->CorrdidorAddPoint( 0.0, 4.0, 0.0);
	dunGen->CorrdidorAddPoint( 4.0, 0.0, 0.9);
	dunGen->CorrdidorAddPoint( 4.0,-4.0, 1.4);
	dunGen->CorrdidorAddPoint( 0.0,-4.0, 2.0);

	// specify detailobjects
	DunGen::SDetailobjectParameters detailObject;
	irr::scene::IMesh* mesh = sceneManager->getMesh("data/detailobject.3ds");
	detailObject.Node = sceneManager->addMeshSceneNode(mesh);
	detailObject.Node->setName("Torch");
	detailObject.Node->setMaterialType(irr::video::EMT_SOLID);
	detailObject.Node->setMaterialFlag(irr::video::EMF_LIGHTING,false);
	detailObject.Node->setMaterialFlag(irr::video::EMF_ANTI_ALIASING, (0!=antiAlias) );
	detailObject.Position = irr::core::vector2d<double>(-4.0,-0.5);
	detailObject.Scale =  irr::core::vector3d<double>(1.0,1.0,1.0);
	detailObject.Rotation =  irr::core::vector3d<double>(0.0,90.0,0.0);
	detailObject.DistanceSampling = 0.6;
	detailObject.DistanceNumFactor = 4;
	detailObject.DistanceNumMin = 3;
	detailObject.DistanceNumMax = 3;
	detailObject.DistanceNumMinFirstElement = 0;
	detailObject.DistanceNumMaxFirstElement = 0;
	detailObject.ObjectAtT1 = false;
	dunGen->CorrdidorAddDetailObject(detailObject);
	detailObject.Position = irr::core::vector2d<double>(4.0,-0.5);
	detailObject.Rotation =  irr::core::vector3d<double>(0.0,270.0,0.0);
	dunGen->CorrdidorAddDetailObject(detailObject);

	// create corridors
	bool sightBlocking;	// tells us, if the corridor is definitely sight blocking, but we don't use this information here
	dunGen->CreateCorridorRoomRoom(0,2,4.0,200.0,2,1,4.0,200.0,sightBlocking);
	dunGen->CreateCorridorRoomRoom(0,1,4.0,100.0,5,0,4.0,100.0,sightBlocking);
	dunGen->CreateCorridorRoomRoom(5,1,4.0,100.0,8,0,4.0,100.0,sightBlocking);
	dunGen->CreateCorridorRoomRoom(2,2,4.0,150.0,7,0,4.0,250.0,sightBlocking);
	dunGen->CreateCorridorRoomRoom(2,3,4.0,150.0,7,1,4.0,250.0,sightBlocking);

	dunGen->CreateCorridorRoomRoom(1,1,4.0,200.0,6,0,4.0,200.0,sightBlocking);
	dunGen->CreateCorridorRoomRoom(1,2,4.0,200.0,11,0,4.0,200.0,sightBlocking);
	dunGen->CreateCorridorRoomRoom(6,1,4.0,200.0,3,3,4.0,200.0,sightBlocking);
	dunGen->CreateCorridorRoomRoom(3,2,4.0,100.0,9,0,4.0,100.0,sightBlocking);

	dunGen->CreateCorridorRoomRoom(4,1,4.0,100.0,10,0,4.0,100.0,sightBlocking);
	dunGen->CreateCorridorRoomRoom(4,2,4.0,400.0,4,3,4.0,400.0,sightBlocking);

	dunGen->CreateCorridorRoomCave(4,0,4.0,100.0, irr::core::vector3d<unsigned int>(46,172,470),irr::core::vector3d<unsigned int>(54,180,471),DunGen::EDirection::Z_NEGATIVE,4.0,100.0,sightBlocking);
	dunGen->CreateCorridorRoomCave(0,0,4.0,200.0, irr::core::vector3d<unsigned int>(136,189,450),irr::core::vector3d<unsigned int>(144,197,451),DunGen::EDirection::Z_NEGATIVE,4.0,200.0,sightBlocking);
	dunGen->CreateCorridorRoomCave(0,3,4.0,100.0, irr::core::vector3d<unsigned int>(389,231,450),irr::core::vector3d<unsigned int>(397,239,451),DunGen::EDirection::Z_NEGATIVE,4.0,100.0,sightBlocking);
	dunGen->CreateCorridorRoomCave(2,0,4.0,400.0, irr::core::vector3d<unsigned int>(478,257,260),irr::core::vector3d<unsigned int>(486,265,261),DunGen::EDirection::Z_NEGATIVE,4.0,200.0,sightBlocking);

	dunGen->CreateCorridorRoomCave(3,1,4.0,400.0, irr::core::vector3d<unsigned int>(460,264,80),irr::core::vector3d<unsigned int>(468,272,81),DunGen::EDirection::Z_POSITIVE,4.0,400.0,sightBlocking);
	dunGen->CreateCorridorRoomCave(3,0,4.0,50.0, irr::core::vector3d<unsigned int>(340,287,40),irr::core::vector3d<unsigned int>(348,295,41),DunGen::EDirection::Z_POSITIVE,4.0,50.0,sightBlocking);
	dunGen->CreateCorridorRoomCave(1,0,4.0,400.0, irr::core::vector3d<unsigned int>(50,318,262),irr::core::vector3d<unsigned int>(51,326,270),DunGen::EDirection::X_POSITIVE,4.0,300.0,sightBlocking);
	dunGen->CreateCorridorRoomCave(1,3,4.0,100.0, irr::core::vector3d<unsigned int>(80,181,240),irr::core::vector3d<unsigned int>(88,189,241),DunGen::EDirection::Z_POSITIVE,4.0,100.0,sightBlocking);

	// modify corridor parameters
	dunGen->CorrdidorRemovePoints();
	dunGen->CorrdidorAddPoint(-4.0,-4.0, 0.0);
	dunGen->CorrdidorAddPoint(-4.0, 4.0, 1.0);
	dunGen->CorrdidorAddPoint(4.0, 4.0, 2.0);
	dunGen->CorrdidorAddPoint(4.0, -4.0, 1.0);

	dunGen->CorrdidorRemoveDetailobjects();
	detailObject.Position = irr::core::vector2d<double>(4.0,0.0);
	dunGen->CorrdidorAddDetailObject(detailObject);
	detailObject.Position = irr::core::vector2d<double>(-4.0,0.0);
	detailObject.Rotation =  irr::core::vector3d<double>(0.0,90.0,0.0);
	dunGen->CorrdidorAddDetailObject(detailObject);
	
	// cave to cave corridor with 2 voxel additional digging depth
	dunGen->CreateCorridorCaveCave(
		irr::core::vector3d<unsigned int>(300,306,205),irr::core::vector3d<unsigned int>(302,314,213),DunGen::EDirection::X_NEGATIVE,4.0,200.0,
		irr::core::vector3d<unsigned int>(350,244,259),irr::core::vector3d<unsigned int>(352,252,267),DunGen::EDirection::X_POSITIVE,4.0,200.0,
		sightBlocking);

	// create mesh cave (including filter step)
	dunGen->RemoveHoveringVoxelFragments();
	dunGen->CreateMeshCave();

	// assemble dungeon
	dunGen->AddDungeon(sceneManager->getRootSceneNode(),sceneManager);

	// drop the detailobject-mesh and hide the original node (it is duplicated in the dungeon already)
	// note: you cannot drop the original node itself, this would lead to some strange behavior
	// it seems, that the clones depend on the original node on some way
	mesh->drop();
	detailObject.Node->setVisible(false);

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

				irr::core::stringw str = L"Dungeongenerator Tutorial 01: Create dungeon by code. - FPS: ";
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