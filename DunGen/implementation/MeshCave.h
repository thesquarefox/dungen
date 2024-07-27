// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef MESHCAVE_H
#define MESHCAVE_H

#include "interface/MeshCaveCommon.h"
#include "interface/VoxelCaveCommon.h"
#include <irrlicht.h>
#include <list>
#include <vector>

/// Namespace DunGen : DungeonGenerator
namespace DunGen
{
	class CRandomGenerator;
	class CVoxelCave;

	/// mesh cave class
	///
	/// class for converting voxels into meshes
	class CMeshCave
	{
	private:
		// octree node for conversion voxel->mesh
		struct SOctreeNode
		{
			/// child nodes
			SOctreeNode* ChildNode[8];
			/// parent node
			SOctreeNode* ParentNode;
			/// borders
			unsigned int BorderMinX, BorderMaxX, BorderMinY, BorderMaxY, BorderMinZ, BorderMaxZ;
			/// number of needed vertices to convert this node
			unsigned int VertexNumber;

			/// recursively sum up VertexNumber from the leafs up to this node
			unsigned int CountVertices();
		};
		/// which warp directions are allowed when warping?
		struct SVertexWarpDirections
		{
			/// allowed directions: -1...negative, +1...positive, 0...negative & positive
			int DirectionX, DirectionY, DirectionZ;
		};

		/// helper-struct for normal computing: where does a vertex appears?
		struct SVertexAddress
		{
			unsigned int MeshbufferID;	///< ID of the meshbuffer
			unsigned int VertexID;		///< index of the vertex
		};

		/// helper-struct for normal computing: further information about a vertex
		struct SVertexInformations
		{
			unsigned int Y;							///< Y-coordinate (X,Z are stored by the hash table adress)
			irr::core::vector3d<double> Normal;		///< normal
			SVertexAddress Address;					///< adress of the vertex

			/// compare operator for sorting hash table entries (sorted by Y-coordinate)
			bool operator<(const SVertexInformations& other_);
		};

	public:
		/// constructor
		__declspec(noinline) CMeshCave(CVoxelCave* voxelCave_, const CRandomGenerator* randomGenerator_);
		/// destructor
		~CMeshCave();

		/// read the generated Mesh
		irr::scene::SMesh* GetMesh();

		/// conversion method: voxel drawing will be converted to a triangular mesh
		void CreateMeshFromVoxels();

		/// computes vertex coordinates and if the vertex is a docking vertex
		irr::core::vector3d<irr::f32> ComputeVertexCoordinates(unsigned int x_, unsigned int y_, unsigned int z_, irr::f32& markingDockingVertex_);

		/// set parameter warp random seed
		void SetWarpRandomSeed(unsigned int seed_);
		/// set parameter warp strength, returns resulting warp strength (clamped to [0,0.49])
		void SetWarpStrength(double value_);

		/// specifies if warping should be used
		void SetWarpOption(bool enabled_);
		/// specifies if warping should be smoothing the mesh
		void SetSmoothOption(bool enabled_);
		
		/// sets the method for normal weighting
		void SetNormalWeightMethod(ENormalWeightMethod::Enum normalWeightMethod_);

		/// sets if status reports should be printed to the console
		void SetPrintToConsole(bool enabled_);

	private:
		/// compute the octree of the voxelspace
		void ComputeOctree();

		/// compute the geometry of the mesh
		void ComputeGeometry();

		/// compute the normals of the mesh
		void ComputeNormals();

		/// checks if a vertex at specified sweep plane position is present, if not the vertex is created
		inline void CreateVertex(irr::scene::SMeshBuffer* meshBuffer_, unsigned int& bufferVertices_, SOctreeNode* octreeNode_,
			unsigned int sweepPlaneLayer_, unsigned int x_, unsigned int y_, unsigned int z_);

		/// tests if a vertex is a border vertex (which is shared by other mesh buffers)
		irr::f32 IsBorderVertex(unsigned int x_, unsigned int y_, unsigned int z_, SOctreeNode* octreeNode_);

	private:
		/// the stored mesh
		irr::scene::SMesh* Mesh;

		/// random generator, used for warping
		const CRandomGenerator* RandomGenerator;
		/// the voxel cave
		CVoxelCave* VoxelCave;

		/// should vertices be warped?
		bool WarpEnabled;
		/// how much should they be warped?
		double WarpStrength;
		/// should the warping smooth the mesh?
		bool SmoothEnabled;
		/// random seed for warping
		unsigned int RandomSeed;
		
		/// how are the vertex normals weighted when summing up the triangle normals?
		ENormalWeightMethod::Enum NormalWeightMethod;
		
		/// maximal allowed manhatten distance per plane for clamping
		static const double MaxClampDistance;
		/// maximal number of vertices per meshbuffer
		static const unsigned int MaxVertexCount = 65500;

		/// global sweep plane, for counting and referring vertices
		unsigned int GlobalSweepPlane[2][SVoxelSpace::DimY+1][SVoxelSpace::DimZ+1];

		/// hash table for normal computing: hash value = (X,Z) rounded in integer values
		std::list<SVertexInformations>* HashTable[SVoxelSpace::DimX+1][SVoxelSpace::DimZ+1];
		
		/// look up table for allowed warp directions
		SVertexWarpDirections VertexWarpDirections[256];

		/// octree for converting
		std::vector<SOctreeNode*> Octree;

		/// print status reports to console if true
		bool PrintToConsole;
	};

} // END NAMESPACE DunGen

#endif