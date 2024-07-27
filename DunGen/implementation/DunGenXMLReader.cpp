// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#include "interface/DunGen.h"
#include "DunGenXMLReader.h"
#include <string>

DunGen::CDunGenXMLReader::CDunGenXMLReader(irr::IrrlichtDevice* irrDevice_, CDunGen* dunGen_)
	: FileSystem(irrDevice_->getFileSystem())
	, DunGenInterface(dunGen_)
	, File(NULL)
	, XmlReader(NULL)
	, PrivateSceneManager(irrDevice_->getSceneManager()->createNewSceneManager(false))
{
}

DunGen::CDunGenXMLReader::~CDunGenXMLReader()
{
	PrivateSceneManager->drop();
}

bool DunGen::CDunGenXMLReader::Read( const irr::io::path& filename_ )
{
	// get read access to file
	File = FileSystem->createAndOpenFile(filename_);

	if (!File)
		return false;

	XmlReader = FileSystem->createXMLReader(File);

	// clear previous content
	DunGenInterface->ClearRoomsAndCorridors();
	RoomPathes.clear();
	PrivateSceneManager->clear();

	// process file content
	bool dunGenPart = false;
	while(XmlReader->read())
    {
		// is this part of DunGen XML?
		if (XmlReader->getNodeType() == irr::io::EXN_ELEMENT && irr::core::stringw("DunGen") == XmlReader->getNodeName())
		{
			dunGenPart = true;
		}
		else if (XmlReader->getNodeType() == irr::io::EXN_ELEMENT_END && irr::core::stringw("DunGen") == XmlReader->getNodeName())
		{
			dunGenPart = false;
		}

		// delegate XML reading, depending on tag
		if (dunGenPart && XmlReader->getNodeType() == irr::io::EXN_ELEMENT)
		{
			if (irr::core::stringw("Materials") == XmlReader->getNodeName())
				ReadMaterials();
			else if (irr::core::stringw("RandomGenerator") == XmlReader->getNodeName())
				ReadWarpOptions();
			else if (irr::core::stringw("WarpOptions") == XmlReader->getNodeName())
				ReadWarpOptions();
			else if (irr::core::stringw("DrawVoxelCave") == XmlReader->getNodeName())
				ReadDrawVoxelCave();
			else if (irr::core::stringw("Erode") == XmlReader->getNodeName())
				ReadErode();
			else if (irr::core::stringw("Filter") == XmlReader->getNodeName())
				ReadFilter();
			else if (irr::core::stringw("GenerateMeshCave") == XmlReader->getNodeName())
				ReadGenerateMeshCave();

			else if (irr::core::stringw("PlaceRoom") == XmlReader->getNodeName())
				ReadPlaceRoom();
			else if (irr::core::stringw("CorridorSettings") == XmlReader->getNodeName())
				ReadCorridorSettings();
			else if (irr::core::stringw("CorridorDetailobjects") == XmlReader->getNodeName())
				ReadCorrdidorDetailObjects();
			else if (irr::core::stringw("CorridorRoomRoom") == XmlReader->getNodeName())
				ReadCorridorRoomRoom();
			else if (irr::core::stringw("CorridorRoomCave") == XmlReader->getNodeName())
				ReadCorridorRoomCave();
			else if (irr::core::stringw("CorridorCaveCave") == XmlReader->getNodeName())
				ReadCorridorCaveCave();
		}
	}

	// close file
	XmlReader->drop();
	File->drop();
	return true;
}

void DunGen::CDunGenXMLReader::ReadMaterials()
{
	bool backFaceCulling = (irr::core::stringw("0") != XmlReader->getAttributeValue(L"BackfaceCulling"));
	bool antiAliasing = (irr::core::stringw("0") != XmlReader->getAttributeValue(L"AntiAliasing"));

	while(XmlReader->read() && (XmlReader->getNodeType() != irr::io::EXN_ELEMENT_END || irr::core::stringw("Materials") != XmlReader->getNodeName()) )
    {
		if (XmlReader->getNodeType() == irr::io::EXN_ELEMENT)
		{
			// read cave material parameters
			if (irr::core::stringw("Cave") == XmlReader->getNodeName())
			{
				if (irr::core::stringw("SingleColor") == XmlReader->getAttributeValue(L"Shader"))
				{
					float red = XmlReader->getAttributeValueAsFloat(L"Red");
					float green = XmlReader->getAttributeValueAsFloat(L"Green");
					float blue = XmlReader->getAttributeValueAsFloat(L"Blue");
					DunGenInterface->MaterialSetCaveSingleColor(irr::video::SColorf(red,green,blue), backFaceCulling, antiAliasing);
				}
				else
				{
					DunGenInterface->MaterialSetCaveMultiColor(backFaceCulling, antiAliasing);
				}
			}
			// read corridor material parameters
			else if (irr::core::stringw("Corridor") == XmlReader->getNodeName())
			{
				DunGenInterface->MaterialSetCorridor(XmlReader->getAttributeValue(L"Texture"), backFaceCulling, antiAliasing);
			}
		}
	}
}

void DunGen::CDunGenXMLReader::ReadRandomGenerator()
{
	DunGenInterface->RandomGeneratorSetParameters(
		XmlReader->getAttributeValueAsInt(L"Seed"),
		XmlReader->getAttributeValueAsInt(L"A"),
		XmlReader->getAttributeValueAsInt(L"C"),
		XmlReader->getAttributeValueAsInt(L"M"));
}

void DunGen::CDunGenXMLReader::ReadWarpOptions()
{
	DunGenInterface->MeshCaveSetWarpParameters(
		(irr::core::stringw("0") != XmlReader->getAttributeValue(L"Warping")),
		(irr::core::stringw("0") != XmlReader->getAttributeValue(L"Smoothing")),
		XmlReader->getAttributeValueAsInt(L"WarpRandomSeed"),
		XmlReader->getAttributeValueAsFloat(L"WarpStrength") );
}

void DunGen::CDunGenXMLReader::ReadDrawVoxelCave()
{
	// delete the old settings
	DunGenInterface->LSystemDeleteRules();

	// read the new settings
	irr::core::stringc tmpString = XmlReader->getAttributeValue(L"StartString");
	DunGenInterface->LSystemSetStart(tmpString.c_str());

	DunGenInterface->LSystemSetParameter(ELSystemParameter::RADIUS_START, XmlReader->getAttributeValueAsFloat(L"StartRadius"));
	unsigned int derivation = XmlReader->getAttributeValueAsInt(L"Derivation");

	while(XmlReader->read() && (XmlReader->getNodeType() != irr::io::EXN_ELEMENT_END || irr::core::stringw("DrawVoxelCave") != XmlReader->getNodeName()) )
    {
		if (XmlReader->getNodeType() == irr::io::EXN_ELEMENT)
		{
			if (irr::core::stringw("Basic") == XmlReader->getNodeName())
			{
				DunGenInterface->VoxelCaveSetParameters(XmlReader->getAttributeValueAsInt(L"VoxelBorder"),
					XmlReader->getAttributeValueAsInt(L"MinDrawRadius"));
			}
			else if (irr::core::stringw("Radius") == XmlReader->getNodeName())
			{
				DunGenInterface->LSystemSetParameter(ELSystemParameter::RADIUS_FACTOR,  XmlReader->getAttributeValueAsFloat(L"RadiusFactor"));
				DunGenInterface->LSystemSetParameter(ELSystemParameter::RADIUS_DECREMENT, XmlReader->getAttributeValueAsFloat(L"RadiusDecrement"));
			}
			else if (irr::core::stringw("Angle") == XmlReader->getNodeName())
			{
				DunGenInterface->LSystemSetParameter(ELSystemParameter::ANGLE_YAW, XmlReader->getAttributeValueAsFloat(L"Yaw"));
				DunGenInterface->LSystemSetParameter(ELSystemParameter::ANGLE_PITCH, XmlReader->getAttributeValueAsFloat(L"Pitch"));
				DunGenInterface->LSystemSetParameter(ELSystemParameter::ANGLE_ROLL, XmlReader->getAttributeValueAsFloat(L"Roll"));
			}
			else if (irr::core::stringw("Rule") == XmlReader->getNodeName())
			{
				tmpString = XmlReader->getAttributeValue(L"Symbol");
				char sym = tmpString[0];
				tmpString = XmlReader->getAttributeValue(L"Substitution");
				
				DunGenInterface->LSystemAddRule(sym, tmpString.c_str());
			}
		}
	}

	// create L-System derivations and draw the voxel cave
	DunGenInterface->CreateLSystemDerivation(derivation);
	DunGenInterface->CreateVoxelCave();
}

void DunGen::CDunGenXMLReader::ReadErode()
{
	DunGenInterface->ErodeVoxelCave(XmlReader->getAttributeValueAsFloat(L"Likelihood"));	
}

void DunGen::CDunGenXMLReader::ReadFilter()
{
	DunGenInterface->RemoveHoveringVoxelFragments();
}

void DunGen::CDunGenXMLReader::ReadGenerateMeshCave()
{
	DunGenInterface->MeshCaveSetNormalWeightMethod(static_cast<DunGen::ENormalWeightMethod::Enum>(
		XmlReader->getAttributeValueAsInt(L"NormalWeighting") ));

	DunGenInterface->CreateMeshCave();
}

void DunGen::CDunGenXMLReader::ReadPlaceRoom()
{
	irr::io::path roomPatternPath = XmlReader->getAttributeValue(L"Filename");

	// determine the id of the room pattern
	unsigned int roomPatternId = RoomPathes.size();

	for (unsigned int i=0; i<RoomPathes.size(); ++i)
	{
		if (roomPatternPath == RoomPathes[i])
		{
			roomPatternId = i;
			break;
		}
	}

	// load new room pattern, if not loaded already
	if (RoomPathes.size() == roomPatternId)
	{
		DunGenInterface->RoomPatternLoad(roomPatternPath);
		RoomPathes.push_back(roomPatternPath);
	}

	// read instance parameters
	irr::core::vector3d<double> position;
	irr::core::vector3d<double> rotation;
	irr::core::vector3d<double> scaling;

	while(XmlReader->read() && (XmlReader->getNodeType() != irr::io::EXN_ELEMENT_END || irr::core::stringw("PlaceRoom") != XmlReader->getNodeName()) )
    {
		if (XmlReader->getNodeType() == irr::io::EXN_ELEMENT)
		{
			if (irr::core::stringw("Position") == XmlReader->getNodeName())
			{
				position.X = XmlReader->getAttributeValueAsFloat(L"X");
				position.Y = XmlReader->getAttributeValueAsFloat(L"Y");
				position.Z = XmlReader->getAttributeValueAsFloat(L"Z");
			}
			else if (irr::core::stringw("Rotation") == XmlReader->getNodeName())
			{
				rotation.X = XmlReader->getAttributeValueAsFloat(L"X");
				rotation.Y = XmlReader->getAttributeValueAsFloat(L"Y");
				rotation.Z = XmlReader->getAttributeValueAsFloat(L"Z");
			}
			else if (irr::core::stringw("Scaling") == XmlReader->getNodeName())
			{
				scaling.X = XmlReader->getAttributeValueAsFloat(L"X");
				scaling.Y = XmlReader->getAttributeValueAsFloat(L"Y");
				scaling.Z = XmlReader->getAttributeValueAsFloat(L"Z");
			}
		}
	}

	// create room instance
	DunGenInterface->CreateRoom(roomPatternId, position, rotation, scaling);
}

void DunGen::CDunGenXMLReader::ReadCorridorSettings()
{
	// remove old points
	DunGenInterface->CorrdidorRemovePoints();

	// read and set sampling values
	double sampling = XmlReader->getAttributeValueAsFloat(L"Sampling");
	double texSampling = XmlReader->getAttributeValueAsFloat(L"TextureSampling");

	DunGenInterface->CorridorSetDistances(sampling, texSampling);

	// read and add points
	while(XmlReader->read() && (XmlReader->getNodeType() != irr::io::EXN_ELEMENT_END || irr::core::stringw("CorridorSettings") != XmlReader->getNodeName()) )
    {
		if (XmlReader->getNodeType() == irr::io::EXN_ELEMENT && irr::core::stringw("Point") == XmlReader->getNodeName())
		{
			double posX = XmlReader->getAttributeValueAsFloat(L"PosX");
			double posY = XmlReader->getAttributeValueAsFloat(L"PosY");
			double texX = XmlReader->getAttributeValueAsFloat(L"TexX");

			DunGenInterface->CorrdidorAddPoint(posX, posY, texX);
		}
	}
}

void DunGen::CDunGenXMLReader::ReadCorrdidorDetailObjects()
{
	DunGen::SDetailobjectParameters detailObject;
	irr::core::stringc tmpString;
	DunGenInterface->CorrdidorRemoveDetailobjects();

	while(XmlReader->read() && (XmlReader->getNodeType() != irr::io::EXN_ELEMENT_END || irr::core::stringw("CorridorDetailobjects") != XmlReader->getNodeName()) )
	{
		// parse values of actualdetail object
		if (XmlReader->getNodeType() == irr::io::EXN_ELEMENT)
		{
			if (irr::core::stringw("Detailobject") == XmlReader->getNodeName())
			{
				irr::scene::IMesh* mesh = PrivateSceneManager->getMesh(XmlReader->getAttributeValue(L"Model"));
				detailObject.Node = PrivateSceneManager->addMeshSceneNode(mesh);
				detailObject.Node->setName(XmlReader->getAttributeValue(L"Name"));

				detailObject.Node->setMaterialType(irr::video::EMT_SOLID);
				detailObject.Node->setMaterialFlag(irr::video::EMF_LIGHTING, irr::core::stringw("0") != XmlReader->getAttributeValue(L"Lighting") );
				detailObject.Node->setMaterialFlag(irr::video::EMF_ANTI_ALIASING, irr::core::stringw("0") != XmlReader->getAttributeValue(L"AntiAliasing") );
			}
			else if (irr::core::stringw("Position") == XmlReader->getNodeName())
			{
				detailObject.Position.X = XmlReader->getAttributeValueAsFloat(L"X");
				detailObject.Position.Y = XmlReader->getAttributeValueAsFloat(L"Y");
			}
			else if (irr::core::stringw("Rotation") == XmlReader->getNodeName())
			{
				detailObject.Rotation.X = XmlReader->getAttributeValueAsFloat(L"X");
				detailObject.Rotation.Y = XmlReader->getAttributeValueAsFloat(L"Y");
				detailObject.Rotation.Z = XmlReader->getAttributeValueAsFloat(L"Z");

			}
			else if (irr::core::stringw("Scaling") == XmlReader->getNodeName())
			{
				detailObject.Scale.X = XmlReader->getAttributeValueAsFloat(L"X");
				detailObject.Scale.Y = XmlReader->getAttributeValueAsFloat(L"Y");
				detailObject.Scale.Z = XmlReader->getAttributeValueAsFloat(L"Z");
			}
			else if (irr::core::stringw("Distance") == XmlReader->getNodeName())
			{
				detailObject.DistanceSampling = XmlReader->getAttributeValueAsFloat(L"Sampling");
				detailObject.DistanceNumFactor = XmlReader->getAttributeValueAsInt(L"NumFactor");
				detailObject.DistanceNumMin = XmlReader->getAttributeValueAsInt(L"NumMin");
				detailObject.DistanceNumMax = XmlReader->getAttributeValueAsInt(L"NumMax");

			}
			else if (irr::core::stringw("FirstAndLast") == XmlReader->getNodeName())
			{
				detailObject.DistanceNumMinFirstElement = XmlReader->getAttributeValueAsInt(L"NumMinFirst");
				detailObject.DistanceNumMaxFirstElement = XmlReader->getAttributeValueAsInt(L"NumMaxFirst");
				detailObject.ObjectAtT1 = (  irr::core::stringw("0") != XmlReader->getAttributeValue(L"ObjectAt1") );
			}
		}
		// closing tag reached, detailobject can be added
		else if (irr::core::stringw("Detailobject") == XmlReader->getNodeName())
		{
			DunGenInterface->CorrdidorAddDetailObject(detailObject);
		}
	}
}

void DunGen::CDunGenXMLReader::ReadCorridorRoomRoom()
{
	// read end 0:
	// parse to 'Room' opening tag
	while(XmlReader->read() && (XmlReader->getNodeType() == irr::io::EXN_ELEMENT) && (irr::core::stringw("Room") != XmlReader->getNodeName()) )
	{
	}
	unsigned int index0 = XmlReader->getAttributeValueAsInt(L"Index");
	unsigned int dockingSite0 = XmlReader->getAttributeValueAsInt(L"DockingSite");
	double distance0 = XmlReader->getAttributeValueAsFloat(L"Distance");
	double strength0 = XmlReader->getAttributeValueAsFloat(L"Strength");

	// read end 1:
	// parse to 'Room' opening tag
	while(XmlReader->read() && (XmlReader->getNodeType() == irr::io::EXN_ELEMENT) && (irr::core::stringw("Room") != XmlReader->getNodeName()) )
	{
	}
	unsigned int index1 = XmlReader->getAttributeValueAsInt(L"Index");
	unsigned int dockingSite1 = XmlReader->getAttributeValueAsInt(L"DockingSite");
	double distance1 = XmlReader->getAttributeValueAsFloat(L"Distance");
	double strength1 = XmlReader->getAttributeValueAsFloat(L"Strength");

	// create
	bool tmp;
	DunGenInterface->CreateCorridorRoomRoom(index0, dockingSite0, distance0, strength0, index1, dockingSite1, distance1, strength1, tmp);

	// parse to the closing tag
	while(XmlReader->read() && (XmlReader->getNodeType() == irr::io::EXN_ELEMENT_END) && (irr::core::stringw("CorridorRoomRoom") != XmlReader->getNodeName()) )
	{
	}
}

void DunGen::CDunGenXMLReader::ReadCorridorRoomCave()
{
	// read end 0:
	// parse to 'Room' opening tag
	while(XmlReader->read() && (XmlReader->getNodeType() == irr::io::EXN_ELEMENT) && (irr::core::stringw("Room") != XmlReader->getNodeName()) )
	{
	}
	unsigned int index0 = XmlReader->getAttributeValueAsInt(L"Index");
	unsigned int dockingSite0 = XmlReader->getAttributeValueAsInt(L"DockingSite");
	double distance0 = XmlReader->getAttributeValueAsFloat(L"Distance");
	double strength0 = XmlReader->getAttributeValueAsFloat(L"Strength");

	// read end 1:
	// parse to 'Cave' opening tag
	while(XmlReader->read() && (XmlReader->getNodeType() == irr::io::EXN_ELEMENT) && (irr::core::stringw("Cave") != XmlReader->getNodeName()) )
	{
	}
	EDirection::Enum direction1 = static_cast<EDirection::Enum>(XmlReader->getAttributeValueAsInt(L"Direction"));
	double distance1 = XmlReader->getAttributeValueAsFloat(L"Distance");
	double strength1 = XmlReader->getAttributeValueAsFloat(L"Strength");
	
	// parse to 'MinVox' opening tag
	while(XmlReader->read() && (XmlReader->getNodeType() == irr::io::EXN_ELEMENT) && (irr::core::stringw("MinVox") != XmlReader->getNodeName()) )
	{
	}
	irr::core::vector3d<unsigned int> minVox1;
	minVox1.X = XmlReader->getAttributeValueAsInt(L"X");
	minVox1.Y = XmlReader->getAttributeValueAsInt(L"Y");
	minVox1.Z = XmlReader->getAttributeValueAsInt(L"Z");

	// parse to 'MaxVox' opening tag
	while(XmlReader->read() && (XmlReader->getNodeType() == irr::io::EXN_ELEMENT) && (irr::core::stringw("MaxVox") != XmlReader->getNodeName()) )
	{
	}
	irr::core::vector3d<unsigned int> maxVox1;
	maxVox1.X = XmlReader->getAttributeValueAsInt(L"X");
	maxVox1.Y = XmlReader->getAttributeValueAsInt(L"Y");
	maxVox1.Z = XmlReader->getAttributeValueAsInt(L"Z");

	// create
	bool tmp;
	DunGenInterface->CreateCorridorRoomCave(index0, dockingSite0, distance0, strength0, minVox1, maxVox1, direction1, distance1, strength1, tmp);
	
	// parse to the closing tag
	while(XmlReader->read() && (XmlReader->getNodeType() == irr::io::EXN_ELEMENT_END) && (irr::core::stringw("CorridorRoomCave") != XmlReader->getNodeName()) )
	{
	}
}

void DunGen::CDunGenXMLReader::ReadCorridorCaveCave()
{
	// read end 0:
	// parse to 'MinVox' opening tag
	while(XmlReader->read() && (XmlReader->getNodeType() == irr::io::EXN_ELEMENT) && (irr::core::stringw("Cave") != XmlReader->getNodeName()) )
	{
	}
	EDirection::Enum direction0 = static_cast<EDirection::Enum>(XmlReader->getAttributeValueAsInt(L"Direction"));
	double distance0 = XmlReader->getAttributeValueAsFloat(L"Distance");
	double strength0 = XmlReader->getAttributeValueAsFloat(L"Strength");
	
	// parse to 'MinVox' opening tag
	while(XmlReader->read() && (XmlReader->getNodeType() == irr::io::EXN_ELEMENT) && (irr::core::stringw("MinVox") != XmlReader->getNodeName()) )
	{
	}
	irr::core::vector3d<unsigned int> minVox0;
	minVox0.X = XmlReader->getAttributeValueAsInt(L"X");
	minVox0.Y = XmlReader->getAttributeValueAsInt(L"Y");
	minVox0.Z = XmlReader->getAttributeValueAsInt(L"Z");

	irr::core::stringw test = XmlReader->getNodeName();

	// parse to 'MinVox' opening tag
	while(XmlReader->read() && (XmlReader->getNodeType() == irr::io::EXN_ELEMENT) && (irr::core::stringw("MaxVox") != XmlReader->getNodeName()) )
	{
	}
	irr::core::vector3d<unsigned int> maxVox0;
	maxVox0.X = XmlReader->getAttributeValueAsInt(L"X");
	maxVox0.Y = XmlReader->getAttributeValueAsInt(L"Y");
	maxVox0.Z = XmlReader->getAttributeValueAsInt(L"Z");

	// proceed to ending tag
	while(XmlReader->read() && (XmlReader->getNodeType() == irr::io::EXN_ELEMENT_END) && (irr::core::stringw("Cave") != XmlReader->getNodeName()) )
	{
	}

	// read end 1:
	// parse to 'Cave' opening tag
	while(XmlReader->read() && (XmlReader->getNodeType() == irr::io::EXN_ELEMENT) && (irr::core::stringw("Cave") != XmlReader->getNodeName()) )
	{
	}
	EDirection::Enum direction1 = static_cast<EDirection::Enum>(XmlReader->getAttributeValueAsInt(L"Direction"));
	double distance1 = XmlReader->getAttributeValueAsFloat(L"Distance");
	double strength1 = XmlReader->getAttributeValueAsFloat(L"Strength");
	
	// parse to 'MinVox' opening tag
	while(XmlReader->read() && (XmlReader->getNodeType() == irr::io::EXN_ELEMENT) && (irr::core::stringw("MinVox") != XmlReader->getNodeName()) )
	{
	}
	irr::core::vector3d<unsigned int> minVox1;
	minVox1.X = XmlReader->getAttributeValueAsInt(L"X");
	minVox1.Y = XmlReader->getAttributeValueAsInt(L"Y");
	minVox1.Z = XmlReader->getAttributeValueAsInt(L"Z");

	// parse to 'MaxVox' opening tag
	while(XmlReader->read() && (XmlReader->getNodeType() == irr::io::EXN_ELEMENT) && (irr::core::stringw("MaxVox") != XmlReader->getNodeName()) )
	{
	}
	irr::core::vector3d<unsigned int> maxVox1;
	maxVox1.X = XmlReader->getAttributeValueAsInt(L"X");
	maxVox1.Y = XmlReader->getAttributeValueAsInt(L"Y");
	maxVox1.Z = XmlReader->getAttributeValueAsInt(L"Z");

	// create
	bool tmp;
	DunGenInterface->CreateCorridorCaveCave(minVox0, maxVox0, direction0, distance0, strength0, minVox1, maxVox1, direction1, distance1, strength1, tmp);
	
	// parse to the closing tag
	while(XmlReader->read() && (XmlReader->getNodeType() == irr::io::EXN_ELEMENT_END) && (irr::core::stringw("CorridorCaveCave") != XmlReader->getNodeName()) )
	{
	}
}