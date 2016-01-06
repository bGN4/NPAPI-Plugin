#pragma once
#include "pluginbase.h"
class Plugin : public nsPluginInstanceBase {
private:
    NPP m_pNPInstance;
	NPBool m_bInitialized;
	NPObject *m_pScriptableObject;
public:
	Plugin(NPP pNPInstance);
	~Plugin();
	NPBool init(NPWindow* pNPWindow);
	void shut();
	NPBool isInitialized();
	NPObject *GetScriptableObject();
};