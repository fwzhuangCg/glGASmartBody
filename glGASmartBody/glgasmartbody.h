#ifndef __glGASmartBody_h_
#define __glGASmartBody_h_


// smartbody
#ifndef NDEBUG
#define NDEBUG
#endif

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

#include <sb/SBScene.h>
#include <sb/SBBmlProcessor.h>
#include <sb/SBEvent.h>
#include <sb/SBJointMap.h>
#include <sb/SBJointMapManager.h>

typedef glm::vec4 point4;

class glGASmartBody
{
public:
	glGASmartBody(void);
	virtual ~glGASmartBody(void);
	//bool run(void);
	//functions
	//float           calcRunningTime(glGASmartBody* glgasb);
	//double			calcFPS(double theTimeInterval, std::string theWindowTitle);
	//void			RenderRun(glGASmartBody * glGAe);

	void initMeshDynamic(const std::string & objectClass);
	//void     displayMeshDynamic(glGASmartBody* glgasb);
	//static GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

	// smartbody
	 SmartBody::SBScene* m_pScene;
};

#endif // #ifndef __glGASmartBody_h_