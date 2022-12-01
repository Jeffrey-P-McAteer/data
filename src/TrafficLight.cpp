
#include <algorithm>

#include "TrafficLight.h"

std::string SignalTypes[3] = { // Cheap trick to get Signal as string w/o template mess
    "Green",
    "Yellow",
    "Red"
};

TrafficLight::TrafficLight(void)
{
}


TrafficLight::~TrafficLight(void)
{
}

void TrafficLight::setMaterials()
{
	// In this function, you are supposed to assign values to the variables redOn, redOff,
	// yellowOn, yellowOff, greenOn, greenOff.
  this->redOn = this->materials["_Red_"];
  this->redOn.d = 1.0;
  
  this->redOff = this->materials["_Red_"];
  this->redOff.d = -1.0;

  this->yellowOn = this->materials["_Yellow_"];
  this->yellowOn.d = 1.0;
  
  this->yellowOff = this->materials["_Yellow_"];
  this->yellowOff.d = -1.0;

  this->greenOn = this->materials["_Green_"];
  this->greenOn.d = 1.0;
  
  this->greenOff = this->materials["_Green_"];
  this->greenOff.d = -1.0;

  this->opaqueMaterials.erase(std::remove(this->opaqueMaterials.begin(), this->opaqueMaterials.end(), "_Red_"), this->opaqueMaterials.end());
  this->opaqueMaterials.erase(std::remove(this->opaqueMaterials.begin(), this->opaqueMaterials.end(), "_Yellow_"), this->opaqueMaterials.end());
  this->opaqueMaterials.erase(std::remove(this->opaqueMaterials.begin(), this->opaqueMaterials.end(), "_Green_"), this->opaqueMaterials.end());
  
  this->translucentMaterials.push_back("_Red_");
  this->translucentMaterials.push_back("_Yellow_");
  this->translucentMaterials.push_back("_Green_");

}

void TrafficLight::setSignal(Signal signal)
{	
  // You are supposed to assign the materials used in the ObjModel class based on
	// values of the input signal.
  switch (signal) {
    case Green:
      this->materials["_Red_"] = this->redOff;
      this->materials["_Yellow_"] = this->yellowOff;
      this->materials["_Green_"] = this->greenOn;
      break;
    case Yellow:
      this->materials["_Red_"] = this->redOff;
      this->materials["_Yellow_"] = this->yellowOn;
      this->materials["_Green_"] = this->greenOff;
      break;
    case Red:
      this->materials["_Red_"] = this->redOn;
      this->materials["_Yellow_"] = this->yellowOff;
      this->materials["_Green_"] = this->greenOff;
      break;
  }

}

void TrafficLight::ReadFile(string fileName)
{
	ObjModel::ReadFile(fileName);
	setMaterials();
}