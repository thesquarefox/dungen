// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <time.h>

// Namespace DunGen : DungeonGenerator
namespace DunGen
{
	/// timer class: holds 4 timers, that can be used independently
	class CTimer
	{
	public:
		/// start timer i
		void Start(unsigned int i_)
		{
			
			TimeStart[i_] = clock();
			TimeTemp[i_] = 0;
		}

		// pause timer i
		void Pause(unsigned int i_)
		{			
			TimeEnd = clock();
			TimeTemp[i_] += TimeEnd - TimeStart[i_];
		}

		/// continue timer i
		void Continue(unsigned int i_)
		{			
			TimeStart[i_] = clock();
		}

		/// stop timer i and print the elapsed time to console
		void Stop(unsigned int i_)
		{			
			TimeEnd = clock();
			TimeTemp[i_] += TimeEnd - TimeStart[i_];
			std::cout << "CPU-Time: " << TimeTemp[i_] << " , with CLOCKS_PER_SEC: " << CLOCKS_PER_SEC << std::endl;
		}

	private:
		// time variables:

		/// start times
		clock_t TimeStart[4];
		/// temporary time variables
		clock_t TimeTemp[4];
		/// current end time
		clock_t TimeEnd;
	};
	
} // END NAMESPACE DunGen

#endif