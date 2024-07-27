// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <irrlicht.h>

/// declaration of PI
const double M_PI = 3.1415926535897932384626433832795028841971693993751;
const double M_DegToRad = M_PI / 180.0;

/// convert double to integer with rounding up and down
inline int d2i(double d_)
{
	return static_cast<int>(d_<0?d_-0.5:d_+0.5);
}

/// cast float vector into double vector
inline irr::core::vector3d<double> vec3D(const irr::core::vector3df& inVec_)
{
	return irr::core::vector3d<double>(static_cast<double>(inVec_.X),
		static_cast<double>(inVec_.Y), static_cast<double>(inVec_.Z));
}

/// cast double vector into float vector
inline irr::core::vector3df vec3F(const irr::core::vector3d<double>& inVec_)
{
	return irr::core::vector3df(static_cast<irr::f32>(inVec_.X),
		static_cast<irr::f32>(inVec_.Y), static_cast<irr::f32>(inVec_.Z));
}

/// compute weight values for each point of a triangle, dependent on angle in this point
///
/// source: CMeshManipulator.cpp (Irrlicht Release 1.7.3) - function getAngleWeight()
/// slightly modified for double precision
inline irr::core::vector3d<double> computeAngleWeight(
	const irr::core::vector3d<double>& v1_,
	const irr::core::vector3d<double>& v2_,
	const irr::core::vector3d<double>& v3_)
{	
	// compute the side lenght of each side
	double a = v2_.getDistanceFromSQ(v3_);
	double asqrt = sqrt(a);
	double b = v1_.getDistanceFromSQ(v3_);
	double bsqrt = sqrt(b);
	double c = v1_.getDistanceFromSQ(v2_);
	double csqrt = sqrt(c);

	// compute the angle in each point
	return irr::core::vector3d<double>(
		acos((b + c - a) / (2.0 * bsqrt * csqrt)),
		acos((-b + c + a) / (2.0 * asqrt * csqrt)),
		acos((b - c + a) / (2.0 * bsqrt * asqrt)));
}

/// 4x4 double matrix with only some basic functions
struct SMatrix4D 
{
	/// convert float matrix into double matrix
	explicit inline SMatrix4D(const irr::core::matrix4& matrix4F_)
	{
		for (unsigned i=0; i<16; ++i)
			M[i] = static_cast<double>(matrix4F_[i]);
	}

	/// store double matrix
	explicit inline SMatrix4D(const irr::core::CMatrix4<double>& matrix4D_)
	{
		for (unsigned i=0; i<16; ++i)
			M[i] = matrix4D_[i];
	}

	/// transforming a vector (x,y,z,0.0) into (x',y',z',0.0)
	inline void MultiplyWith1x3_0(irr::core::vector3d<double>& vec_) const
	{
		irr::core::vector3d<double> tmp = vec_;
		vec_.X = tmp.X*M[0] + tmp.Y*M[4] + tmp.Z*M[8];
		vec_.Y = tmp.X*M[1] + tmp.Y*M[5] + tmp.Z*M[9];
		vec_.Z = tmp.X*M[2] + tmp.Y*M[6] + tmp.Z*M[10];
	}

	/// transforming a vector (x,y,z,1.0) into (x',y',z',1.0)
	inline void MultiplyWith1x3_1(irr::core::vector3d<double>& vect_) const
	{
		irr::core::vector3d<double> tmp = vect_;
		vect_.X = tmp.X*M[0] + tmp.Y*M[4] + tmp.Z*M[8] + M[12]*1.0;
		vect_.Y = tmp.X*M[1] + tmp.Y*M[5] + tmp.Z*M[9] + M[13]*1.0;
		vect_.Z = tmp.X*M[2] + tmp.Y*M[6] + tmp.Z*M[10] + M[14]*1.0;
	}

	/// the matrix
	double M[16];
};

/// project a vector onto a plane spanned by given up and left vector (both normalized)
///
/// \returns (X,Y)-coordinates in the plane with X+ ... -left, Y+ ... up
inline irr::core::vector2d<double> projectVectorToPlane(const irr::core::vector3d<double>& vector_,
	const irr::core::vector3d<double>& left_, const irr::core::vector3d<double>& up_)
{
	return irr::core::vector2d<double>(-vector_.dotProduct(left_),vector_.dotProduct(up_));
}

#endif