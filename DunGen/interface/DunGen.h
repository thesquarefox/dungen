// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef DUNGEN_H
#define DUNGEN_H

#include "ArchitectCommon.h"
#include "CorridorCommon.h"
#include "LSystemCommon.h"
#include "MaterialDunGen.h"
#include "MeshCaveCommon.h"
#include "VoxelCaveCommon.h"
#include <irrlicht.h>
#include <string>

#define DllExport __declspec(dllexport)

/// Namespace DunGen : DungeonGenerator
namespace DunGen
{
	class CDunGenXMLReader;
	class CDungeonGenerator;
	class CMaterialProvider;

	/// Interfaceclass for the DunGen-DungeonGenerator.
	class DllExport CDunGen
	{
	public:
		/// Constructor.
		/// \param irrDevice The Irrlicht device.
		CDunGen(irr::IrrlichtDevice* irrDevice);

		/// Destructor.
		~CDunGen();

		/// Start up the Dungeongenerator.
		void StartUp();

		/// Shut down the Dungeongenerator. Deletes all unnecessary objects.
		void ShutDown();

		/// Deletes all rooms, room patterns and corridors.
		///
		/// This is necessary, if a new dungeon shall be created without shutdown / startup usage.
		/// The voxel cave and the mesh cave need no explicit reset.
		/// They are cleared automatically when drawing a new cave.
		void ClearRoomsAndCorridors();

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// generators
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// L-system creation functions:

		/// Create a derivation of the L-system.
		/// \param desiredIteration The desired iteration. The derivation will eventually be lower if the length of the generated string would exceed 1,000,000 instructions.
		/// \return The generated iteration.
		unsigned int CreateLSystemDerivation(unsigned int desiredIteration);

		// Voxel cave creation functions:

		/// Creates the voxel cave from the currently generated L-system iteration.
		void CreateVoxelCave();

		/// Erodes the voxel cave 1 voxel deep. Can be applied muliple times.
		/// \param erosionLikelihood The likelihood, with which the voxels are removed.
		void ErodeVoxelCave(double erosionLikelihood);
		
		/// Removes all hovering fragments.
		void RemoveHoveringVoxelFragments();

		// Mesh cave creation functions:

		/// Creates the mesh cave from the currently generated voxel cave.
		void CreateMeshCave();

		// Room creation functions:

		/// Creates a room from a room pattern.
		/// \param roompatternIndex The index of the room pattern. Refers to the rooms that have been added with DunGen::RoomPatternLoad, starts with 0.
		/// \param position	The position of the room.
		/// \param rotation	The rotation of the room.
		/// \param scaleFactor The scale factor of the room.
		/// \return Returns true, if this was successful, false otherwise.
		bool CreateRoom(unsigned int roompatternIndex, const irr::core::vector3d<double>& position,
			const irr::core::vector3d<double>& rotation, const irr::core::vector3d<double>& scaleFactor);

		// Corridor creation functions:

		/// Creates a corridor between two rooms.
		/// \param room0 The index of room 0 which shall be connected. Refers the the rooms, that have been created with DunGen::roompatternIndex, starts with 0.
		/// \param dockingSite0 The docking site of room 0, which shall be used for connecting the corridor.
		/// \param distance0 The distance in which the corridor shall start from docking site 0. The space between the room and the corridor is used by an adapter.
		/// \param strenght0 The strength, with which the corridor direction is influenced by the docking site 0 direction.
		/// \param room1 The index of room 1 which shall be connected. Refers the the rooms, that have been created with DunGen::roompatternIndex, starts with 0.
		/// \param dockingSite1 The docking site of room 1, which shall be used for connecting the corridor.
		/// \param distance1 The distance in which the corridor shall start from docking site 1. The space between the room and the corridor is used by an adapter.
		/// \param strenght1 The strength, with which the corridor direction is influenced by the docking site 1 direction.
		/// \param sightBlocking Is the corridor definitely sight blocking (can you see from one end the the other)?
		/// \return Returns true, if this was successful, false otherwise.
		bool CreateCorridorRoomRoom(unsigned int room0, unsigned int dockingSite0, double distance0, double strenght0,
									unsigned int room1, unsigned int dockingSite1, double distance1, double strenght1,
									bool& sightBlocking);

		/// Creates a room corridor between a room and the cave. A docking site for the cave is created automatically with direction -direction1.
		/// \param room0 The index of room 0 which shall be connected. Refers the the rooms, that have been created with DunGen::roompatternIndex, starts with 0.
		/// \param dockingSite0 The docking site of room 0, which shall be used for connecting the corridor.
		/// \param distance0 The distance in which the corridor shall start from room 0. The space between the room and the corridor is used by an adapter.
		/// \param strenght0 The strength , with which the corridor direction is influenced by the docking site 0 direction.
		/// \param minVox1 The start voxels, where the cave search begins. Assume you have X-direction, Y and Z define the width of the docking site and have to be the same as for maxVox1. The difference of minVox1.X and maxVox1.X is carved into the cave.
		/// \param maxVox1 The end voxels for cave docking. Assume you have X-direction, Y and Z define the width of the docking site and have to be the same as for minVox1. The difference of minVox1.X and maxVox1.X is carved into the cave.
		/// \param direction1 The direction in which the corridor is connected.
		/// \param distance1 The distance in which the corridor shall start from docking site 1. The space between the room and the corridor is used by an adapter.
		/// \param strenght1 The strength , with which the corridor direction is influenced by the docking site 1 direction.
		/// \param sightBlocking Is the corridor definitely sight blocking (can you see from one end the the other)?
		/// \return Returns true, if this was successful, false otherwise.
		bool CreateCorridorRoomCave(unsigned int room0, unsigned int dockingSite0, double distance0, double strenght0,
									const irr::core::vector3d<unsigned int>& minVox1, const irr::core::vector3d<unsigned int>& maxVox1,
									EDirection::Enum direction1, double distance1, double strenght1,
									bool& sightBlocking);

		/// Creates a room corridor between two cave positions. A docking site for each cave position is created automatically with directions -direction0 and -direction1.
		/// \param minVox0 The start voxels, where the cave search begins. Assume you have X-direction, Y and Z define the width of the docking site and have to be the same as for maxVox0. The difference of minVox0.X and maxVox0.X is carved into the cave.
		/// \param maxVox0 The end voxels for cave docking. Assume you have X-direction, Y and Z define the width of the docking site and have to be the same as for minVox0. The difference of minVox0.X and maxVox0.X is carved into the cave.
		/// \param direction0 The direction in which the corridor is connected.
		/// \param distance0 The distance in which the corridor shall start from docking site 0. The space between the room and the corridor is used by an adapter.
		/// \param strenght0 The strength, with which the corridor direction is influenced by the docking site 0 direction.
		/// \param minVox1 The start voxels, where the cave search begins. Assume you have X-direction, Y and Z define the width of the docking site and have to be the same as for maxVox1. The difference of minVox1.X and maxVox1.X is carved into the cave.
		/// \param maxVox1 The end voxels for cave docking. Assume you have X-direction, Y and Z define the width of the docking site and have to be the same as for minVox1. The difference of minVox1.X and maxVox1.X is carved into the cave.
		/// \param direction1 The direction in which the corridor is connected.
		/// \param distance1 The distance in which the corridor shall start from docking site 1. The space between the room and the corridor is used by an adapter.
		/// \param strenght1 The strength , with which the corridor direction is influenced by the docking site 1 direction.
		/// \param sightBlocking Is the corridor definitely sight blocking (can you see from one end the the other)?
		/// \return Returns true, if this was successful, false otherwise.
		bool CreateCorridorCaveCave(const irr::core::vector3d<unsigned int>& minVox0, const irr::core::vector3d<unsigned int>& maxVox0,
									EDirection::Enum direction0, double distance0, double strenght0,
									const irr::core::vector3d<unsigned int>& minVox1, const irr::core::vector3d<unsigned int>& maxVox1,
									EDirection::Enum direction1, double distance1, double strenght1,
									bool& sightBlocking);

		// Dungeon creation functions:

		/// Reads the dungeon from a file. The dungeon creation steps will be done accordingly to the XML description in this file.
		/// \param filename The filename of the file.
		/// \return Returns true, if the file exists and can be processed, false otherwise.
		bool ReadDungeonFromFile(const irr::io::path& filename);

		/// Assembles the dungeon and adds it under the specified node in the specified scene manager.
		/// \param parentNode The parent node for the dungeon.
		/// \param sceneManager The scene manager parentNode belongs to.
		void AddDungeon(irr::scene::ISceneNode* parentNode, irr::scene::ISceneManager* sceneManager);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// parameters
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		/// Set the parameters for the random generator: linear congruential generator, X[i+1] = (a*X[i]+c) mod m).
		/// \param seed The random seed.
		/// \param a The parameter a.
		/// \param c The parameter c.
		/// \param m The parameter m.
		void RandomGeneratorSetParameters(unsigned int seed, unsigned int a, unsigned int c, unsigned int m);

		// L-system parameter functions:

		/// Adds a substitution rule for the L-system.
		/// \param symbol The symbol, which shall be replaced.
		/// \param substitution The substitution for the symbol.
		void LSystemAddRule(char symbol, const std::string& substitution);

		/// Deletes all substitution rules for the L-system.
		void LSystemDeleteRules();

		/// Sets the start symbols for the L-system
		/// \param start The start symbols.
		void LSystemSetStart(const std::string& start);

		/// Sets a parameter for the L-system.
		/// \param parameter The parameter.
		/// \param value The value to be set.
		void LSystemSetParameter(ELSystemParameter::Enum parameter, double value);

		// Voxel cave parameter functions:

		/// Sets the parameters of the voxel cave.
		/// \param border The border of 0-voxels, which will be preserved. Will be capped to at least SVoxelSpace::MinBorder
		/// \param mindrawradius The minimum drawing radius, which will be not deceeded.
		void VoxelCaveSetParameters(unsigned int border, unsigned int mindrawradius);

		/// Estimates the mesh complexity for the mesh cave which results from the current voxel cave.
		/// \returns The estimated number of triangles needed.
		unsigned int VoxelCaveEstimateMeshComplexity() const;

		// Mesh cave parameter functions:

		/// Sets the warp parameters for the mesh cave.
		/// \param warpEnabled Shall the voxels be warped, when creating the mesh cave?
		/// \param smoothEnabled Shall the warping smooth the mesh?
		/// \param warpRandomSeed Random seed for the warping.
		/// \param warpStrength Strength of the warping. Will be clamped to [0,0.49].
		void MeshCaveSetWarpParameters(bool warpEnabled, bool smoothEnabled, unsigned int warpRandomSeed, double warpStrength);

		/// Sets the normal weighting method for the mesh cave.
		/// \param value The normal weighting method.
		void MeshCaveSetNormalWeightMethod(ENormalWeightMethod::Enum value);

		// Corridor parameters:

		/// Sets the distances for the corridor.
		/// \param distance The sampling factor for computing the underlying hermite spline. Lower values will result in more triangles, but a smoother mesh.
		/// \param textureDistance The Y texture coordinate increase per sampling point
		void CorridorSetDistances(double distance, double textureDistance);

		/// Adds a point for the corridor profile. The profile has to have at least 3 points and enclose (0,0), which is the center.
		/// \param x X coordinate.
		/// \param y Y coordinate.
		/// \param textureX X texture coordinate.
		void CorrdidorAddPoint(double x, double y, double textureX);
		
		/// Removes all points from the corridor profile.
		void CorrdidorRemovePoints();

		/// Adds a detail object template, which shall be placed along the corridor.
		/// \param parameters The parameters of the detail object.
		void CorrdidorAddDetailObject(const SDetailobjectParameters& parameters);
		
		/// Removes all detail object templates.
		void CorrdidorRemoveDetailobjects();

		/// Loads a room pattern, which has to be a .irr file. See mainpage of documentation of requirements for room patterns.
		/// \param filename The filename of the room pattern.
		void RoomPatternLoad(const irr::io::path filename);

		// Material parameters:

		/// Sets material parameters for the corridor.
		/// \param textureFilename The filename of the texture that shall be used.
		/// \param backFaceCulling Shall backface culling be enabled?
		/// \param antiAliasing Shall anti aliasing culling be enabled?
		void MaterialSetCorridor(const irr::io::path& textureFilename, bool backFaceCulling, bool antiAliasing);

		/// Chooses the single color material for the mesh cave and sets the parameters for it.
		/// \param color The color of the cave.
		/// \param backFaceCulling Shall backface culling be enabled?
		/// \param antiAliasing Shall anti aliasing culling be enabled?
		void MaterialSetCaveSingleColor(const irr::video::SColorf& color, bool backFaceCulling, bool antiAliasing);

		/// Chooses the multi color material for the mesh cave and sets the parameters for it.
		/// \param backFaceCulling Shall backface culling be enabled?
		/// \param antiAliasing Shall anti aliasing culling be enabled?
		void MaterialSetCaveMultiColor(bool backFaceCulling, bool antiAliasing);

		/// Sets a custom material for cave or corridor.
		/// \param type The type of the custom material. Can be SMaterialType::CAVE_CUSTOM or SMaterialType::CORRIDOR_CUSTOM.
		/// \param material The material being used.
		/// \param backFaceCulling Shall backface culling be enabled?
		/// \param antiAliasing Shall anti aliasing culling be enabled?
		void MaterialSetCustom(SMaterialType::Enum type, CMaterialDunGen* material, bool backFaceCulling, bool antiAliasing);

		/// Sets if status reports shall be printed to the console.
		/// \param value If true, status reports will be printed to the console.
		void SetPrintToConsole(bool value);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// other
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		/// Provides direct access to voxel space.
		///
		/// Use with: unsigned char (&voxelspace)[DunGen::SVoxelSpace::DimX][DunGen::SVoxelSpace::DimY][DunGen::SVoxelSpace::DimZ] = dunGen->GetVoxelSpace(); .
		/// Then you can set voxels with: voxelspace[x][y][z] = value; .
		/// Only set voxels to 0 (stone) or 1 (free space), preserve a three voxel border of 0-voxels.
		/// All 1-voxel have to be 6-connected.
		/// Be careful with this (could crash if you do not preserve the border).
		/// \returns A reference to the three-dimensional array of the voxel space.
		unsigned char (&GetVoxelSpace())[SVoxelSpace::DimX][SVoxelSpace::DimY][SVoxelSpace::DimZ];

	private:
		/// Private copy constructor, because it shall not be used.
		CDunGen(const CDunGen& other);
		/// Private assignment operator, because it shall not be used.
		CDunGen& operator=(const CDunGen& other);

		/// Implementation of the dungeon generator.
		CDungeonGenerator* DungeonGenerator;

		/// The material provider.
		CMaterialProvider* MaterialProvider;

		/// The DunGen XML reader.
		CDunGenXMLReader* DunGenXMLReader;

		/// The irrlicht device.
		irr::IrrlichtDevice* IrrDevice;
	};

} // END NAMESPACE DunGen

#undef DllExport

#endif