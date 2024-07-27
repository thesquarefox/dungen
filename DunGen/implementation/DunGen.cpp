// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#include "interface/DunGen.h"
#include "DunGenXMLReader.h"
#include "DungeonGenerator.h"
#include "LSystem.h"
#include "MaterialProvider.h"
#include "MeshCave.h"
#include "VoxelCave.h"

DunGen::CDunGen::CDunGen(irr::IrrlichtDevice* irrDevice)
: DungeonGenerator(NULL)
, DunGenXMLReader(NULL)
, MaterialProvider(new CMaterialProvider(irrDevice))
, IrrDevice(irrDevice)
{
}

DunGen::CDunGen::~CDunGen()
{
	delete DunGenXMLReader;
	delete DungeonGenerator;
	delete MaterialProvider;
}

void DunGen::CDunGen::StartUp()
{
	if (!DungeonGenerator)
		DungeonGenerator = new CDungeonGenerator(IrrDevice, MaterialProvider);
	if (!DunGenXMLReader)
		DunGenXMLReader = new CDunGenXMLReader(IrrDevice, this);
}

void DunGen::CDunGen::ShutDown()
{
	delete DunGenXMLReader;
	DunGenXMLReader = NULL;
	delete DungeonGenerator;
	DungeonGenerator = NULL;
}

void DunGen::CDunGen::ClearRoomsAndCorridors()
{
	if (DungeonGenerator)
		DungeonGenerator->ClearRoomsAndCorridors();
}

unsigned int DunGen::CDunGen::CreateLSystemDerivation(unsigned int desiredIteration)
{
	if (DungeonGenerator)
		return DungeonGenerator->CreateLSystemDerivation(desiredIteration);
	else
		return 0;
}

void DunGen::CDunGen::CreateVoxelCave()
{
	if (DungeonGenerator)
		DungeonGenerator->CreateVoxelCave();
}

void DunGen::CDunGen::ErodeVoxelCave(double erosionLikelihood)
{
	if (DungeonGenerator)
		DungeonGenerator->ErodeVoxelCave(erosionLikelihood);
}

void DunGen::CDunGen::RemoveHoveringVoxelFragments()
{
	if (DungeonGenerator)
		DungeonGenerator->RemoveHoveringVoxelFragments();
}

void DunGen::CDunGen::CreateMeshCave()
{
	if (DungeonGenerator)
		DungeonGenerator->CreateMeshCave();
}

bool DunGen::CDunGen::CreateRoom(unsigned int roompatternIndex, const irr::core::vector3d<double>& position,
	const irr::core::vector3d<double>& rotation, const irr::core::vector3d<double>& scaleFactor)
{
	if (DungeonGenerator)
		return DungeonGenerator->CreateRoom(roompatternIndex, position, rotation, scaleFactor);
	else
		return false;
}

bool DunGen::CDunGen::CreateCorridorRoomRoom(
	unsigned int room0, unsigned int dockingSite0, double distance0, double strenght0,
	unsigned int room1, unsigned int dockingSite1, double distance1, double strenght1,
	bool& sightBlocking)
{
	if (DungeonGenerator)
		return DungeonGenerator->CreateCorridorRoomRoom(
			room0, dockingSite0, distance0, strenght0, room1, dockingSite1, distance1, strenght1, sightBlocking);
	else
		return false;
}

bool DunGen::CDunGen::CreateCorridorRoomCave(
	unsigned int room0, unsigned int dockingSite0, double distance0, double strenght0,
	const irr::core::vector3d<unsigned int>& minVox1, const irr::core::vector3d<unsigned int>& maxVox1,
	EDirection::Enum direction1, double distance1, double strenght1,
	bool& sightBlocking)
{
	if (DungeonGenerator)
		return DungeonGenerator->CreateCorridorRoomCave(
			room0, dockingSite0, distance0, strenght0, minVox1, maxVox1, direction1, distance1, strenght1, sightBlocking);
	else
		return false;
}

bool DunGen::CDunGen::CreateCorridorCaveCave(
	const irr::core::vector3d<unsigned int>& minVox0, const irr::core::vector3d<unsigned int>& maxVox0,
	EDirection::Enum direction0, double distance0, double strenght0,
	const irr::core::vector3d<unsigned int>& minVox1, const irr::core::vector3d<unsigned int>& maxVox1,
	EDirection::Enum direction1, double distance1, double strenght1,
	bool& sightBlocking)
{
	if (DungeonGenerator)
		return DungeonGenerator->CreateCorridorCaveCave(
			minVox0, maxVox0, direction0, distance0, strenght0, minVox1, maxVox1, direction1, distance1, strenght1, sightBlocking);
	else
		return false;
}

bool DunGen::CDunGen::ReadDungeonFromFile(const irr::io::path& filename)
{
	if (DunGenXMLReader)
		return DunGenXMLReader->Read(filename);
	else
		return false;
}

void DunGen::CDunGen::AddDungeon(irr::scene::ISceneNode* parentNode, irr::scene::ISceneManager* sceneManager)
{
	if (DungeonGenerator)
		DungeonGenerator->AddDungeon(parentNode, sceneManager);
}

void DunGen::CDunGen::RandomGeneratorSetParameters(unsigned int seed, unsigned int a, unsigned int c, unsigned int m)
{
	if (DungeonGenerator)
		DungeonGenerator->RandomGeneratorSetParameters(seed, a, c, m);
}

void DunGen::CDunGen::LSystemAddRule(char symbol, const std::string& substitution)
{
	if (DungeonGenerator)
		DungeonGenerator->GetLSystem()->AddRule(symbol, substitution);
}

void DunGen::CDunGen::LSystemDeleteRules()
{
	if (DungeonGenerator)
		DungeonGenerator->GetLSystem()->DeleteRules();
}

void DunGen::CDunGen::LSystemSetStart(const std::string& start)
{
	if (DungeonGenerator)
		DungeonGenerator->GetLSystem()->SetStart(start);
}

void DunGen::CDunGen::LSystemSetParameter(ELSystemParameter::Enum parameter, double value)
{
	if (DungeonGenerator)
		DungeonGenerator->GetLSystem()->SetParameter(parameter, value);
}

void DunGen::CDunGen::VoxelCaveSetParameters(unsigned int border, unsigned int mindrawradius)
{
	if (DungeonGenerator)
	{
		DungeonGenerator->GetVoxelCave()->SetBorder(border);
		DungeonGenerator->GetVoxelCave()->SetMinDrawRadius(mindrawradius);
	}
}

unsigned int DunGen::CDunGen::VoxelCaveEstimateMeshComplexity() const
{
	if (DungeonGenerator)
		return DungeonGenerator->GetVoxelCave()->EstimateMeshComplexity();
	else
		return 0;
}

void DunGen::CDunGen::MeshCaveSetWarpParameters(bool warpEnabled, bool smoothEnabled, unsigned int warpRandomSeed, double warpStrength)
{
	if (DungeonGenerator)
	{
		DungeonGenerator->GetMeshCave()->SetWarpOption(warpEnabled);
		DungeonGenerator->GetMeshCave()->SetSmoothOption(smoothEnabled);
		DungeonGenerator->GetMeshCave()->SetWarpRandomSeed(warpRandomSeed);
		DungeonGenerator->GetMeshCave()->SetWarpStrength(warpStrength);
	}
}

void DunGen::CDunGen::MeshCaveSetNormalWeightMethod(ENormalWeightMethod::Enum value)
{
	if (DungeonGenerator)
		DungeonGenerator->GetMeshCave()->SetNormalWeightMethod(value);
}

void DunGen::CDunGen::CorridorSetDistances(double distance, double textureDistance)
{
	if (DungeonGenerator)
		DungeonGenerator->CorridorSetDistances(distance,textureDistance);
}

void DunGen::CDunGen::CorrdidorAddPoint(double x, double y, double textureX)
{
	if (DungeonGenerator)
		DungeonGenerator->CorrdidorAddPoint(x, y, textureX);
}

void DunGen::CDunGen::CorrdidorRemovePoints()
{
	DungeonGenerator->CorrdidorRemovePoints();
}

void DunGen::CDunGen::CorrdidorAddDetailObject(const SDetailobjectParameters& parameters)
{
	if (DungeonGenerator)
		DungeonGenerator->CorrdidorAddDetailObject(parameters);
}

void DunGen::CDunGen::CorrdidorRemoveDetailobjects()
{
	if (DungeonGenerator)
		DungeonGenerator->CorrdidorRemoveDetailobjects();
}

void DunGen::CDunGen::RoomPatternLoad(const irr::io::path filename)
{
	if (DungeonGenerator)
		DungeonGenerator->RoomPatternLoad(filename);
}

void DunGen::CDunGen::MaterialSetCorridor(const irr::io::path& textureFilename, bool backFaceCulling, bool antiAliasing)
{
	if (DungeonGenerator)
		DungeonGenerator->MaterialSetCorridor(textureFilename, backFaceCulling, antiAliasing);
}

void DunGen::CDunGen::MaterialSetCaveSingleColor(const irr::video::SColorf& color, bool backFaceCulling, bool antiAliasing)
{
	DungeonGenerator->MaterialSetCaveSingleColor(color, backFaceCulling, antiAliasing);
}

void DunGen::CDunGen::MaterialSetCaveMultiColor(bool backFaceCulling, bool antiAliasing)
{
	if (DungeonGenerator)
		DungeonGenerator->MaterialSetCaveMultiColor(backFaceCulling, antiAliasing);
}

void DunGen::CDunGen::MaterialSetCustom(SMaterialType::Enum type, CMaterialDunGen* material, bool backFaceCulling, bool antiAliasing)
{
	if (DungeonGenerator)
		DungeonGenerator->MaterialSetCustom(type, material, backFaceCulling, antiAliasing);
}

void DunGen::CDunGen::SetPrintToConsole(bool value)
{
	if (DungeonGenerator)
		DungeonGenerator->SetPrintToConsole(value);
}

unsigned char (&DunGen::CDunGen::GetVoxelSpace())[SVoxelSpace::DimX][SVoxelSpace::DimY][SVoxelSpace::DimZ]
{
	// no test for DungeonGenerator here, what shall we return if it doesn't exist?
	return DungeonGenerator->GetVoxelCave()->GetVoxelSpace();
}