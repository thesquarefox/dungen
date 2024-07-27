// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#include "LSystem.h"
#include "Helperfunctions.h"

// ======================================================
// constructor / destructor
// ======================================================

DunGen::CLSystem::CLSystem()
	: AngleYaw(5.0)
	, AnglePitch(5.0)
	, AngleRoll(5.0)
	, RadiusStart(10.0)
	, RadiusFactor(1.0)
	, RadiusDecrement(1.0)	
{
	Derivations.push_back("F");
}

DunGen::CLSystem::~CLSystem()
{
}

// ======================================================
// setting parameters
// ======================================================

void DunGen::CLSystem::AddRule(char symbol_, const std::string& substitution_)
{
	// search, if symbol already present
	for(unsigned int i=0; i<SubstitutionRules.size(); ++i)
		if (symbol_ == SubstitutionRules[i].Symbol)
		{
			// if present, then actualize
			SubstitutionRules[i].Substitution = substitution_;
			return;
		}
	
	// else: add symbol
	// construct new rule
	SSubstitutionRule newRule;
	newRule.Symbol = symbol_;
	newRule.Substitution = substitution_;
	// add this rule
	SubstitutionRules.push_back(newRule);
}
void DunGen::CLSystem::DeleteRules()
{
	// delete all rules
	SubstitutionRules.clear();
}
void DunGen::CLSystem::SetStart(const std::string& start_)
{
	// store new value
	Derivations.clear();
	Derivations.push_back(start_);	
}
void DunGen::CLSystem::SetParameter(ELSystemParameter::Enum parameter_, double value_)
{
	// store the appropriate value
	switch(parameter_)
	{
		case ELSystemParameter::ANGLE_YAW:			AngleYaw=value_;			break;
		case ELSystemParameter::ANGLE_PITCH:		AnglePitch=value_;			break;
		case ELSystemParameter::ANGLE_ROLL:			AngleRoll=value_;			break;
		case ELSystemParameter::RADIUS_START:		RadiusStart=value_;			break;
		case ELSystemParameter::RADIUS_FACTOR:		RadiusFactor=value_;		break;
		case ELSystemParameter::RADIUS_DECREMENT:	RadiusDecrement=value_;		break;
	}
}

// ======================================================
// reading parameters and results
// ======================================================

std::string DunGen::CLSystem::GetHighestDerivation()
{
	// return last derivation
	if (!Derivations.empty())
		return Derivations.back();

	// if no derivation available, return empty string
	const std::string tmp;
	return tmp;
}

double DunGen::CLSystem::GetParameter(ELSystemParameter::Enum parameter_)
{
	// return the appropriate value
	switch(parameter_)
	{
		case ELSystemParameter::ANGLE_YAW:			return AngleYaw;
		case ELSystemParameter::ANGLE_PITCH:		return AnglePitch;
		case ELSystemParameter::ANGLE_ROLL:			return AngleRoll;
		case ELSystemParameter::RADIUS_START:		return RadiusStart;
		case ELSystemParameter::RADIUS_FACTOR:		return RadiusFactor;
		case ELSystemParameter::RADIUS_DECREMENT:	return RadiusDecrement;
	}

	return 0.0;
}

// ======================================================
// generating strings
// ======================================================

inline std::string DunGen::CLSystem::Replace(const std::string& baseString_)
{
	// initialize new string
	std::string newString = "";
	
	// iterate over basestring
	bool existsSubstitution;
	for (unsigned int i=0; i<baseString_.size(); ++i)
	{
		existsSubstitution = false;
		// find matching substitution
		for (unsigned int j=0; j<SubstitutionRules.size(); ++j)
			if (baseString_[i] == SubstitutionRules[j].Symbol)
			{
				// substitute
				newString += SubstitutionRules[j].Substitution;
				existsSubstitution = true;
				// break the loop
				j = SubstitutionRules.size();
			}
		// no substitution was matching: symbol is substituted by itself
		if (!existsSubstitution) newString += baseString_[i];
	}
			
	// return result of substitution
	return newString;
}

// ======================================================
// generation L-system derivations
// ======================================================
unsigned int DunGen::CLSystem::GenerateDerivations(unsigned int maxIterations_)
{
	// generate iteration strings
	std::string actString = Derivations[0]; // iteration 0 = start
	// only the start has to be here
	Derivations.clear();
	Derivations.push_back(actString);

	for (unsigned int i=0; i<maxIterations_; ++i)
	{	
		// replicate new string
		actString = Replace(actString);

		// break iteration, if string too large
		if (MAX_SIZE<actString.size()) break;

		// add string to the derivation object
		Derivations.push_back(actString);
	}

	// return the number of iterations
	return Derivations.size();
}