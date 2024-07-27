// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H

// Namespace DunGen : DungeonGenerator
namespace DunGen
{
	/// random number generator: linear congruential generator
	///
	/// as described by Knuth, The Art of Computer Programming, Volume 2: Seminumerical Algorithms
	/// X[i+1] = (a*X[i]+c) mod m
	class CRandomGenerator
	{
	public:
		
		/// constructor
		CRandomGenerator()
		{
			SetSeed(0);
			SetA(1103515245);
			SetC(12345);
			SetM(32768);
		}
		/// destructor
		~CRandomGenerator()
		{
		}

		// ==================================
		// set/get parameters
		// ==================================

		/// set parameter A
		void SetA(unsigned int a_)
		{
			A = a_;
		}

		/// set parameter C
		void SetC(unsigned int c_)
		{
			C = c_;
		}

		/// set parameter M
		void SetM(unsigned int m_)
		{
			M = (m_<2) ? 2 : m_; // everything less than 2 is pointless (case 1) or invalid (case 0)
			MaxValuePlus1 = static_cast<double>(M);
			MaxValue = MaxValuePlus1 - 1.0;
		}

		/// set random seed
		void SetSeed(unsigned int seed_) const
		{
			X = seed_;
		}

		/// read parameter A
		unsigned int GetA() const
		{
			return A;
		}

		/// read parameter C
		unsigned int GetC() const
		{
			return C;
		}

		/// read parameter M
		unsigned int GetM() const
		{
			return M;
		}

		// ==================================
		// random number generation functions
		// ==================================

		/// returns random integer random number between 0 and M-1 
		unsigned int GetRandomNumber() const
		{
			return RandomNumber();
		}

		/// returns random integer in [_Minimum,_Maximum]
		///
		/// it is necessary that: M >= Maximum >= Minimum
		int GetRandomNumberMinMax(int minimum_, int maximum_) const
		{
			return static_cast<int>(RandomNumber() % static_cast<unsigned int>(maximum_-minimum_+1)) + minimum_;
		}

		/// returns random integer in [0,_Maximum)
		///
		/// it is necessary that: M >= Maximum > 0
		unsigned int GetRandomNumber0Max_(unsigned int maximum_) const
		{
			return RandomNumber()%maximum_;
		}

		/// returns random double in [0,1]
		double GetRandomNumber01() const
		{
			return static_cast<double>(RandomNumber()) / MaxValue;
		}

		/// returns random double in (0,1]
		double GetRandomNumber_01() const
		{
			return static_cast<double>(RandomNumber()+1) / MaxValuePlus1;
		}

	private:
		
		/// core random function
		inline unsigned int RandomNumber() const
		{
			X = (A * X + C) % M;
			return X;
		}
		
		/// parameter A
		unsigned int A;

		/// parameter C
		unsigned int C;

		/// parameter M = max value + 1
		unsigned int M;

		/// parameter X = actual seed
		mutable unsigned int X;

		/// pre conversion to double: maximum value
		double MaxValue;

		/// pre conversion to double: maximum value + 1
		double MaxValuePlus1;

	};

} // END NAMESPACE DunGen

#endif