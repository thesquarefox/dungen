// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef VOXELCAVE_H
#define VOXELCAVE_H

#include "interface/VoxelCaveCommon.h"
#include <irrlicht.h>
#include <string>
#include <vector>

// Namespace DunGen : DungeonGenerator
namespace DunGen
{
	class CRandomGenerator;

	/// voxel cave class
	///
	/// voxel space with functions to draw turtle graphics, erode and filter
	class CVoxelCave
	{
	private:
		/// state of the turtle (for turtle graphic)
		struct STurtleState
		{
			/// coordinates
			irr::core::vector3d<double> Position;

			/// local coordinate system: front vector
			irr::core::vector3d<double> Front;
			/// local coordinate system: left vector
			irr::core::vector3d<double> Left;
			/// local coordinate system: up vector
			irr::core::vector3d<double> Up;

			/// drawing radius in voxels
			double Radius;	
		};

		/// voxel coordinates
		struct SVoxelToDraw
		{
			/// coordinates
			int X, Y, Z;
		};

		/// the value of a helper voxel
		static const unsigned char HelperVoxel = 2;

	public:
		/// the value of a docking voxel
		static const unsigned char DockingVoxel = 3;

		/// minimum border for filtering loops
		static const unsigned int MinBorderFilter = SVoxelSpace::MinBorder-1;

	public:
		/// constructor
		__declspec(noinline) CVoxelCave(const CRandomGenerator* randomGenerator_);
		// destructor
		~CVoxelCave();

		/// draw turtle graphic in voxel space
		void Draw(std::string instructions_, double angleYaw_, double anglePitch_, double angleRoll_, double radiusStart_, double radiusFactor_, double radiusDecrement_);

		/// erode 1 voxel deep
		///
		/// ErosionLikelihood is the likelihood for the erosion of a voxel:
		/// 1 ... every voxel is eroded, 0 ... no voxel is eroded
		void Erode(double erosionLikelihood_);

		/// remove hovering fragments
		unsigned int Filter();

		/// set the minimum voxel space border (this is always 0 = stone, cannot be smaller than 3)
		void SetBorder(unsigned int border_);

		/// set the minimal draw radius for turle graphic
		void SetMinDrawRadius(unsigned int minDrawRadius_);
		
		/// estimate number of triangles, that are needed to model the voxel cave as mesh
		unsigned int EstimateMeshComplexity() const;

		/// sets if status reports should be printed to the console
		void SetPrintToConsole(bool enabled_);

		/// sets a voxel to a specific value
		inline void SetVoxel(unsigned int x_, unsigned int y_, unsigned int z_, unsigned char value_);

		/// gets the value of a voxel
		inline unsigned char GetVoxel(unsigned int x_, unsigned int y_, unsigned int z_) const;

		/// Provides direct access to voxel space.
		unsigned char (&GetVoxelSpace())[SVoxelSpace::DimX][SVoxelSpace::DimY][SVoxelSpace::DimZ];

	private:
		/// geometric primitive: cylinder
		void DrawCylinder(int xs_,int ys_,int zs_,int xe_,int ye_,int ze_, int radius_, irr::core::vector3d<double> left_, irr::core::vector3d<double> up_);
		/// geometric primitive: sphere
		void DrawSphere(int x_,int y_,int z_, int radius_);

		/// Bresenham: longest axis = X
		inline void Bresenham3dX(int xs_,int ys_,int zs_,int xe_,int ye_,int ze_, std::vector<SVoxelToDraw>& vector_);
		/// Bresenham: longest axis = Y
		inline void Bresenham3dY(int xs_,int ys_,int zs_,int xe_,int ye_,int ze_, std::vector<SVoxelToDraw>& vector_);
		/// Bresenham: longest axis = Z
		inline void Bresenham3dZ(int xs_,int ys_,int zs_,int xe_,int ye_,int ze_, std::vector<SVoxelToDraw>& vector_);
		/// central call of the Bresenham algorithm
		void Bresenham3d(int xs_,int ys_,int z_s,int xe_,int ye_,int ze_, std::vector<SVoxelToDraw>& vector_);

		/// draws a 18-connected voxelblock around a central voxel
		inline void DrawVoxelBlock(unsigned int x_,unsigned int y_,unsigned int z_);
		/// creates a draws a voxel in 
		inline void DrawVoxelToVector(int x_, int y_, int z_, std::vector<SVoxelToDraw>& vector_);

		/// ensures 3 voxel border
		void EnsureIntegrity();

		/// test if voxel is a cave border voxel (used by erosion)
		inline bool IsBoundaryVoxel(unsigned int x_,unsigned int y_,unsigned int z_) const;
		/// test if voxel is a cave border voxel (used by filtering)
		inline bool IsBoundaryVoxelExtended(unsigned int x_,unsigned int y_,unsigned int z_) const;
		/// fast test if voxel is a cave border voxel (used by filtering), test in positive direction
		inline bool IsBoundaryVoxelBeginUpward(unsigned int x_,unsigned int y_,unsigned int z_) const;
		/// fast test if voxel is a cave border voxel (used by filtering), test in negative direction
		inline bool IsBoundaryVoxelBeginDownward(unsigned int x_,unsigned int y_,unsigned int z_) const;

	private:
		/// voxel space
		unsigned char Voxel[SVoxelSpace::DimX][SVoxelSpace::DimY][SVoxelSpace::DimZ];

		/// random generator
		const CRandomGenerator* RandomGenerator;

		/// minimum distance of the graphic to the border of the voxel space
		unsigned int Border;
		/// minimum draw radius
		unsigned int MinDrawRadius;

		/// print status reports to console if true
		bool PrintToConsole;
	};

	void DunGen::CVoxelCave::SetVoxel(unsigned int x_, unsigned int y_, unsigned int z_, unsigned char value_)
	{
		Voxel[x_][y_][z_] = value_;
	}

	unsigned char DunGen::CVoxelCave::GetVoxel(unsigned int x_, unsigned int y_, unsigned int z_) const
	{
		return Voxel[x_][y_][z_];
	}

	
} // END NAMESPACE DunGen

#endif