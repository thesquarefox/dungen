// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#include "VoxelCave.h"
#include "Helperfunctions.h"
#include "RandomGenerator.h"
#include <iostream>
#include <math.h>
#include <queue>
#include <stack>

// ======================================================
// constructor / destructor
// ======================================================

// no inlining (else the VS compiler tends to put the whole pre-initialized voxelspace in the executable in some cases)
__declspec(noinline) DunGen::CVoxelCave::CVoxelCave(const CRandomGenerator* randomGenerator_)
	: RandomGenerator(randomGenerator_)
	, Border(SVoxelSpace::MinBorder)
	, MinDrawRadius(2)
	, PrintToConsole(false)
{
	// clear voxelspace
	memset(Voxel,0,sizeof(unsigned char)*SVoxelSpace::DimX*SVoxelSpace::DimY*SVoxelSpace::DimZ);
}

DunGen::CVoxelCave::~CVoxelCave()
{
}

// ======================================================
// setting parameters
// ======================================================

void DunGen::CVoxelCave::SetBorder(unsigned int border_)
{
	Border = (border_ >= SVoxelSpace::MinBorder) ? border_ : SVoxelSpace::MinBorder;
}

void DunGen::CVoxelCave::SetMinDrawRadius(unsigned int minDrawRadius_)
{
	MinDrawRadius = minDrawRadius_;
}

void DunGen::CVoxelCave::SetPrintToConsole(bool enabled_)
{
	PrintToConsole = enabled_;
}

// ======================================================
// setting voxels
// ======================================================

inline void DunGen::CVoxelCave::DrawVoxelBlock(unsigned int x_,unsigned int y_,unsigned int z_)
{
	// draw block of voxels around a central voxel

	// draw central voxel
	SetVoxel(x_,y_,z_,1);
	
	// draw 6-connected voxels
	SetVoxel(x_-1,y_,z_,1); SetVoxel(x_,y_-1,z_,1); SetVoxel(x_,y_,z_-1,1);	
	SetVoxel(x_+1,y_,z_,1); SetVoxel(x_,y_+1,z_,1); SetVoxel(x_,y_,z_+1,1);

	// draw 18-connected voxels
	SetVoxel(x_-1,y_-1,z_,1); SetVoxel(x_-1,y_+1,z_,1); SetVoxel(x_+1,y_-1,z_,1); SetVoxel(x_+1,y_+1,z_,1);
	SetVoxel(x_-1,y_,z_-1,1); SetVoxel(x_-1,y_,z_+1,1); SetVoxel(x_+1,y_,z_-1,1); SetVoxel(x_+1,y_,z_+1,1);
	SetVoxel(x_,y_-1,z_-1,1); SetVoxel(x_,y_-1,z_+1,1); SetVoxel(x_,y_+1,z_-1,1); SetVoxel(x_,y_+1,z_+1,1);	
}

inline void DunGen::CVoxelCave::DrawVoxelToVector(int x_, int y_, int z_, std::vector<SVoxelToDraw>& vector_)
{
	// create voxel
	SVoxelToDraw newVoxel;
	newVoxel.X = x_;
	newVoxel.Y = y_;
	newVoxel.Z = z_;
	// store voxel in the vector
	vector_.push_back(newVoxel);
}

// ======================================================
// Bresenham in 3D
// ======================================================
// 3D extension of Bresenham’s algorithm
// source:
//    Three-dimensional extension of Bresenham's algorithm and its application in straight-line interpolation,
//    Proceedings of the Institution of Mechanical Engineers, Part B: Journal of Engineering Manufacture March 1, 2002 216: 459-463
//    pib.sagepub.com/content/216/3/459.full.pdf
// modificated for all possible cases

inline void DunGen::CVoxelCave::Bresenham3dX(int xs_,int ys_,int zs_,int xe_,int ye_,int ze_, std::vector<SVoxelToDraw>& vector_)
{	// input: endpoints
	
	// compute directions
	int xinc = (xe_<xs_) ? -1 : 1;
	int yinc = (ye_<ys_) ? -1 : 1;
	int zinc = (ze_<zs_) ? -1 : 1;

	// compute dx,dy,dz
	int dx=abs (xe_-xs_);
	int dy=abs (ye_-ys_);
	int dz=abs (ze_-zs_);

	// compute initial decision parameters
	int ey=2*dy-dx; int ez=2*dz-dx;
	// compute constants for Y
	int twoDy=2*dy; int twoDyDx=2*(dy-dx);
	// compute constants for Z
	int twoDz=2*dz; int twoDzDx=2*(dz-dx);
	// inialize position
	int x=xs_; int y=ys_; int z=zs_;
	
	// loop to draw the line
	for (int i=0; i<=dx; ++i)
	{
		// draw voxel
		DrawVoxelToVector(x,y,z,vector_);

		// start of computing the next position
		x+=xinc; 
		if (ey>=0) {
			y+=yinc; ey+=twoDyDx;
		}
		else
			ey+=twoDy;
		
		if (ez>=0) {
			z+=zinc; ez+=twoDzDx;
		}
		else
			ez+=twoDz;
		// end of computing the next position
	}
}

inline void DunGen::CVoxelCave::Bresenham3dY(int xs_,int ys_,int zs_,int xe_,int ye_,int ze_, std::vector<SVoxelToDraw>& vector_)
{	// input: endpoints

	// compute directions
	int xinc = (xe_<xs_) ? -1 : 1;
	int yinc = (ye_<ys_) ? -1 : 1;
	int zinc = (ze_<zs_) ? -1 : 1;

	// compute dx,dy,dz
	int dx=abs (xe_-xs_);
	int dy=abs (ye_-ys_);
	int dz=abs (ze_-zs_);

	// compute initial decision parameters
	int ex=2*dx-dy; int ez=2*dz-dy;
	// compute constants for X
	int twoDx=2*dx; int twoDxDy=2*(dx-dy);
	// compute constants for Z
	int twoDz=2*dz; int twoDzDy=2*(dz-dy);
	// inialize position
	int x=xs_; int y=ys_; int z=zs_;
	
	// loop to draw the line
	for (int i=0; i<=dy; ++i)
	{
		// draw voxel
		DrawVoxelToVector(x,y,z,vector_);

		// start of computing the next position
		y+=yinc; 
		if (ex>=0) {
			x+=xinc; ex+=twoDxDy;
		}
		else
			ex+=twoDx;
		
		if (ez>=0) {
			z+=zinc; ez+=twoDzDy;
		}
		else
			ez+=twoDz;
		// end of computing the next position
	}
}

inline void DunGen::CVoxelCave::Bresenham3dZ(int xs_,int ys_,int zs_,int xe_,int ye_,int ze_, std::vector<SVoxelToDraw>& vector_)
{	// input: endpoints

	// compute directions
	int xinc = (xe_<xs_) ? -1 : 1;
	int yinc = (ye_<ys_) ? -1 : 1;
	int zinc = (ze_<zs_) ? -1 : 1;

	// compute dx,dy,dz
	int dx=abs (xe_-xs_);
	int dy=abs (ye_-ys_);
	int dz=abs (ze_-zs_);

	// compute initial decision parameters
	int ey=2*dy-dz; int ex=2*dx-dz;
	// compute constants for Y
	int twoDy=2*dy; int twoDyDz=2*(dy-dz);
	// compute constants for Z
	int twoDx=2*dx; int twoDxDz=2*(dx-dz);
	// inialize position
	int x=xs_; int y=ys_; int z=zs_;
	
	// loop to draw the line
	for (int i=0; i<=dz; ++i)
	{
		// draw voxel
		DrawVoxelToVector(x,y,z,vector_);

		/// start of computing the next position
		z+=zinc; 
		if (ey>=0) {
			y+=yinc; ey+=twoDyDz;
		}
		else
			ey+=twoDy;
		
		if (ex>=0) {
			x+=xinc; ex+=twoDxDz;
		}
		else
			ex+=twoDx;
		// end of computing the next position
	}
}

void DunGen::CVoxelCave::Bresenham3d(int xs_,int ys_,int z_s,int xe_,int ye_,int ze_, std::vector<SVoxelToDraw>& vector_)
{
	// compute axis lengths
	int dx=abs (xe_-xs_);
	int dy=abs (ye_-ys_);
	int dz=abs (ze_-z_s);

	if ((dx>dy)&&(dx>dz)) Bresenham3dX(xs_,ys_,z_s,xe_,ye_,ze_,vector_);		// longest axis: X
	else if (dy>dz) Bresenham3dY(xs_,ys_,z_s,xe_,ye_,ze_,vector_);				// longest axis: Y
	else Bresenham3dZ(xs_,ys_,z_s,xe_,ye_,ze_,vector_);							// longest axis: Z
}

// ======================================================
// further drawing functions for geometric primitives
// ======================================================

void DunGen::CVoxelCave::DrawCylinder(int xs_,int ys_,int zs_,int xe_,int ye_,int ze_, int radius_, irr::core::vector3d<double> left_, irr::core::vector3d<double> up_)
{
	// this algorithm uses three 3D Bresenhams to span a cylinder
	// voxelblocks are used to draw, to close potential gaps that are caused by the 26-connectivity of the 3D Bresenham algorithm
	// _Left und _Up are the vectors to span the top surface - to get the correct radius, they have to be normalized

	// decrement radius by one, because voxelblocks are drawn instead single voxels	
	--radius_;
	
	// buffers for the spaning axis
	std::vector<SVoxelToDraw> voxelVector1;
	std::vector<SVoxelToDraw> voxelVector2; 
	std::vector<SVoxelToDraw> voxelVector3; 

	// parameters for the first bresenham:
	int xn = d2i(radius_ * up_.X);
	int yn = d2i(radius_ * up_.Y);
	int zn = d2i(radius_ * up_.Z);

	// start first Bresenham: cylinder top surface vertical
	Bresenham3d(0,0,0,xn,yn,zn,voxelVector1);

	// parameters for the second bresenham:
	xn = d2i(radius_ * left_.X);
	yn = d2i(radius_ * left_.Y);
	zn = d2i(radius_ * left_.Z);
	
	// start second Bresenham: cylinder top surface horizontal
	Bresenham3d(0,0,0,xn,yn,zn,voxelVector2);
	// start third Bresenham: cylinder central axis
	Bresenham3d(xs_,ys_,zs_,xe_,ye_,ze_,voxelVector3);

	// preallocate memory
	SVoxelToDraw actualVoxel1;
	SVoxelToDraw actualVoxel2;
	SVoxelToDraw actualVoxel3;
	int xm, ym, zm;

	// compute the squared radius for the inner loop
	const int radiusSquared = radius_ * radius_;
	
	// iterate over the voxels of the 3 axis
	for (unsigned int i=0; i<voxelVector1.size(); ++i)
	{
		actualVoxel1 = voxelVector1[i];
		for (unsigned int j=0; j<voxelVector2.size(); ++j)
		{
			actualVoxel2 = voxelVector2[j];

			// position relative to the center point of the top surface
			xn = actualVoxel1.X + actualVoxel2.X;
			yn = actualVoxel1.Y + actualVoxel2.Y;
			zn = actualVoxel1.Z + actualVoxel2.Z;

			// voxel on top surface of the cylinder?
			if ((xn*xn+yn*yn+zn*zn) <= radiusSquared)
			{
				// parameters for mirroring
				xm = actualVoxel1.X - actualVoxel2.X;
				ym = actualVoxel1.Y - actualVoxel2.Y;
				zm = actualVoxel1.Z - actualVoxel2.Z;

				for (unsigned int k=0; k<voxelVector3.size(); ++k)
				{
					actualVoxel3 = voxelVector3[k];

					// draw first voxelblock: one quadrant
					DrawVoxelBlock(actualVoxel3.X+xn,actualVoxel3.Y+yn,actualVoxel3.Z+zn);
					// mirroring 1
					DrawVoxelBlock(actualVoxel3.X-xn,actualVoxel3.Y-yn,actualVoxel3.Z-zn);
					// mirroring 2
					DrawVoxelBlock(actualVoxel3.X+xm,actualVoxel3.Y+ym,actualVoxel3.Z+zm);
					// mirroring 3
					DrawVoxelBlock(actualVoxel3.X-xm,actualVoxel3.Y-ym,actualVoxel3.Z-zm);
				}
			}
		}
	}
}

void DunGen::CVoxelCave::DrawSphere(int x_,int y_,int z_, int radius_)
{
	// precompute and preallocate
	const int radiusSquared = radius_*radius_;
	int xSquared;
	int xSquaredPlusYSquared;
	
	// walk over the bounding cube of the sphere
	for (int i = -radius_; i<=radius_; ++i)
	{
		// compute x^2
		xSquared = i*i;
		for (int j = -radius_; j<=radius_; ++j)
		{
			// compute x^2 + y^2
			xSquaredPlusYSquared = xSquared + j*j;
			for (int k = -radius_; k<=radius_; ++k)
			{
				// test, if x^2 + y^2 + z^2 no larger than radius^2
				// -> belongs to sphere -> draw voxel
				if ((xSquaredPlusYSquared+k*k) <= radiusSquared)
					SetVoxel(x_+i,y_+j,z_+k,1);
			}
		}
	}
}

// ======================================================
// helper fuctions
// ======================================================

unsigned int DunGen::CVoxelCave::EstimateMeshComplexity() const
{
	unsigned int triangleCounter = 0;

	const unsigned int xMax = SVoxelSpace::DimX - SVoxelSpace::MinBorder;
	const unsigned int yMax = SVoxelSpace::DimY - SVoxelSpace::MinBorder;
	const unsigned int zMax = SVoxelSpace::DimZ - SVoxelSpace::MinBorder;

	// iterate over voxelspace
	for (unsigned int i=SVoxelSpace::MinBorder; i<=xMax; ++i)
		for (unsigned int j=SVoxelSpace::MinBorder; j<=yMax; ++j)
			for (unsigned int k=SVoxelSpace::MinBorder; k<=zMax; ++k)
			{
				// count the 0<->1 junctions
				if (1 == GetVoxel(i-1,j,k) + GetVoxel(i,j,k)) {++triangleCounter;}
				if (1 == GetVoxel(i,j-1,k) + GetVoxel(i,j,k)) {++triangleCounter;}
				if (1 == GetVoxel(i,j,k-1) + GetVoxel(i,j,k)) {++triangleCounter;}
			}

	// per junction, 2 triangles are needed
	return triangleCounter*2;
}


// ======================================================
// drawing functions for turtle graphics
// ======================================================

void DunGen::CVoxelCave::Draw(std::string instructions_,
	double angleYaw_, double anglePitch_, double angleRoll_, double radiusStart_, double radiusFactor_, double radiusDecrement_)
{
	// factors for degree measure to radian measure
	const double angleYawRAD = angleYaw_ * M_DegToRad;
	const double angleNickRAD = anglePitch_ * M_DegToRad;
	const double angleRollRAD = angleRoll_ * M_DegToRad;

	// compute sinus and cosinus values for the given angles
	// for negative angles: cos(-x) = cos(x), sin(-x) = -sin(x)
	const double cosAngleYaw = cos(angleYawRAD);	
	const double sinAngleYaw = sin(angleYawRAD);
	const double cosAngleNick = cos(angleNickRAD);
	const double sinAngleNick = sin(angleNickRAD);
	const double cosAngleRoll = cos(angleRollRAD);
	const double sinAngleRoll = sin(angleRollRAD);

	// clear the voxel space
	memset(Voxel,0,sizeof(unsigned char)*SVoxelSpace::DimX*SVoxelSpace::DimY*SVoxelSpace::DimZ);
		
	// state stack
	std::stack<STurtleState> stateStack;
	// actual stat
	STurtleState actState;
	// previous state
	STurtleState oldState;
		
	// for scaling turtle graphic into voxel space
	double xMin = 0.0;	double xMax = 0.0;
	double yMin = 0.0;	double yMax = 0.0;
	double zMin = 0.0;	double zMax = 0.0;
	double xMid, yMid, zMid;
	
	// lenght of a line
	double fValue = 1.0;
	// set starting postion
	actState.Position = irr::core::vector3d<double>(0.0,0.0,0.0);
	// clamp start radius
	if (radiusStart_<MinDrawRadius)
		radiusStart_ = MinDrawRadius;

	// preallocate memory
	int xOld, yOld, zOld, xNew, yNew, zNew, radiusNew;
	
	// counter to display progress
	unsigned int counterAct = 0;
	// 1 point = 100/VoxelspaceX percent
	// if there are fewer drawing instructions than VoxelspaceX, the progress bar will be shorter
	const unsigned int counterInc = instructions_.size()/SVoxelSpace::DimX;

	// mode 0: compute bounding box (for rescaling), mode 1: draw scaled graphic
	unsigned int mode = 0;

	// append end symbol, so that aggregation of long lines has a symbol !='F'/'[' to stop
	instructions_ += ' ';
	// loop through drawing modes
	while (mode < 2)
	{
		// starting values
		actState.Radius = radiusStart_;
		actState.Front = irr::core::vector3d<double>(1.0,0.0,0.0);
		actState.Left = irr::core::vector3d<double>(0.0,0.0,1.0);
		actState.Up = irr::core::vector3d<double>(0.0,1.0,0.0);

		// clear stack
		while (!stateStack.empty()) stateStack.pop();

		// parse drawing instructions
		for (unsigned int i=0; i<instructions_.size(); ++i)
		{
			// progress output
			if (PrintToConsole && (1 == mode) && (i-counterAct > counterInc))
			{
				counterAct+=counterInc;
				std::cout << ".";
			}

			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// begin of evaluation of drawing instruction
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			switch (instructions_[i])
			{
			case 'F': // draw line
				{
					oldState = actState;

					// compute new position
					actState.Position += fValue*actState.Front;

					// optimization: aggregate long lines (several 'F' repeating)
					while ('F' == instructions_[i+1] || '[' == instructions_[i+1])
					{
						if ('F' == instructions_[i+1])
							actState.Position += fValue*actState.Front;
						else
							stateStack.push(actState);
						++i;
					}

					if (0 == mode)
					{
						// actualize bounding box
						if (actState.Position.X < xMin) xMin = actState.Position.X;
						if (actState.Position.X > xMax) xMax = actState.Position.X;
						if (actState.Position.Y < yMin) yMin = actState.Position.Y;
						if (actState.Position.Y > yMax) yMax = actState.Position.Y;
						if (actState.Position.Z < zMin) zMin = actState.Position.Z;
						if (actState.Position.Z > zMax) zMax = actState.Position.Z;	
					}
					else
					{
						// convert coordinates
						xOld = static_cast<int>(oldState.Position.X);
						yOld = static_cast<int>(oldState.Position.Y);
						zOld = static_cast<int>(oldState.Position.Z);
						xNew = static_cast<int>(actState.Position.X);
						yNew = static_cast<int>(actState.Position.Y);
						zNew = static_cast<int>(actState.Position.Z);
						radiusNew = static_cast<int>(actState.Radius);

						// only draw, if changes make it necessary
						if( xOld!=xNew || yOld!=yNew || zOld!=zNew || radiusNew<static_cast<int>(oldState.Radius) )
						{
							// line, if a coordinate has changed more than 1 voxel
							if ((abs(xOld-xNew) > 1)||(abs(yOld-yNew) > 1)||(abs(zOld-zNew) > 1))
								DrawCylinder(xOld,yOld,zOld,xNew,yNew,zNew, radiusNew, actState.Left, actState.Up);	
							// end: sphere to close gap to successor line (can have other radius or direction)
							DrawSphere(xNew,yNew,zNew, radiusNew);
						}
					}
				}
				break;
			
			case '!': // reduce radius
				{
					actState.Radius= radiusFactor_ * actState.Radius - radiusDecrement_;
					if (actState.Radius < MinDrawRadius)
						actState.Radius = MinDrawRadius;
				}
				break;

			// ~-~-~-~-~-~-~-~-
			// stack operations
			// ~-~-~-~-~-~-~-~-
			case '[': // push on stack
				{
					stateStack.push(actState);
				}
				break;

			case ']': // take from stack
				{
					if (!stateStack.empty())
					{
						actState = stateStack.top();
						stateStack.pop();
					}
				}
				break;

			// ~-~-~-~-~
			// rotations
			// ~-~-~-~-~
			// angles are in mathematical positive direction

			case '+': // rotate around Up by AngleYaw (left): R_up(AngleYaw)
				{
					// compute new coordinate system vectors
					// normalize for numerical stability
					actState.Front = cosAngleYaw * actState.Front + sinAngleYaw * actState.Left;
					actState.Front.normalize();
					// use cross product for second vector (also for numerical stability)
					actState.Left = actState.Front.crossProduct(actState.Up);
					actState.Left.normalize();
				}
				break;

			case '-': // rotate around Up by -AngleYaw (right): R_up(-AngleYaw)
				{
					// compute new coordinate system vectors
					actState.Front = cosAngleYaw * actState.Front - sinAngleYaw * actState.Left;
					actState.Front.normalize();
					actState.Left = actState.Front.crossProduct(actState.Up);
					actState.Left.normalize();
				}
				break;

			case 'u': // rotate around Left by AnglePitch (down): R_left(AnglePitch)
				{
					// compute new coordinate system vectors
					actState.Front = cosAngleNick * actState.Front - sinAngleNick * actState.Up;
					actState.Front.normalize();	
					actState.Up = actState.Left.crossProduct(actState.Front);
					actState.Up.normalize();
				}
				break;

			case 'o': // rotate around Left by -AnglePitch (up): R_left(-AnglePitch)
				{
					// compute new coordinate system vectors
					actState.Front = cosAngleNick * actState.Front + sinAngleNick * actState.Up;
					actState.Front.normalize();	
					actState.Up = actState.Left.crossProduct(actState.Front);
					actState.Up.normalize();
				}
				break;

			case 'z': // rotate around Front by AngleRoll (clockwise): R_front(AngleRoll)
				{
					// compute new coordinate system vectors
					actState.Left = cosAngleRoll * actState.Left + sinAngleRoll * actState.Up;
					actState.Left.normalize();
					actState.Up = actState.Left.crossProduct(actState.Front);
					actState.Up.normalize();
				}
				break;

			case 'g': // rotate around Front by AngleRoll (counterclockwise): R_front(AngleRoll)
				{
					// compute new coordinate system vectors
					actState.Left = cosAngleRoll * actState.Left - sinAngleRoll * actState.Up;
					actState.Left.normalize();
					actState.Up = actState.Left.crossProduct(actState.Front);
					actState.Up.normalize();
				}
				break;

			case '|': // rotate around Up by 180 degrees
				{
					actState.Front = -actState.Front;
					actState.Left = -actState.Left;
				}
				break;

			case '$': // orient turtle to the XZ-plane (horizontal)
				// -> Front und Left shall span the XZ-plane
				{
					// Up is vertical to the XZ-plane
					actState.Up = irr::core::vector3d<double>(0,1,0);

					// values to test for most numerical stable vector:
					// the vector that is more perpendicular to Up
					double leftValue = abs(actState.Up.dotProduct(actState.Left));
					double frontValue = abs(actState.Up.dotProduct(actState.Front));

					// compute remaining vectors
					// normalize for numerical stability
					if (leftValue<frontValue)	// Left is more stable
					{
						// compute Front
						actState.Front = actState.Up.crossProduct(actState.Left);
						actState.Front.normalize();
						// then Left
						actState.Left = actState.Front.crossProduct(actState.Up);
						actState.Left.normalize();
					}
					else // Front is more stable
					{
						// compute Left
						actState.Left = actState.Front.crossProduct(actState.Up);
						actState.Left.normalize();
						// then Front
						actState.Front = actState.Up.crossProduct(actState.Left);
						actState.Front.normalize();
					}	
				}
				break;
			
			} // end switch
			
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// end of evaluation of drawing instruction
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		} // end for

		// scale graphic into voxel space
		if (0 == mode)
		{
			// compute center
			xMid = xMin + 0.5*(xMax - xMin);
			yMid = yMin + 0.5*(yMax - yMin);
			zMid = zMin + 0.5*(zMax - zMin);

			/// adjust invalid values
			if (xMax==xMin)	++xMax;
			if (yMax==yMin)	++yMax;
			if (zMax==zMin)	++zMax;

			// border offset = 2*radius + 2*border
			// + 1 because voxel space has a side length 1 smaller than amount of voxels at that side
			// + 1 because of rounding issues integer <-> floating point
			int borderOffset = 2 * (Border + static_cast<int>(radiusStart_)) + 2;
			// compute scaling factors
			double scaleX = static_cast<double>(SVoxelSpace::DimX-borderOffset) / (xMax-xMin);
			double scaleY = static_cast<double>(SVoxelSpace::DimY-borderOffset) / (yMax-yMin);
			double scaleZ = static_cast<double>(SVoxelSpace::DimZ-borderOffset) / (zMax-zMin);

			// set line length to minimum scale factor
			// so the graphic is scaled uniformly along all dimensions
			fValue = std::min(scaleX,std::min(scaleY,scaleZ));
			
			// set new starting point
			// the center point of the voxel graphic shall be identical with the center point of the voxel space
			actState.Position.X = 0.5*static_cast<double>(SVoxelSpace::DimX-1) - fValue*xMid;
			actState.Position.Y = 0.5*static_cast<double>(SVoxelSpace::DimY-1) - fValue*yMid;
			actState.Position.Z = 0.5*static_cast<double>(SVoxelSpace::DimZ-1) - fValue*zMid;
			
			// draw sphere at start
			DrawSphere(static_cast<int>(actState.Position.X)
				, static_cast<int>(actState.Position.Y)
				, static_cast<int>(actState.Position.Z)
				, static_cast<int>(radiusStart_));
			
			if (PrintToConsole) std::cout << "start drawing with F-length: " << fValue
				<< " at: " << actState.Position.X << " , " << actState.Position.Y << " , " << actState.Position.Z << std::endl;
		}

		++mode;

	} // end while

	if (PrintToConsole) std::cout << std::endl;

	EnsureIntegrity();

} // end Draw


void DunGen::CVoxelCave::EnsureIntegrity()
{
	if (PrintToConsole)
		std::cout << "Ensuring integrity of voxelspace ... ";

	// Set all border voxels to 0
	
	// X planes
	for (unsigned int i=0; i<SVoxelSpace::MinBorder; ++i)
		for (unsigned int j=0; j<SVoxelSpace::DimY; ++j)
			for (unsigned int k=0; k<SVoxelSpace::DimZ; ++k)
				SetVoxel(i,j,k,0);

	for (unsigned int i=SVoxelSpace::DimX-SVoxelSpace::MinBorder; i<SVoxelSpace::DimX; ++i)
		for (unsigned int j=0; j<SVoxelSpace::DimY; ++j)
			for (unsigned int k=0; k<SVoxelSpace::DimZ; ++k)
				SetVoxel(i,j,k,0);

	// Y planes
	for (unsigned int i=SVoxelSpace::MinBorder; i<SVoxelSpace::DimX-SVoxelSpace::MinBorder; ++i)
		for (unsigned int j=0; j<SVoxelSpace::MinBorder; ++j)
			for (unsigned int k=0; k<SVoxelSpace::DimZ; ++k)
				SetVoxel(i,j,k,0);

	for (unsigned int i=SVoxelSpace::MinBorder; i<SVoxelSpace::DimX-SVoxelSpace::MinBorder; ++i)
		for (unsigned int j=SVoxelSpace::DimY-SVoxelSpace::MinBorder; j<SVoxelSpace::DimY; ++j)
			for (unsigned int k=0; k<SVoxelSpace::DimZ; ++k)
				SetVoxel(i,j,k,0);

	// Z planes
	for (unsigned int i=SVoxelSpace::MinBorder; i<SVoxelSpace::DimX-SVoxelSpace::MinBorder; ++i)
		for (unsigned int j=SVoxelSpace::MinBorder; j<SVoxelSpace::DimY-SVoxelSpace::MinBorder; ++j)
			for (unsigned int k=0; k<SVoxelSpace::MinBorder; ++k)
				SetVoxel(i,j,k,0);

	for (unsigned int i=SVoxelSpace::MinBorder; i<SVoxelSpace::DimX-SVoxelSpace::MinBorder; ++i)
		for (unsigned int j=SVoxelSpace::MinBorder; j<SVoxelSpace::DimY-SVoxelSpace::MinBorder; ++j)
			for (unsigned int k=SVoxelSpace::DimZ-SVoxelSpace::MinBorder; k<SVoxelSpace::DimZ; ++k)
				SetVoxel(i,j,k,0);

	if (PrintToConsole)
		std::cout << "done." << std::endl;
}

// ======================================================
// post processing
// ======================================================

inline bool DunGen::CVoxelCave::IsBoundaryVoxel(unsigned int x_,unsigned int y_,unsigned int z_) const
{
	// a border voxel has to be a 0-voxel
	if (0 != GetVoxel(x_,y_,z_))
		return false;

	// if it has a 6-connected 1-voxel, it is a border voxel
	if (1 == GetVoxel(x_-1,y_,z_) || 1 == GetVoxel(x_+1,y_,z_) ||
		1 == GetVoxel(x_,y_-1,z_) || 1 == GetVoxel(x_,y_+1,z_) ||
		1 == GetVoxel(x_,y_,z_-1) || 1 == GetVoxel(x_,y_,z_+1))
		return true;

	// else: no border voxel
	return false;
}

inline bool DunGen::CVoxelCave::IsBoundaryVoxelExtended(unsigned int x_,unsigned int y_,unsigned int z_) const
{
	// a border voxel has to be a 0-voxel
	if (0 != GetVoxel(x_,y_,z_))
		return false;

	// if it has a 6-connected 1-voxel, it is a border voxel
	if (1 == GetVoxel(x_-1,y_,z_) || 1 == GetVoxel(x_+1,y_,z_) ||
		1 == GetVoxel(x_,y_-1,z_) || 1 == GetVoxel(x_,y_+1,z_) ||
		1 == GetVoxel(x_,y_,z_-1) || 1 == GetVoxel(x_,y_,z_+1))
		return true;

	// this is extended to 18-connected voxels
	if (1 == GetVoxel(x_-1,y_-1,z_) || 1 == GetVoxel(x_-1,y_+1,z_) ||
		1 == GetVoxel(x_+1,y_-1,z_) || 1 == GetVoxel(x_+1,y_+1,z_) ||

		1 == GetVoxel(x_-1,y_,z_-1) || 1 == GetVoxel(x_-1,y_,z_+1) ||
		1 == GetVoxel(x_+1,y_,z_-1) || 1 == GetVoxel(x_+1,y_,z_+1) ||
		
		1 == GetVoxel(x_,y_-1,z_-1) || 1 == GetVoxel(x_,y_-1,z_+1) ||
		1 == GetVoxel(x_,y_+1,z_-1) || 1 == GetVoxel(x_,y_+1,z_+1))
		return true;

	// else: no border voxel
	return false;
}

inline bool DunGen::CVoxelCave::IsBoundaryVoxelBeginUpward(unsigned int x_,unsigned int y_,unsigned int z_) const
{
	// a border voxel has to be a 0-voxel
	if (0 != GetVoxel(x_,y_,z_))
		return false;

	// if it has a 6-connected 1-voxel, it is a border voxel
	// check only for one voxel, for faster test
	if (1 == GetVoxel(x_+1,y_,z_))
		return true;

	// else: no border voxel
	return false;
}

inline bool DunGen::CVoxelCave::IsBoundaryVoxelBeginDownward(unsigned int x_,unsigned int y_,unsigned int z_) const
{
	// a border voxel has to be a 0-voxel
	if (0 != GetVoxel(x_,y_,z_))
		return false;

	// if it has a 6-connected 1-voxel, it is a border voxel
	// check only for one voxel, for faster test
	if (1 == GetVoxel(x_-1,y_,z_))
		return true;

	// else: no border voxel
	return false;
}

unsigned int DunGen::CVoxelCave::Filter()
{
	// remove all 0-voxels that are not part of the outer connected component of 0-voxels

	unsigned int lastXofSearch = UINT_MAX;

	// step 1: compute and mark outer hull
	if (PrintToConsole) std::cout << "filter step 1: marking outer hull..." << std::endl;
	SVoxelToDraw startVoxel;
	// find start voxel for outer hull
	for (unsigned int i=MinBorderFilter; i<SVoxelSpace::DimX-MinBorderFilter; ++i)
		for (unsigned int j=MinBorderFilter; j<SVoxelSpace::DimY-MinBorderFilter; ++j)
			for (unsigned int k=MinBorderFilter; k<SVoxelSpace::DimZ-MinBorderFilter; ++k)
				if (IsBoundaryVoxelBeginUpward(i,j,k))
				{
					// store voxel
					startVoxel.X=i; startVoxel.Y=j; startVoxel.Z=k;
					lastXofSearch = i;
					// break search
					i = SVoxelSpace::DimX; j = SVoxelSpace::DimY; break;
				}
	
	if (lastXofSearch >= SVoxelSpace::DimX-MinBorderFilter)
	{
		if (PrintToConsole) std::cout << "filtering did not find outer hull -> aborting." << std::endl;
		return 0;
	}

	// queue for for breadth-first search over border voxels
	// (depth-first search (with stack) not recommendable, because "graph" is not acyclic -> extreme high memory usage (tested))
	std::queue<SVoxelToDraw> queueVoxel;
	SVoxelToDraw actVoxel;
	queueVoxel.push(startVoxel);

	// mark outer hull
	while (!queueVoxel.empty())
	{
		// take voxel from queue
		actVoxel = queueVoxel.front();
		queueVoxel.pop();

		// check all 6-connected neighbor voxels
		if (IsBoundaryVoxelExtended(actVoxel.X-1,actVoxel.Y,actVoxel.Z))
		{
			// create new voxel
			SVoxelToDraw newVoxel;
			newVoxel.X = actVoxel.X-1; newVoxel.Y = actVoxel.Y; newVoxel.Z = actVoxel.Z;
			// store into queue
			queueVoxel.push(newVoxel);
			// then mark voxel
			SetVoxel(newVoxel.X,newVoxel.Y,newVoxel.Z,HelperVoxel);
		}
		if (IsBoundaryVoxelExtended(actVoxel.X+1,actVoxel.Y,actVoxel.Z))
		{
			// create new voxel
			SVoxelToDraw newVoxel;
			newVoxel.X = actVoxel.X+1; newVoxel.Y = actVoxel.Y; newVoxel.Z = actVoxel.Z;
			// store into queue
			queueVoxel.push(newVoxel);
			// then mark voxel
			SetVoxel(newVoxel.X,newVoxel.Y,newVoxel.Z,HelperVoxel);
		}
		if (IsBoundaryVoxelExtended(actVoxel.X,actVoxel.Y-1,actVoxel.Z))
		{
			// create new voxel
			SVoxelToDraw newVoxel;
			newVoxel.X = actVoxel.X; newVoxel.Y = actVoxel.Y-1; newVoxel.Z = actVoxel.Z;
			// store into queue
			queueVoxel.push(newVoxel);
			// then mark voxel
			SetVoxel(newVoxel.X,newVoxel.Y,newVoxel.Z,HelperVoxel);
		}
		if (IsBoundaryVoxelExtended(actVoxel.X,actVoxel.Y+1,actVoxel.Z))
		{
			// create new voxel
			SVoxelToDraw newVoxel;
			newVoxel.X = actVoxel.X; newVoxel.Y = actVoxel.Y+1; newVoxel.Z = actVoxel.Z;
			// store into queue
			queueVoxel.push(newVoxel);
			// then mark voxel
			SetVoxel(newVoxel.X,newVoxel.Y,newVoxel.Z,HelperVoxel);
		}
		if (IsBoundaryVoxelExtended(actVoxel.X,actVoxel.Y,actVoxel.Z-1))
		{
			// create new voxel
			SVoxelToDraw newVoxel;
			newVoxel.X = actVoxel.X; newVoxel.Y = actVoxel.Y; newVoxel.Z = actVoxel.Z-1;
			// store into queue
			queueVoxel.push(newVoxel);
			// dann Voxel  markieren
			SetVoxel(newVoxel.X,newVoxel.Y,newVoxel.Z,HelperVoxel);
		}
		if (IsBoundaryVoxelExtended(actVoxel.X,actVoxel.Y,actVoxel.Z+1))
		{
			// create new voxel
			SVoxelToDraw newVoxel;
			newVoxel.X = actVoxel.X; newVoxel.Y = actVoxel.Y; newVoxel.Z = actVoxel.Z+1;
			// store into queue
			queueVoxel.push(newVoxel);
			// then mark voxel
			SetVoxel(newVoxel.X,newVoxel.Y,newVoxel.Z,HelperVoxel);
		}
	}

	// step 2: remove hovering voxel fragments
	if (PrintToConsole) std::cout << "filter step 2: removing hovering voxel fragments..." << std::endl;
	unsigned int numberOfVoxels = 0;
	// search for remaining border voxels (they belong to hovering fragments)
	for (unsigned int i=lastXofSearch+2; i<SVoxelSpace::DimX-MinBorderFilter; ++i)
		for (unsigned int j=MinBorderFilter; j<SVoxelSpace::DimY-MinBorderFilter; ++j)
			for (unsigned int k=MinBorderFilter; k<SVoxelSpace::DimZ-MinBorderFilter; ++k)
				if (IsBoundaryVoxelBeginDownward(i,j,k))
				{
					// count removed voxels
					++numberOfVoxels;
					// clear voxel
					SetVoxel(i,j,k,1);
				}

	// step 3: restore original marking of outer hull
	if (PrintToConsole) std::cout << "filter step 3: restore original marking of outer hull..." << std::endl;

	for (unsigned int i=lastXofSearch; i<SVoxelSpace::DimX-MinBorderFilter; ++i)
		for (unsigned int j=MinBorderFilter; j<SVoxelSpace::DimY-MinBorderFilter; ++j)
			for (unsigned int k=MinBorderFilter; k<SVoxelSpace::DimZ-MinBorderFilter; ++k)
				if (HelperVoxel == GetVoxel(i,j,k))
					SetVoxel(i,j,k,0);		// original marking was 0

	return numberOfVoxels;
}

void DunGen::CVoxelCave::Erode(double erosionLikelihood_)
{
	if (PrintToConsole) std::cout << "erode step 1: mark voxels to erode..." << std::endl;
	// step 1: mark voxels to erode
	for (unsigned int i=SVoxelSpace::MinBorder; i<SVoxelSpace::DimX-SVoxelSpace::MinBorder; ++i)
	{
		for (unsigned int j=SVoxelSpace::MinBorder; j<SVoxelSpace::DimY-SVoxelSpace::MinBorder; ++j)
			for (unsigned int k=SVoxelSpace::MinBorder; k<SVoxelSpace::DimZ-SVoxelSpace::MinBorder; ++k)
				if (IsBoundaryVoxel(i,j,k))
					if (RandomGenerator->GetRandomNumber_01()<=erosionLikelihood_)
						SetVoxel(i,j,k,HelperVoxel);
	}

	if (PrintToConsole) std::cout << "erode step 2: delete marked voxels..." << std::endl;
	// step 2: delete marked voxels
	for (unsigned int i=SVoxelSpace::MinBorder; i<SVoxelSpace::DimX-SVoxelSpace::MinBorder; ++i)
		for (unsigned int j=SVoxelSpace::MinBorder; j<SVoxelSpace::DimY-SVoxelSpace::MinBorder; ++j)
			for (unsigned int k=SVoxelSpace::MinBorder; k<SVoxelSpace::DimZ-SVoxelSpace::MinBorder; ++k)
				if (HelperVoxel == GetVoxel(i,j,k))
					SetVoxel(i,j,k,1);
}

unsigned char (&DunGen::CVoxelCave::GetVoxelSpace())[SVoxelSpace::DimX][SVoxelSpace::DimY][SVoxelSpace::DimZ]
{
	return Voxel;
}