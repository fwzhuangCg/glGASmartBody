#include "glgasmartbodylistener.h"
#include "glgasmartbody.h"

#include <iostream>
using namespace std;

glGASmartBodyListener::glGASmartBodyListener(glGASmartBody* glgasb) : SBSceneListener()
{
	glGASB = glgasb;
}

glGASmartBodyListener::~glGASmartBodyListener()
{
}


//ok found when it is being used
void glGASmartBodyListener::OnCharacterCreate(const std::string & name, const std::string & objectClass)
{
	
	std::cout << "I am inside the OncharacterCreate of glGASmartBody with string ObjectClass" << objectClass << " and string name " << name << std::endl;
	//start by initializing the RigMesh 
	// the first parameter is the glGASmartBody object and the second is the path of the where the dae file is
	glGASB->initMeshDynamic(objectClass);
	
	std::cout << "The rigged mesh along with the bones for the file in path " << objectClass << " should be initialized" << std::endl;

}



//--
void glGASmartBodyListener::OnCharacterDelete(const std::string & name)
{
	
}



//ok found sometimes when it being used
void glGASmartBodyListener::OnCharacterUpdate(const std::string& name)
{
	
	std::cout << "I am inside the OncharacterUpdate of glGASmartBody with name :" << name << std::endl;

}



//--
void glGASmartBodyListener::OnLogMessage(const std::string & message)
{
#ifdef WIN32
	LOG(message.c_str());
#endif
}




//ok it found when it being used
void glGASmartBodyListener::OnPawnCreate(const std::string & name){
	std::cout << "I am inside the OnPawnCreate of glGASmartBody with name: " << name << std::endl;
}



//--
void glGASmartBodyListener::OnPawnDelete(const std::string & name){
	std::cout << "I am inside the OnPawnDelete of glGASmartBody" << std::endl;

}


// ok found an occasion where at least one it is being used
void glGASmartBodyListener::OnViseme(const std::string & name, const std::string & visemeName, const float weight, const float blendTime){
	std::cout << "I am inside the OnViseme of glGASmartBody with name " << name << " visemeName "<< visemeName << " weight "  << weight << " and blendTime " << blendTime << std::endl;


}



void glGASmartBodyListener::OnChannel(const std::string & name, const std::string & channelName, const float value){
	std::cout << "I am inside the OnChannel of glGASmartBody" << std::endl;


}


//ok found an occasion where at least one time it is being used
void glGASmartBodyListener::OnEvent(const std::string & eventName, const std::string & eventParameters){
	std::cout << "I am inside the OnEvent of glGASmartBody with eventName being " << eventName << " and eventParameters being "<< eventParameters <<std::endl;


}



// ok found whene at least one occasion when used
void glGASmartBodyListener::OnObjectCreate(SmartBody::SBObject* object){
	std::cout << "I am inside the OnObjectCreate of glGASmartBody and the type of the object is " << object->getName() << std::endl;
	

//--	
}
void glGASmartBodyListener::OnObjectDelete(SmartBody::SBObject* object){
	std::cout << "I am inside the OnObjectDelete of glGASmartBody" << std::endl;


}

//ok found when found when it is being used
void glGASmartBodyListener::OnSimulationStart(){
	std::cout << "I am inside the OnSimulationStart of glGASmartBody" << std::endl;

}

//--
void glGASmartBodyListener::OnSimulationEnd(){
	std::cout << "I am inside the OnSimulationEnd of glGASmartBody" << std::endl;


}

// this is the function that makes is always running so inside it we could do the rendering
void glGASmartBodyListener::OnSimulationUpdate(){
	std::cout << "I am inside the OnSimulationUpdate of glGASmartBody" << std::endl;
	
	//if (glGASB != NULL)
	//	std::cout << "Den Einai NUll to glGASB" << std::endl;

	//std::cout << "Before glGASB" << std::endl;
	//glGASB->RenderRun(glGASB);
}


