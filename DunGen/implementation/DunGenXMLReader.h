// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef DUNGENXMLREADER_H
#define DUNGENXMLREADER_H

#include <vector>
#include <irrlicht.h>

// Namespace DunGen : DungeonGenerator
namespace DunGen
{
	class CDunGen;

	/// Class for reading and processing DunGen XML files.
	class CDunGenXMLReader
	{
	public:
		/// Constructor.
		CDunGenXMLReader(irr::IrrlichtDevice* irrDevice_, CDunGen* dunGen_);

		/// Destructor.
		~CDunGenXMLReader();

		/// process a DunGen XML file
		bool Read(const irr::io::path& filename_);

	private:
		/// process 'Materials' block
		void ReadMaterials();

		/// process 'RandomGenerator' block
		void ReadRandomGenerator();

		/// process 'WarpOptions' block
		void ReadWarpOptions();

		/// process 'DrawVoxelCave' block
		void ReadDrawVoxelCave();

		/// process 'Erode' block
		void ReadErode();

		/// process 'Filter' block
		void ReadFilter();

		/// process 'GenerateMeshCave' block
		void ReadGenerateMeshCave();

		/// process 'PlaceRoom' block
		void ReadPlaceRoom();

		/// process 'CorridorSettings' block
		void ReadCorridorSettings();

		/// process 'CorrdidorDetailObjects' block
		void ReadCorrdidorDetailObjects();

		/// process 'CorridorRoomRoom' block
		void ReadCorridorRoomRoom();
		
		/// process 'CorridorRoomCave' block
		void ReadCorridorRoomCave();

		/// process 'CorridorCaveCave' block
		void ReadCorridorCaveCave();

	private:
		/// reference to the DunGen interface
		CDunGen* DunGenInterface;

		/// reference to the the file system
		irr::io::IFileSystem* FileSystem;

		/// the actual file
		irr::io::IReadFile* File;

		/// the XML reader for the actual file
		irr::io::IXMLReader* XmlReader;

		/// actual stored room patterns, specified by their paths
		std::vector<irr::io::path> RoomPathes;

		/// own scene manager for managing detail objects
		irr::scene::ISceneManager* PrivateSceneManager;
	};
}

#endif
