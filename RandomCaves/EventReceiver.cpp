// Copyright (C) 2011-2014 by Maximilian Hönig
// This file is part of "DunGen - the Dungeongenerator".
// For conditions of distribution and use, see licence.txt provided together with DunGen.

#include "EventReceiver.h"
#include <iostream>
#include <time.h>

CEventReceiver::CEventReceiver()
{
}

CEventReceiver::~CEventReceiver()
{
	delete DungeonGenerator;
}

void CEventReceiver::Intialize(irr::IrrlichtDevice* irrDevice_)
{
	IrrDevice = irrDevice_;
	DungeonGenerator = new DunGen::CDunGen(irrDevice_);
	DungeonGenerator->StartUp();
	DungeonGenerator->SetPrintToConsole(true);
	DungeonGenerator->MaterialSetCaveMultiColor(false,true);

	Camera = IrrDevice->getSceneManager()->addCameraSceneNodeFPS(0,100.0f,0.05f);
	Camera->setFarValue(20000.0f);
	Camera->setPosition(irr::core::vector3df(0,0,0));
	Camera->setTarget(irr::core::vector3df(99999.f, 99999.f, 99999.f));
}

irr::scene::ICameraSceneNode* CEventReceiver::GetCamera()
{
	return Camera;
}

bool CEventReceiver::OnEvent(const irr::SEvent& event_)
{
	if (event_.EventType == irr::EET_KEY_INPUT_EVENT && false == event_.KeyInput.PressedDown)
	{
		switch(event_.KeyInput.Key)
		{
		case irr::KEY_F5 :
			GenerateCave();
			break;
		case irr::KEY_F6 :
			SaveCave();
			break;
		case irr::KEY_F7 :
			if (0 < Derivation)
				--Derivation;
			Apply();
			break;
		case irr::KEY_F8 :
			++Derivation;
			Apply();
			break;
		}
	}
	return false;
}

void CEventReceiver::Apply()
{
	// output parameters on console
	std::cout << "=== Parameters: F[]+-ouzg|$ ===" << std::endl;
	std::cout << "Derivation: " << Derivation << std::endl;
	std::cout << "AngleYaw: " << AngleYaw << std::endl;
	std::cout << "AnglePitch: " << AnglePitch << std::endl;
	std::cout << "AngleRoll: " << AngleRoll << std::endl;
	std::cout << "RadiusStart: " << RadiusStart << std::endl;
	std::cout << "RadiusFactor: " << RadiusFactor << std::endl;
	std::cout << "RadiusDecrement: " << RadiusDecrement << std::endl;
	std::cout << "Start: " << Start << std::endl;
	for (unsigned int i=0; i<Rules.size(); ++i)
	{
		std::cout << Rules[i].Symbol << " -> " << Rules[i].Replacement << std::endl;
	}
	std::cout << "=== /Parameters: ===" << std::endl;

	// create cave
	DungeonGenerator->CreateLSystemDerivation(Derivation);
	DungeonGenerator->CreateVoxelCave();
	DungeonGenerator->CreateMeshCave();

	IrrDevice->getSceneManager()->getRootSceneNode()->removeAll();
	Camera = IrrDevice->getSceneManager()->addCameraSceneNodeFPS(0,100.0f,0.05f);
	Camera->setFarValue(20000.0f);
	Camera->setPosition(irr::core::vector3df(0,0,0));
	Camera->setTarget(irr::core::vector3df(99999.f, 99999.f, 99999.f));
	DungeonGenerator->AddDungeon(IrrDevice->getSceneManager()->getRootSceneNode(),IrrDevice->getSceneManager());
}

std::string CEventReceiver::GenerateRandomString(const std::string& allowedSymbols_, unsigned int minLength_, unsigned int maxLength_)
{
	std::string randomString = "";
	unsigned int length = rand()%(maxLength_-minLength_)+minLength_;

	// roll first symbol
	unsigned int index;
	index = rand()%(allowedSymbols_.length());
	randomString += allowedSymbols_[index];

	// catch senseless data input
	bool onlySymb1 = true;
	bool onlySymb2 = true;

	for (unsigned int i = 0; i<allowedSymbols_.size(); ++i)
	{
		if (allowedSymbols_.at(i) != '|')
			onlySymb1 = false;
		if (allowedSymbols_.at(i) != '$')
			onlySymb2 = false;
	}

	if (onlySymb1 || onlySymb2) 
		return randomString;

	// roll remaining symbols
	for (unsigned int i = 1; i<length; i++)
	{
		// prevent senseless symbol combination
		index = rand()%(allowedSymbols_.length());
		while ((randomString.at(i-1)=='[' && allowedSymbols_.at(index) ==']')
			|| (randomString.at(i-1)=='+' && allowedSymbols_.at(index) =='-')
			|| (randomString.at(i-1)=='-' && allowedSymbols_.at(index) =='+')
			|| (randomString.at(i-1)=='u' && allowedSymbols_.at(index) =='o')
			|| (randomString.at(i-1)=='o' && allowedSymbols_.at(index) =='u')
			|| (randomString.at(i-1)=='g' && allowedSymbols_.at(index) =='z')
			|| (randomString.at(i-1)=='z' && allowedSymbols_.at(index) =='g')
			|| (randomString.at(i-1)=='|' && allowedSymbols_.at(index) =='|')
			|| (randomString.at(i-1)=='$' && allowedSymbols_.at(index) =='$'))
			index = rand()%(allowedSymbols_.length());

		// add rolled symbol
		randomString += allowedSymbols_[index];
	}

	return randomString;
}

void CEventReceiver::AddRuleRandom(char symbol_, const std::string& allowedSymbols_, unsigned int minLength_, unsigned int maxLength_)
{
	// compute and add rule
	LSystemRule newRule;
	newRule.Symbol = symbol_;
	newRule.Replacement = GenerateRandomString(allowedSymbols_,minLength_,maxLength_);
	DungeonGenerator->LSystemAddRule(newRule.Symbol, newRule.Replacement);

	// save result
	Rules.push_back(newRule);
}

void CEventReceiver::SetStartRandom(const std::string& allowedSymbols_, unsigned int minLength_, unsigned int maxLength_)
{
	// compute and set start
	Start = GenerateRandomString(allowedSymbols_,minLength_,maxLength_);
	DungeonGenerator->LSystemSetStart(Start);
}

void CEventReceiver::SetParameterRandom(DunGen::ELSystemParameter::Enum parameter_, double minValue_, double maxValue_, int digits_)
{
	// shift comma to high
	double conversionFactor = pow(10.0,digits_);
	int minValue =  static_cast<int>(minValue_ * conversionFactor);
	if (minValue_ * conversionFactor > static_cast<double>(minValue))
		++minValue;
	int maxValue = static_cast<int>(maxValue_ * conversionFactor);

	// roll result
	int tmp = (rand()%(maxValue-minValue))+minValue; 

	// shift comma back to low
	double result = ((double)tmp)/conversionFactor;

	// set parameter
	DungeonGenerator->LSystemSetParameter(parameter_, result);

	// save result
	switch(parameter_)
	{
	case DunGen::ELSystemParameter::ANGLE_YAW:
		AngleYaw = result;
		break;
	case DunGen::ELSystemParameter::ANGLE_PITCH:
		AnglePitch = result;
		break;
	case DunGen::ELSystemParameter::ANGLE_ROLL:
		AngleRoll = result;
		break;
	case DunGen::ELSystemParameter::RADIUS_START:
		RadiusStart = result;
		break;
	case DunGen::ELSystemParameter::RADIUS_FACTOR:
		RadiusFactor = result;
		break;
	case DunGen::ELSystemParameter::RADIUS_DECREMENT:
		RadiusDecrement = result;
		break;
	}
}

void CEventReceiver::GenerateCave()
{
	std::cout << "Generating Cave ... " << std::endl;
	srand (static_cast<unsigned int>(time(NULL)));
	Rules.clear();
	DungeonGenerator->LSystemDeleteRules();

	irr::io::IReadFile* file = IrrDevice->getFileSystem()->createAndOpenFile("RandomCavesConfig.xml");
	if (!file)
	{
		std::cout << "failed." << std::endl;
		return;
	}
	irr::io::IXMLReader* xmlReader = IrrDevice->getFileSystem()->createXMLReader(file);

	std::string replacementCandidates;

	while(xmlReader->read())
	{
		if (xmlReader->getNodeType() == irr::io::EXN_ELEMENT)
		{
			if (irr::core::stringw("General") == xmlReader->getNodeName())
			{
				Derivation = xmlReader->getAttributeValueAsInt(L"Derivation");
			}
			else if (irr::core::stringw("Start") == xmlReader->getNodeName())
			{
				std::string symbols = std::string(irr::core::stringc(xmlReader->getAttributeValue(L"Symbols")).c_str());
				SetStartRandom(symbols, xmlReader->getAttributeValueAsInt(L"Min"), xmlReader->getAttributeValueAsInt(L"Max"));
				replacementCandidates += symbols;
			}
			else if (irr::core::stringw("Rules") == xmlReader->getNodeName())
			{
				std::string symbols = std::string(irr::core::stringc(xmlReader->getAttributeValue(L"Symbols")).c_str());

				// fetch replacement candidates
				replacementCandidates += symbols;
				std::string symbolsToReplace = "F";
				char curChar;
				bool alreadyThere;
				for (unsigned i = 0; i<replacementCandidates.size(); ++i)
				{
					curChar = replacementCandidates.at(i);

					if (curChar != '[' && curChar != ']' && curChar != '!'
						&& curChar != '+' && curChar != '-'
						&& curChar != 'u' && curChar != 'o' 
						&& curChar != 'g' && curChar != 'z' 
						&& curChar != '|' && curChar != '$')
					{
						alreadyThere = false;
						for (unsigned j = 0; j<symbolsToReplace.size(); ++j)
							if (curChar == symbolsToReplace.at(j))
								alreadyThere = true;

						if (!alreadyThere)
							symbolsToReplace.append(1,curChar);
					}
				}

				// add rules
				for (unsigned int i = 0; i < symbolsToReplace.length(); ++i)
					AddRuleRandom(symbolsToReplace[i], symbols, xmlReader->getAttributeValueAsInt(L"Min"), xmlReader->getAttributeValueAsInt(L"Max"));
			}
			else if (irr::core::stringw("AngleYaw") == xmlReader->getNodeName())
			{
				SetParameterRandom(DunGen::ELSystemParameter::ANGLE_YAW, xmlReader->getAttributeValueAsFloat(L"Min"),
					xmlReader->getAttributeValueAsFloat(L"Max"), xmlReader->getAttributeValueAsInt(L"Digits"));
			}
			else if (irr::core::stringw("AnglePitch") == xmlReader->getNodeName())
			{
				SetParameterRandom(DunGen::ELSystemParameter::ANGLE_PITCH, xmlReader->getAttributeValueAsFloat(L"Min"),
					xmlReader->getAttributeValueAsFloat(L"Max"), xmlReader->getAttributeValueAsInt(L"Digits"));
			}
			else if (irr::core::stringw("AngleRoll") == xmlReader->getNodeName())
			{
				SetParameterRandom(DunGen::ELSystemParameter::ANGLE_ROLL, xmlReader->getAttributeValueAsFloat(L"Min"),
					xmlReader->getAttributeValueAsFloat(L"Max"), xmlReader->getAttributeValueAsInt(L"Digits"));
			}
			else if (irr::core::stringw("RadiusStart") == xmlReader->getNodeName())
			{
				SetParameterRandom(DunGen::ELSystemParameter::RADIUS_START, xmlReader->getAttributeValueAsFloat(L"Min"),
					xmlReader->getAttributeValueAsFloat(L"Max"), xmlReader->getAttributeValueAsInt(L"Digits"));
			}
			else if (irr::core::stringw("RadiusFactor") == xmlReader->getNodeName())
			{
				SetParameterRandom(DunGen::ELSystemParameter::RADIUS_FACTOR, xmlReader->getAttributeValueAsFloat(L"Min"),
					xmlReader->getAttributeValueAsFloat(L"Max"), xmlReader->getAttributeValueAsInt(L"Digits"));
			}
			else if (irr::core::stringw("RadiusDecrement") == xmlReader->getNodeName())
			{
				SetParameterRandom(DunGen::ELSystemParameter::RADIUS_DECREMENT, xmlReader->getAttributeValueAsFloat(L"Min"),
					xmlReader->getAttributeValueAsFloat(L"Max"), xmlReader->getAttributeValueAsInt(L"Digits"));
			}
		}
	}

	// close file
	xmlReader->drop();
	file->drop();

	Apply();
	std::cout << "done." << std::endl;
}

void CEventReceiver::SaveCave()
{
	std::cout << "Saving Cave ... " << std::endl;
	time_t timeStamp;
	tm *timeInfo;
	timeStamp = time(NULL);
	timeInfo = localtime(&timeStamp);

	// convert time to string
	irr::io::path timeNamePart = irr::core::stringc(timeInfo->tm_year+1900) + "-";
	if (timeInfo->tm_mon < 9) timeNamePart += "0";
	timeNamePart += irr::core::stringc(timeInfo->tm_mon+1) + "-";
	if (timeInfo->tm_mday < 10) timeNamePart += "0";
	timeNamePart += irr::core::stringc(timeInfo->tm_mday) + "_";

	if (timeInfo->tm_hour < 10) timeNamePart += "0";
	timeNamePart += irr::core::stringc(timeInfo->tm_hour) + "-";
	if (timeInfo->tm_min < 10) timeNamePart += "0";
	timeNamePart += irr::core::stringc(timeInfo->tm_min) + "-";
	if (timeInfo->tm_sec < 10) timeNamePart += "0";
	timeNamePart += irr::core::stringc(timeInfo->tm_sec);

	// set filename and create file
	irr::io::path filename = "Cave_";
	filename += timeNamePart + ".xml";
	irr::io::IWriteFile* file = IrrDevice->getFileSystem()->createAndWriteFile(filename);
	if (!file)
	{
		std::cout << "failed." << std::endl;
		return;
	}

	// write content
	irr::io::IXMLWriter* xmlWriter = IrrDevice->getFileSystem()->createXMLWriter(file);
	xmlWriter->writeXMLHeader();
	xmlWriter->writeLineBreak();

	xmlWriter->writeElement(L"DunGen",false);
	xmlWriter->writeLineBreak();

	xmlWriter->writeElement(L"DrawVoxelCave",false,
		L"StartString",irr::core::stringw(Start.c_str()).c_str(),
		L"StartRadius",irr::core::stringw(RadiusStart).c_str(),
		L"Derivation",irr::core::stringw(Derivation).c_str());
	xmlWriter->writeLineBreak();

	xmlWriter->writeElement(L"Radius",true,
		L"RadiusFactor",irr::core::stringw(RadiusFactor).c_str(),
		L"RadiusDecrement",irr::core::stringw(RadiusDecrement).c_str());
	xmlWriter->writeLineBreak();

	xmlWriter->writeElement(L"Angle",true,
		L"Yaw",irr::core::stringw(AngleYaw).c_str(),
		L"Pitch",irr::core::stringw(AnglePitch).c_str(),
		L"Roll",irr::core::stringw(AngleRoll).c_str());
	xmlWriter->writeLineBreak();

	std::string tmpString = "0";
	for (unsigned int i=0; i<Rules.size(); ++i)
	{
		tmpString[0] = Rules[i].Symbol;
		xmlWriter->writeElement(L"Rule",true,
			L"Symbol",irr::core::stringw(tmpString.c_str()).c_str(),
			L"Substitution",irr::core::stringw(Rules[i].Replacement.c_str()).c_str()),
			xmlWriter->writeLineBreak();
	}

	xmlWriter->writeClosingTag(L"DrawVoxelCave");
	xmlWriter->writeLineBreak();

	xmlWriter->writeElement(L"GenerateMeshCave",true, L"NormalWeighting", L"0");
	xmlWriter->writeLineBreak();

	xmlWriter->writeClosingTag(L"DunGen");
	xmlWriter->writeLineBreak();

	// close file
	xmlWriter->drop();
	file->drop();

	std::cout << "done." << std::endl;
}
