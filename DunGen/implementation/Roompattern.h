// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef ROOMPATTERN_H
#define ROOMPATTERN_H

#include "DockingSite.h"
#include <irrlicht.h>
#include <vector>

struct SMatrix4D;

// Namespace DunGen : DungeonGenerator
namespace DunGen
{
	/// a pattern for dungeon rooms
	class CRoomPattern
	{
	private:

		/// helper struct for adjacency lists
		struct SAdjacentVertex
		{
			bool Multiple;			///< is the neighborhood muliple (is case for non border edges)
			unsigned int Index;		///< the vertex index
		};

		/// adjacency list for one vertex
		struct SAdjacencyList
		{
			std::vector<SAdjacentVertex> AdjacentVertices;	///< the adjacent vertices
			void AddAdjacentVertex(unsigned int index_);	///< add an adjacent vertex
		};

	public:
		/// constructor: load .irr scene as pattern for a room
		CRoomPattern(irr::IrrlichtDevice* irrDevice_, const irr::io::path& filename_);
		/// destructor
		~CRoomPattern();

		/// copy pattern into another scenegraph
		void AddRoomInstance(irr::scene::ISceneNode* parentNode_, irr::scene::ISceneManager* sceneManager_);

		/// read docking site
		SDockingSite GetDockingSite(unsigned int index_);

		/// read number of docking sites
		unsigned int GetDockingSiteNumber();

	private:
		/// compute a docking site
		bool ComputeDockingSite(unsigned int index_);

		/// own scenemanager for reading and managing the pattern
		irr::scene::ISceneManager* PrivateSceneManager;

		/// the docking site of the room pattern
		std::vector<SDockingSite> DockingSite;
	};

} // END NAMESPACE DunGen

#endif