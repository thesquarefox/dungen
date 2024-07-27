// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef CORRIDORCOMMON_H
#define CORRIDORCOMMON_H

#include <irrlicht.h>

namespace DunGen
{
	/// The parameters of a detailobject in a corridor.
	struct SDetailobjectParameters
	{	
		irr::scene::ISceneNode* Node;				///< The scene node (with its children), which is used as template for the detail object.
		irr::core::vector3d<double> Scale;			///< The scale factor of the detail object.
		irr::core::vector3d<double> Rotation;		///< The rotation of the detail object.
		irr::core::vector2d<double> Position;		///< The 2D position of the detail object, uses the same coordiante system as the corridor points: (0,0) is center.
		double DistanceSampling;					///< The sampling factor for computing the underlying hermite spline. Advice: use the same as the distance you set with DunGen::CorridorSetDistances.
		unsigned int DistanceNumFactor;				///< The distancefactor. Each detailobject is placed every DistanceNumFactor*[DistanceNumMin,DistanceNumMax] samplepoints.
		unsigned int DistanceNumMin;				///< The minimum random distance. Each detailobject is placed every DistanceNumFactor*[DistanceNumMin,DistanceNumMax] samplepoints.
		unsigned int DistanceNumMax;				///< The maximum random distance. Each detailobject is placed every DistanceNumFactor*[DistanceNumMin,DistanceNumMax] samplepoints.
		unsigned int DistanceNumMinFirstElement;	///< The minimum random distance for the first detailobject. The first detailobject is placed every DistanceNumFactor*[DistanceNumMinFirstElement,DistanceNumMaxFirstElement] samplepoints.
		unsigned int DistanceNumMaxFirstElement;	///< The maximum random distance for the first detailobject. The first detailobject is placed every DistanceNumFactor*[DistanceNumMinFirstElement,DistanceNumMaxFirstElement] samplepoints.
		bool ObjectAtT1;							///< Shall a detailobjects be placed at the last samplepoint (end of corridor), even if the distance would be too short?
	};
}

#endif