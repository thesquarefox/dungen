// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef DOCKINGSITE_H
#define DOCKINGSITE_H

#include "Helperfunctions.h"
#include <irrlicht.h>
#include <vector>

// Namespace DunGen : DungeonGenerator
namespace DunGen
{
	/// a docking site, used to connect caves with corridors and rooms with corridors
	///
	/// a chain of 3d points as a closed curve
	/// clockwise direction
	/// important: (docking site plane) polar angle of the points has to be monotonic increasing circulary
	/// the XY 0 point has to be enclosed by the (docking site plane) projected curve (-> at least 3 points necessary)
	struct SDockingSite
	{
		std::vector<irr::core::vector3d<double>> Point;	///< points of the docking site
		irr::core::vector3d<double> Normal;				///< normal of the docking site (normal of the approximated plane of all points)
		irr::core::vector3d<double> Center;				///< center of the docking site
		double Extend;									///< extend of the docking site in direction of the normal

		/// transform docking site with a homogeneous transforamtion
		void TransformDockingSite(const SMatrix4D& transformationMatrix_)
		{
			// transform points
			for (unsigned int i=0; i<Point.size(); ++i)
				transformationMatrix_.MultiplyWith1x3_1(Point[i]);

			// transform center
			transformationMatrix_.MultiplyWith1x3_1(Center);

			// transform extend (extend can be 0, thats why it is not computable with the normal together)
			irr::core::vector3d<double> extendWeightedNormal = Extend * Normal;
			transformationMatrix_.MultiplyWith1x3_0(extendWeightedNormal);
			Extend = extendWeightedNormal.getLength();

			// transform normal
			transformationMatrix_.MultiplyWith1x3_0(Normal);
			Normal.normalize();	// because transformation can contain scaling
		}
	};
}
#endif