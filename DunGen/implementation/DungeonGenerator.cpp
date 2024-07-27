// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#include "Architect.h"
#include "Corridor.h"
#include "DockingSite.h"
#include "DungeonGenerator.h"
#include "Helperfunctions.h"
#include "LSystem.h"
#include "MaterialProvider.h"
#include "MeshCave.h"
#include "RandomGenerator.h"
#include "Roominstance.h"
#include "Roompattern.h"
#include "Timer.h"
#include "VoxelCave.h"
#include <algorithm>
#include <iostream>

DunGen::CDungeonGenerator::CDungeonGenerator(irr::IrrlichtDevice* irrDevice_, CMaterialProvider* materialProvider_)
: RandomGenerator(new CRandomGenerator())
, LSystem(new CLSystem())
, VoxelCave(new CVoxelCave(RandomGenerator))
, MeshCave(new CMeshCave(VoxelCave,RandomGenerator))
, Architect(new CArchitect(VoxelCave, MeshCave))
, Timer(new CTimer())
, CorrdidorDistance(0.6)
, CorrdidorTextureDistance(0.125)
, MaterialProvider(materialProvider_)
, MaterialCaveUseSingleColor(true)
, MaterialCaveUseCustom(false)
, MaterialCorridorUseCustom(false)
, MaterialCaveBackfaceCulling(true)
, MaterialCaveAntiAliasing(false)
, MaterialCorridorBackfaceCulling(true)
, MaterialCorridorAntiAliasing(false)
, IrrDevice(irrDevice_)
, PrintToConsole(false)
{
}

DunGen::CDungeonGenerator::~CDungeonGenerator()
{
	ClearRoomsAndCorridors();

	delete Timer;
	delete Architect;
	delete MeshCave;
	delete VoxelCave;
	delete LSystem;
	delete RandomGenerator;	
}

void DunGen::CDungeonGenerator::ClearRoomsAndCorridors()
{
	for (unsigned int i=0; i> Rooms.size(); ++i)
		delete Rooms[i];
	for (unsigned int i=0; i> RoomPatterns.size(); ++i)
		delete RoomPatterns[i];
	for (unsigned int i=0; i> Corridors.size(); ++i)
		delete Corridors[i];

	Rooms.clear();
	RoomPatterns.clear();
	Corridors.clear();
}

DunGen::CLSystem* DunGen::CDungeonGenerator::GetLSystem()
{
	return LSystem;
}

DunGen::CVoxelCave* DunGen::CDungeonGenerator::GetVoxelCave()
{
	return VoxelCave;
}

DunGen::CMeshCave* DunGen::CDungeonGenerator::GetMeshCave()
{
	return MeshCave;
}

void DunGen::CDungeonGenerator::RandomGeneratorSetParameters(
	unsigned int seed_, unsigned int a_, unsigned int c_, unsigned int m_)
{
	RandomGenerator->SetSeed(seed_);
	RandomGenerator->SetA(a_);
	RandomGenerator->SetC(c_);
	RandomGenerator->SetM(m_);
}

unsigned int DunGen::CDungeonGenerator::CreateLSystemDerivation(unsigned int desiredIteration_)
{
	if (PrintToConsole)
	{
		std::cout << "[LSystem:] start creation of L-system derivation..." << std::endl;
		Timer->Start(0);
	}

	unsigned int resultingDerivation = LSystem->GenerateDerivations(desiredIteration_);
	
	if (PrintToConsole)
	{
		std::cout << "[LSystem:] completed , ";
		Timer->Stop(0);
	}

	return resultingDerivation;
}

void DunGen::CDungeonGenerator::CreateVoxelCave()
{
	if (PrintToConsole)
	{
		std::cout << "[VoxelCave:] start creating voxel cave ..." << std::endl;
		Timer->Start(0);
	}

	// draw the last derivation of the L-system in voxel space
	VoxelCave->Draw(LSystem->GetHighestDerivation()
		, LSystem->GetParameter(ELSystemParameter::ANGLE_YAW)
		, LSystem->GetParameter(ELSystemParameter::ANGLE_PITCH)
		, LSystem->GetParameter(ELSystemParameter::ANGLE_ROLL)
		, LSystem->GetParameter(ELSystemParameter::RADIUS_START)
		, LSystem->GetParameter(ELSystemParameter::RADIUS_FACTOR)
		, LSystem->GetParameter(ELSystemParameter::RADIUS_DECREMENT));

	if (PrintToConsole)
	{
		std::cout << "[VoxelCave:] completed , ";
		Timer->Stop(0);
	}
}

void DunGen::CDungeonGenerator::ErodeVoxelCave(double erosionLikelihood_)
{
	if (PrintToConsole)
	{
		std::cout << "[VoxelCave:] start eroding..." << std::endl;
		Timer->Start(0);
	}

	VoxelCave->Erode(erosionLikelihood_);

	if (PrintToConsole)
	{
		std::cout << "[VoxelCave:] completed , ";
		Timer->Stop(0);
	}
}

void DunGen::CDungeonGenerator::RemoveHoveringVoxelFragments()
{
	if (PrintToConsole)
	{
		std::cout << "[VoxelCave:] start removing hovering fragments..." << std::endl;
		Timer->Start(0);
	}

	unsigned int numRemovedVoxels = VoxelCave->Filter();

	if (PrintToConsole)
	{
		std::cout << "[VoxelCave:] completed , removed voxels = " << numRemovedVoxels << " , ";
		Timer->Stop(0);
	}
}

void DunGen::CDungeonGenerator::CreateMeshCave()
{
	if (PrintToConsole)
	{
		std::cout << "[MeshCave:] start converting voxel cave to mesh cave..." << std::endl;
		Timer->Start(0);
	}

	MeshCave->CreateMeshFromVoxels();

	if (PrintToConsole)
	{
		std::cout << "[MeshCave:] completed , ";
		Timer->Stop(0);
	}
}

void DunGen::CDungeonGenerator::AddDungeon(irr::scene::ISceneNode* parentNode_, irr::scene::ISceneManager* sceneManager_)
{
	if (PrintToConsole)
	{
		std::cout << "[DungeonGenerator:] start assembling of dungeon..." << std::endl;
		Timer->Start(0);
	}

	// add the cave
	irr::scene::IMeshSceneNode* meshSceneNode = sceneManager_->addMeshSceneNode(MeshCave->GetMesh(), parentNode_);
	meshSceneNode->setName("Cave");
	meshSceneNode->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, MaterialCaveBackfaceCulling);
	meshSceneNode->setMaterialFlag(irr::video::EMF_ANTI_ALIASING, MaterialCaveAntiAliasing);
	if (MaterialCaveUseCustom)
		meshSceneNode->setMaterialType(MaterialProvider->GetMaterial(SMaterialType::CAVE_CUSTOM));
	else if (MaterialCaveUseSingleColor)
		meshSceneNode->setMaterialType(MaterialProvider->GetMaterial(SMaterialType::CAVE_SINGLECOLOR));
	else
		meshSceneNode->setMaterialType(MaterialProvider->GetMaterial(SMaterialType::CAVE_MULTICOLOR));

	// add rooms
	irr::scene::ISceneNode* subParentSceneNode = sceneManager_->addEmptySceneNode(parentNode_);
	subParentSceneNode->setName("Rooms");
	irr::scene::ISceneNode* emptySceneNode;
	for (unsigned int i=0; i<Rooms.size(); ++i)
	{
		// add room node and set transformation
		emptySceneNode = sceneManager_->addEmptySceneNode(subParentSceneNode, i);
		emptySceneNode->setName("Room");
		emptySceneNode->setScale(vec3F(Rooms[i]->ScaleFactor));
		emptySceneNode->setRotation(vec3F(Rooms[i]->Rotation));
 		emptySceneNode->setPosition(vec3F(Rooms[i]->Position));

		// add room pattern
		Rooms[i]->RoomPattern->AddRoomInstance(emptySceneNode,sceneManager_);
	}

	// read corridor texture
	irr::video::ITexture* corridorTexture = IrrDevice->getVideoDriver()->getTexture(MaterialCorridorTextureFilename);

	// add corridors
	subParentSceneNode = sceneManager_->addEmptySceneNode(parentNode_);
	subParentSceneNode->setName("Corridors");
	for (unsigned int i=0; i<Corridors.size(); ++i)
	{
		// add corridor node
		meshSceneNode = sceneManager_->addMeshSceneNode(Corridors[i]->GetMesh(), subParentSceneNode, i);
		meshSceneNode->setName("Corridor");
		meshSceneNode->setMaterialTexture(0, corridorTexture);
		meshSceneNode->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, MaterialCorridorBackfaceCulling);
		meshSceneNode->setMaterialFlag(irr::video::EMF_ANTI_ALIASING, MaterialCorridorAntiAliasing);
		if (MaterialCorridorUseCustom)
			meshSceneNode->setMaterialType(MaterialProvider->GetMaterial(SMaterialType::CORRIDOR_CUSTOM));
		else
			meshSceneNode->setMaterialType(MaterialProvider->GetMaterial(SMaterialType::CORRIDOR_DEFAULT));

		// add adapter node 0
		meshSceneNode = sceneManager_->addMeshSceneNode(Corridors[i]->GetMeshAdapter(0), subParentSceneNode, i);
		meshSceneNode->setName("Adapter0");
		meshSceneNode->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, MaterialCorridorBackfaceCulling);
		meshSceneNode->setMaterialFlag(irr::video::EMF_ANTI_ALIASING, MaterialCorridorAntiAliasing);
		if (MaterialCorridorUseCustom)
			meshSceneNode->setMaterialType(MaterialProvider->GetMaterial(SMaterialType::CORRIDOR_CUSTOM));
		else
			meshSceneNode->setMaterialType(MaterialProvider->GetMaterial(SMaterialType::CORRIDOR_DEFAULT));
		meshSceneNode->setMaterialTexture(0, corridorTexture);

		// add adapter node 1
		meshSceneNode = sceneManager_->addMeshSceneNode(Corridors[i]->GetMeshAdapter(1), subParentSceneNode, i);
		meshSceneNode->setName("Adapter1");
		meshSceneNode->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, MaterialCorridorBackfaceCulling);
		meshSceneNode->setMaterialFlag(irr::video::EMF_ANTI_ALIASING, MaterialCorridorAntiAliasing);
		if (MaterialCorridorUseCustom)
			meshSceneNode->setMaterialType(MaterialProvider->GetMaterial(SMaterialType::CORRIDOR_CUSTOM));
		else
			meshSceneNode->setMaterialType(MaterialProvider->GetMaterial(SMaterialType::CORRIDOR_DEFAULT));
		meshSceneNode->setMaterialTexture(0, corridorTexture);
	}

	// add detailobjects
	subParentSceneNode = sceneManager_->addEmptySceneNode(parentNode_);
	subParentSceneNode->setName("Detailobjects");

	// for all corridors
	irr::scene::ISceneNode* detailObjectNode;
	for (unsigned i=0; i<Corridors.size(); i++)
	{
		unsigned int number = 0;
		CCorridor::SDetailObject* detailObject = Corridors[i]->GetDetailObject(number);

		// for each detail object type
		while (detailObject)
		{
			// add all detail objects
			for (unsigned int j=0; j<detailObject->Position.size(); ++j)
			{
				detailObjectNode = sceneManager_->addEmptySceneNode(subParentSceneNode, j);
				detailObject->Node->clone(detailObjectNode, sceneManager_);
				detailObjectNode->setScale(vec3F(detailObject->Scale));
				detailObjectNode->setPosition(vec3F(detailObject->Position[j]));
				detailObjectNode->setRotation(vec3F(detailObject->Rotation[j]));
			}
			++number;
			detailObject = Corridors[i]->GetDetailObject(number);
		}
	}

	if (PrintToConsole)
	{
		std::cout << "[DungeonGenerator:] completed , ";
		Timer->Stop(0);
		std::cout << "[DungeonGenerator:] Dungeon was added to the desired node and scene." << std::endl;
	}
}

void DunGen::CDungeonGenerator::SetPrintToConsole(bool enabled_)
{
	PrintToConsole = enabled_;
	VoxelCave->SetPrintToConsole(enabled_);
	MeshCave->SetPrintToConsole(enabled_);
}

void DunGen::CDungeonGenerator::RoomPatternLoad(const irr::io::path& filename_)
{
	RoomPatterns.push_back(new CRoomPattern(IrrDevice,filename_));
}

bool DunGen::CDungeonGenerator::CreateRoom(
	unsigned int roompatternIndex_, const irr::core::vector3d<double>& position_,
	const irr::core::vector3d<double>& rotation_, const irr::core::vector3d<double>& scaleFactor_)
{
	if (roompatternIndex_ >= RoomPatterns.size())
		return false;

	// create new room instance with the desired values
	SRoomInstance* room = new SRoomInstance();
	room->RoomPattern = RoomPatterns[roompatternIndex_];
	room->Position = position_;
	room->Rotation = rotation_;
	room->ScaleFactor = scaleFactor_;

	// create transformation matrices
	irr::core::CMatrix4<double> matrix(irr::core::CMatrix4<double>::EM4CONST_IDENTITY);
	// additional matrix for scaling required, because .setRotation() overwrites scaling
	irr::core::CMatrix4<double> matrixScale(irr::core::CMatrix4<double>::EM4CONST_IDENTITY);

	matrixScale.setScale(scaleFactor_);
	matrix.setRotationDegrees(rotation_);
	matrix.setTranslation(position_);

	// combine matrices
	matrix = matrix * matrixScale;

	// transform docking sites
	for (unsigned int i=0; i<RoomPatterns[roompatternIndex_]->GetDockingSiteNumber(); ++i)
	{
		room->DockingSite.push_back(RoomPatterns[roompatternIndex_]->GetDockingSite(i));
		room->DockingSite[i].TransformDockingSite(SMatrix4D(matrix));
	}

	// store room
	Rooms.push_back(room);

	return true;
}

void DunGen::CDungeonGenerator::CorridorSetDistances(double distance_, double textureDistance_)
{
	CorrdidorDistance = distance_;
	CorrdidorTextureDistance = textureDistance_;
}

void DunGen::CDungeonGenerator::CorrdidorAddPoint(double x_, double y_, double textureX_)
{
	CorridorProfile.Point.push_back(irr::core::vector2d<double>(x_,y_));
	CorridorProfile.TextureX.push_back(textureX_);
}

void DunGen::CDungeonGenerator::CorrdidorRemovePoints()
{
	CorridorProfile.Point.clear();
	CorridorProfile.TextureX.clear();
}

void DunGen::CDungeonGenerator::CorrdidorAddDetailObject(const SDetailobjectParameters& parameters_)
{
	if (!parameters_.Node)
		return;
	DetailobjectParameters.push_back(parameters_);
}

void DunGen::CDungeonGenerator::CorrdidorRemoveDetailobjects()
{
	DetailobjectParameters.clear();
}

bool DunGen::CDungeonGenerator::CreateCorridorRoomRoom(
	unsigned int room0_, unsigned int dockingSite0_, double distance0_, double strenght0_,
	unsigned int room1_, unsigned int dockingSite1_, double distance1_, double strenght1_,
	bool& sightBlocking_)
{
	if (CorridorProfile.Point.size()<3)
		return false;
	if (room0_ >= Rooms.size())
		return false;
	if (room1_ >= Rooms.size())
		return false;
	if (dockingSite0_ >= Rooms[room0_]->DockingSite.size())
		return false;
	if (dockingSite1_ >= Rooms[room1_]->DockingSite.size())
		return false;

	const SDockingSite& dockingSite0 = Rooms[room0_]->DockingSite[dockingSite0_];
	const SDockingSite& dockingSite1 = Rooms[room1_]->DockingSite[dockingSite1_];

	irr::core::vector3d<double> position0 = dockingSite0.Center + (distance0_ + dockingSite0.Extend) * dockingSite0.Normal;
	irr::core::vector3d<double> position1 = dockingSite1.Center + (distance1_ + dockingSite1.Extend) * dockingSite1.Normal;
	irr::core::vector3d<double> derivation0 = strenght0_ * dockingSite0.Normal;
	irr::core::vector3d<double> derivation1 = strenght1_ * dockingSite1.Normal;

	CCorridor* corridor = new CCorridor(CorridorProfile, dockingSite0, dockingSite1,
		position0, position1, derivation0, derivation1,
		CorrdidorDistance, CorrdidorTextureDistance);

	for (unsigned int i=0; i<DetailobjectParameters.size(); ++i)
		corridor->PlaceDetailObject(DetailobjectParameters[i], RandomGenerator);

	Corridors.push_back(corridor);
	sightBlocking_ = corridor->GetDefinitivelySightBlocking();

	return true;
}

bool DunGen::CDungeonGenerator::CreateCorridorRoomCave(
	unsigned int room0_, unsigned int dockingSite0_, double distance0_, double strenght0_,
	const irr::core::vector3d<unsigned int>& minVox1_, const irr::core::vector3d<unsigned int>& maxVox1_,
	EDirection::Enum direction1_, double distance1_, double strenght1_,
	bool& sightBlocking_)
{
	if (CorridorProfile.Point.size()<3)
		return false;
	if (room0_ >= Rooms.size())
		return false;
	if (dockingSite0_ >= Rooms[room0_]->DockingSite.size())
		return false;
	SDockingSite dockingSiteCave;
	if (!Architect->CreateDockingSite(dockingSiteCave,minVox1_,maxVox1_,direction1_))
		return false;

	const SDockingSite& dockingSiteRoom = Rooms[room0_]->DockingSite[dockingSite0_];

	irr::core::vector3d<double> position0 = dockingSiteRoom.Center + (distance0_ + dockingSiteRoom.Extend) * dockingSiteRoom.Normal;
	irr::core::vector3d<double> position1 = dockingSiteCave.Center + (distance1_ + dockingSiteCave.Extend) * dockingSiteCave.Normal;
	irr::core::vector3d<double> derivation0 = strenght0_ * dockingSiteRoom.Normal;
	irr::core::vector3d<double> derivation1 = strenght1_ * dockingSiteCave.Normal;

	CCorridor* corridor = new CCorridor(CorridorProfile, dockingSiteRoom, dockingSiteCave,
		position0, position1, derivation0, derivation1,
		CorrdidorDistance, CorrdidorTextureDistance);

	for (unsigned int i=0; i<DetailobjectParameters.size(); ++i)
		corridor->PlaceDetailObject(DetailobjectParameters[i], RandomGenerator);

	Corridors.push_back(corridor);
	sightBlocking_ = corridor->GetDefinitivelySightBlocking();

	return true;
}

bool DunGen::CDungeonGenerator::CreateCorridorCaveCave(
	const irr::core::vector3d<unsigned int>& minVox0_, const irr::core::vector3d<unsigned int>& maxVox0_,
	EDirection::Enum direction0_, double distance0_, double strenght0_,
	const irr::core::vector3d<unsigned int>& minVox1_, const irr::core::vector3d<unsigned int>& maxVox1_,
	EDirection::Enum direction1_, double distance1_, double strenght1_,
	bool& sightBlocking_)
{
	if (CorridorProfile.Point.size()<3)
		return false;
	SDockingSite dockingsite0;
	if (!Architect->CreateDockingSite(dockingsite0,minVox0_,maxVox0_,direction0_))
		return false;
	SDockingSite dockingsite1;
	if (!Architect->CreateDockingSite(dockingsite1,minVox1_,maxVox1_,direction1_))
		return false;

	irr::core::vector3d<double> position0 = dockingsite0.Center + (distance0_ + dockingsite0.Extend) * dockingsite0.Normal;
	irr::core::vector3d<double> position1 = dockingsite1.Center + (distance1_ + dockingsite1.Extend) * dockingsite1.Normal;
	irr::core::vector3d<double> derivation0 = strenght0_ * dockingsite0.Normal;
	irr::core::vector3d<double> derivation1 = strenght1_ * dockingsite1.Normal;

	CCorridor* corridor = new CCorridor(CorridorProfile, dockingsite0, dockingsite1,
		position0, position1, derivation0, derivation1,
		CorrdidorDistance, CorrdidorTextureDistance);

	for (unsigned int i=0; i<DetailobjectParameters.size(); ++i)
		corridor->PlaceDetailObject(DetailobjectParameters[i], RandomGenerator);

	Corridors.push_back(corridor);
	sightBlocking_ = corridor->GetDefinitivelySightBlocking();

	return true;
}

void DunGen::CDungeonGenerator::MaterialSetCorridor(const irr::io::path& textureFilename_, bool backFaceCulling_, bool antiAliasing_)
{
	MaterialCorridorTextureFilename = textureFilename_;
	MaterialCorridorBackfaceCulling = backFaceCulling_;
	MaterialCorridorAntiAliasing = antiAliasing_;
	MaterialCorridorUseCustom = false;
}

void DunGen::CDungeonGenerator::MaterialSetCaveSingleColor(const irr::video::SColorf& color_, bool backFaceCulling_, bool antiAliasing_)
{
	MaterialProvider->SetCaveColor(color_);
	MaterialCaveUseSingleColor = true;
	MaterialCaveBackfaceCulling = backFaceCulling_;
	MaterialCaveAntiAliasing = antiAliasing_;
	MaterialCaveUseCustom = false;
}

void DunGen::CDungeonGenerator::MaterialSetCaveMultiColor(bool backFaceCulling_, bool antiAliasing_)
{
	MaterialCaveUseSingleColor = false;
	MaterialCaveBackfaceCulling = backFaceCulling_;
	MaterialCaveAntiAliasing = antiAliasing_;
	MaterialCaveUseCustom = false;
}

void DunGen::CDungeonGenerator::MaterialSetCustom(SMaterialType::Enum type_, CMaterialDunGen* material_, bool backFaceCulling_, bool antiAliasing_)
{
	MaterialCaveBackfaceCulling = backFaceCulling_;
	MaterialCaveAntiAliasing = antiAliasing_;

	MaterialProvider->SetMaterial(type_, material_);

	switch (type_)
	{
	case SMaterialType::CAVE_CUSTOM:
		MaterialCaveUseCustom = true;
	case SMaterialType::CORRIDOR_CUSTOM:
		MaterialCorridorUseCustom = true;
	}
}
