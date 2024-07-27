// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef DUNGEONGENERATOR_H
#define DUNGEONGENERATOR_H

#include "interface/ArchitectCommon.h"
#include "interface/MaterialDunGen.h"
#include "Corridor.h"
#include <irrlicht.h>
#include <vector>

// Namespace DunGen : DungeonGenerator
namespace DunGen
{
	class CArchitect;
	class CLSystem;
	class CRandomGenerator;
	class CRoomPattern;
	class CMaterialProvider;
	class CMeshCave;
	class CTimer;
	class CVoxelCave;

	struct SRoomInstance;

	class CDungeonGenerator
	{
	public:
		/// Constructor.
		CDungeonGenerator(irr::IrrlichtDevice* irrDevice_, CMaterialProvider* materialProvider_);

		/// Destructor.
		~CDungeonGenerator();

		/// Deletes all rooms, room patterns and corridors.
		/// This is necessary, if a new dungeon shall be created without deleting and recreating the CDungeonGenerator object.
		void ClearRoomsAndCorridors();

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// generators
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// L-system creation functions:
		/// Create a derivation of the L-system.
		unsigned int CreateLSystemDerivation(unsigned int desiredIteration_);

		// Voxel cave creation functions:
		/// Creates the voxel cave from the currently generated L-system iteration.
		void CreateVoxelCave();
		/// Erodes the voxel cave 1 voxel deep. Can be applied muliple times.
		void ErodeVoxelCave(double erosionLikelihood_);
		/// Removes all hovering fragments.
		void RemoveHoveringVoxelFragments();

		// Mesh cave creation functions:
		/// Creates the mesh cave from the currently generated voxel cave.
		void CreateMeshCave();

		// Corridor creation functions:

		/// Creates a corridor between two rooms.
		bool CreateRoom(unsigned int roompatternIndex_, const irr::core::vector3d<double>& position_,
			const irr::core::vector3d<double>& rotation_, const irr::core::vector3d<double>& scaleFactor_);

		// Corridor creation functions:

		/// Creates a corridor between two rooms.
		bool CreateCorridorRoomRoom(unsigned int room0_, unsigned int dockingSite0_, double distance0_, double strenght0_,
									unsigned int room1_, unsigned int dockingSite1_, double distance1_, double strenght1_,
									bool& sightBlocking_);
		/// Creates a room corridor between a room and the cave. A docking site for the cave is created automatically with direction -direction1.
		bool CreateCorridorRoomCave(unsigned int room0_, unsigned int dockingSite0_, double distance0_, double strenght0_,
									const irr::core::vector3d<unsigned int>& minVox1_, const irr::core::vector3d<unsigned int>& maxVox1_,
									EDirection::Enum direction1_, double distance1_, double strenght1_,
									bool& sightBlocking_);
		/// Creates a room corridor between two cave positions. A docking site for each cave position is created automatically with directions -direction0 and -direction1.
		bool CreateCorridorCaveCave(const irr::core::vector3d<unsigned int>& minVox0_, const irr::core::vector3d<unsigned int>& maxVox0_,
									EDirection::Enum direction0_, double distance0_, double strenght0_,
									const irr::core::vector3d<unsigned int>& minVox1_, const irr::core::vector3d<unsigned int>& maxVox1_,
									EDirection::Enum direction1_, double distance1_, double strenght1_,
									bool& sightBlocking_);

		// Dungeon creation functions:
		/// Assembles the dungeon and adds it under the specified node in the specified scene manager.
		void AddDungeon(irr::scene::ISceneNode* parentNode_, irr::scene::ISceneManager* sceneManager_);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// parameters
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// Get subgenerator (to configure it).
		/// Returns the L-system generator.
		CLSystem* GetLSystem();
		/// Returns the voxel cave generator
		CVoxelCave* GetVoxelCave();
		/// Returns the mesh cave generator.
		CMeshCave* GetMeshCave();

		/// Set the parameters for the random generator: linear congruential generator, X[i+1] = (a*X[i]+c) mod m).
		void RandomGeneratorSetParameters(unsigned int seed_, unsigned int a_, unsigned int c_, unsigned int m_);

		// Corridor parameters:
		/// Sets the distances for the corridor.
		void CorridorSetDistances(double distance_, double textureDistance_);
		/// Adds a point for the corridor profile. The profile has to have at least 3 points and enclose (0,0), which is the center.
		void CorrdidorAddPoint(double x_, double y_, double textureX_);
		/// Removes all points from the corridor profile.
		void CorrdidorRemovePoints();
		/// Adds a detail object template, which shall be placed along the corridor.
		void CorrdidorAddDetailObject(const SDetailobjectParameters& parameters_);
		/// Removes all detail object templates.
		void CorrdidorRemoveDetailobjects();

		/// Loads a room pattern, which has to be a .irr file. See mainpage of documentation of requirements for room patterns.
		void RoomPatternLoad(const irr::io::path& filename_);

		// Material parameters:
		/// Sets material parameters for the corridor.
		void MaterialSetCorridor(const irr::io::path& textureFilename_, bool backFaceCulling_, bool antiAliasing_);
		/// Chooses the multi color material for the mesh cave and sets the parameters for it.
		void MaterialSetCaveSingleColor(const irr::video::SColorf& color_, bool backFaceCulling_, bool antiAliasing_);
		/// Chooses the single color material for the mesh cave and sets the parameters for it.
		void MaterialSetCaveMultiColor(bool backFaceCulling_, bool antiAliasing_);
		/// Sets a custom material.
		void MaterialSetCustom(SMaterialType::Enum type_, CMaterialDunGen* material_, bool backFaceCulling_, bool antiAliasing_);

		/// Sets if status reports shall be printed to the console.
		void SetPrintToConsole(bool enabled_);

	private:
		CRandomGenerator* RandomGenerator;								///< the random generator
		CLSystem* LSystem;												///< the L-system generator
		CVoxelCave* VoxelCave;											///< the voxel cave generator
		CMeshCave* MeshCave;											///< the mesh cave generator
		CArchitect* Architect;											///< the architext for constructing docking sites at caves
		CTimer* Timer;													///< the timer

		std::vector<CRoomPattern*> RoomPatterns;						///< the room patterns
		std::vector<SRoomInstance*> Rooms;								///< the room instance parameters
		std::vector<CCorridor*> Corridors;								///< the corridors

		double CorrdidorDistance;										///< the sampling distance for the corridors
		double CorrdidorTextureDistance;								///< the Y texture coordiante increase per sampling point
		SCorridorProfile CorridorProfile;								///< the profile of the corrdior
		std::vector<SDetailobjectParameters> DetailobjectParameters;	///< the detailobject parameters

		CMaterialProvider* MaterialProvider;							///< the material provider
		bool MaterialCaveUseSingleColor;								///< Shall the cave use the single color material (otherwise the multi color material is been used)?
		bool MaterialCaveUseCustom;										///< Shall the material of the cave be a custom material?
		bool MaterialCorridorUseCustom;									///< Shall the material of the corridor be a custom material?
		bool MaterialCaveBackfaceCulling;								///< Shall backface culling for the cave be enabled?
		bool MaterialCaveAntiAliasing;									///< Shall antialiasing for the cave be enabled?
		irr::io::path MaterialCorridorTextureFilename;					///< the filename for the corridor texture
		bool MaterialCorridorBackfaceCulling;							///< Shall backface culling for the corridor be enabled?
		bool MaterialCorridorAntiAliasing;								///< Shall antialiasing for the corridor be enabled?
		
		irr::IrrlichtDevice* IrrDevice;									///< the irrlicht device

		bool PrintToConsole;											///< Print status reports to console?
	};

} // END NAMESPACE DunGen

#endif