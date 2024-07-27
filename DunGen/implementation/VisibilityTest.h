// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef VISIBILITYTEST_H
#define VISIBILITYTEST_H

#include <irrlicht.h>
#include <vector>

// Namespace DunGen : DungeonGenerator
namespace DunGen
{
	struct SCorridorProfile;
	struct SDockingSite;
	
	/// computing visibility information for CCorridor
	namespace NVisibilityTest
	{
		// view region for visibility test
		struct SViewRegionOfInterest
		{
			irr::core::vector3d<double> Position;	///< position
			irr::core::vector3d<double> Left;		///< left vector
			irr::core::vector3d<double> Up;			///< up vector
			irr::core::rect<double> ProfileBB;		///< rectangular profile of the region
		};

		/// viewing volume for visibility test
		struct SViewingVolume
		{
			/// constructor, takes 2 regions of interest to form a viewing volume
			SViewingVolume(const SViewRegionOfInterest* region1_, const SViewRegionOfInterest* region2_);

			irr::core::vector3d<double> PointOfReference;	///< center of the projection plane
			irr::core::vector3d<double> Left;				///< left vector spanning the projection plane
			irr::core::vector3d<double> Up;					///< up vector spanning the projection plane
			irr::core::rect<double> ViewingVolumeBB;		///< projected bounding box of the viewing volume
		};

		/// tests, if a region of interest is outside a viewing volume (if yes -> sighblocking)
		bool IsSightBlocking(const SViewingVolume* viewingVolume_, const SViewRegionOfInterest* regionOfInterest_);

		/// tests, if the corridor blocks sight
		bool VisibilityTest(std::vector<SViewRegionOfInterest>& viewTestRegions_, const SCorridorProfile& profile_,
			const SDockingSite& dockingSite0_, const SDockingSite& dockingSite1_);
	}

} // END NAMESPACE DunGen

#endif