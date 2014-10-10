#ifndef _GLGASMARTBODYLISTENER_H_
#define _GKGASMARTBODYLISTENER_H_

#include <glGA/glGAHelper.h>
#include <glGA/glGAMesh.h>
#include <glGA/glGARigMesh.h>
#include <glGA/PlatformWrapper.h>

#include <glGAMath/common.h>
#include <glGAMath/e3ga.h>
#include <glGAMath/e3ga_util.h>
#include <glGAMath/gabits.h>
#include <glGAMath/glGAMath.h>
#include <glGAMath/mvtypebase.h>



#include "glGAsmartbody.h"
#include <sb/SBSceneListener.h>

class glGASmartBodyListener : public SmartBody::SBSceneListener
{
public:
	glGASmartBodyListener(glGASmartBody* glGAsb);
	~glGASmartBodyListener();

	/*Call the initRigMesh inside OnCharacterCreate*/
	virtual void OnCharacterCreate(const std::string & name, const std::string & objectClass);
	virtual void OnCharacterDelete(const std::string & name);
	virtual void OnCharacterUpdate(const std::string& name);
	virtual void OnLogMessage(const std::string & message);

	//for this this perhaps also call onpawncreate
	virtual  void OnPawnCreate(const std::string & name); 
	virtual  void OnPawnDelete(const std::string & name); 

	virtual  void OnViseme(const std::string & name, const std::string & visemeName, const float weight, const float blendTime); 
	virtual  void OnChannel(const std::string & name, const std::string & channelName, const float value); 

	virtual  void OnEvent(const std::string & eventName, const std::string & eventParameters); 

	virtual  void OnObjectCreate(SmartBody::SBObject* object); 
	virtual  void OnObjectDelete(SmartBody::SBObject* object); 

	//start the simulation for the mesh
	virtual  void OnSimulationStart(); 
	virtual  void OnSimulationEnd();
	//update the simulation by the meshDynamicDisplay
	virtual  void OnSimulationUpdate(); 


private:
	glGASmartBody* glGASB;
};

#endif