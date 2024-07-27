// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#ifndef EVENTRECEIVER_H
#define EVENTRECEIVER_H

#include <irrlicht.h>
#include <vector>
#include <DunGen.h>

class CEventReceiver : public irr::IEventReceiver
{
public:
	/// Constructor.
	CEventReceiver();

	/// Destructor.
	~CEventReceiver();

	/// Initialize the object.
	void Intialize(irr::IrrlichtDevice* irrDevice_);

	/// What to do on an event?
	virtual bool OnEvent(const irr::SEvent& event_);

	/// Gets the first person camera.
	irr::scene::ICameraSceneNode* GetCamera();

private:
	/// Apply current settings to DunGen.
	void Apply();

	/// Generates a random string based on given parameters.
	std::string GenerateRandomString(const std::string& allowedSymbols_, unsigned int minLength_, unsigned int maxLength_);

	/// Generates and adds a random rule.
	void AddRuleRandom(char symbol_, const std::string& allowedSymbols_, unsigned int minLength_, unsigned int maxLength_);

	/// Generates a random start string.
	void SetStartRandom(const std::string& allowedSymbols_, unsigned int minLength_, unsigned int maxLength_);

	/// Sets the specified parameter to a random value, based on parameters
	void SetParameterRandom(DunGen::ELSystemParameter::Enum parameter_, double minValue_, double maxValue_, int digits_);

	/// Generate a cave.
	void GenerateCave();

	/// Save the cave as XML.
	void SaveCave();

private:
	struct LSystemRule
	{
		char Symbol;
		std::string Replacement;
	};

private:
	DunGen::CDunGen* DungeonGenerator;
	irr::IrrlichtDevice* IrrDevice;
	irr::scene::ICameraSceneNode* Camera;

	// L-system parameters and rules
	std::vector<LSystemRule> Rules;
	std::string Start;
	double AngleYaw;
	double AnglePitch;
	double AngleRoll;
	double RadiusStart;
	double RadiusFactor;
	double RadiusDecrement;
	unsigned int Derivation;
};

#endif