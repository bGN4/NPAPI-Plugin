#include "Plugin.h"
#include <stdio.h>
FILE *in;
static NPIdentifier search;
static NPIdentifier writeFile;
class ScriptablePluginObjectBase : public NPObject {
protected:
	NPP mNpp;
public:
	ScriptablePluginObjectBase(NPP npp) : mNpp(npp) {}
	static void Deallocate(NPObject *npobj) {delete (ScriptablePluginObjectBase *)npobj;}
	static void Invalidate(NPObject *npobj) {/*((ScriptablePluginObjectBase *)npobj)->Invalidate(npobj);*/}
	static bool HasMethod(NPObject *npobj, NPIdentifier name) {return (name == writeFile || name == search);}
	static bool Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result) {
		if(name == writeFile) {
			if(args != NULL && argCount > 1){
				if((in=fopen("R:\\temp.txt","a+"))!=NULL){
					NPString npstr = NPVARIANT_TO_STRING(args[1]);
					for(unsigned i=0;i<npstr.UTF8Length;i++){
						if(EOF == fputc(npstr.UTF8Characters[i],in)) return false;
					}
					//fputs(NPVARIANT_TO_STRING(args[1]).UTF8Characters,in);
					//fprintf(in,"%s\n",NPVARIANT_TO_STRING(args[1]).UTF8Characters);
					fclose(in);
					return true;
				}
			}
		}
		else if (name == search) {
			return true;
		}
		return false;
	}
	static bool InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result) {return true;}
	static bool HasProperty(NPObject *npobj, NPIdentifier name) {return false;}
	static bool GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result) {return false;}
	static bool SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value) {return false;}
	static bool RemoveProperty(NPObject *npobj, NPIdentifier name) {return false;}
	static bool Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count) {return false;}
	static bool Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result) {return false;}
};
static NPObject* AllocateScriptablePluginObject(NPP npp, NPClass *aClass) {return new ScriptablePluginObjectBase(npp);}
#define DECLARE_NPOBJECT_CLASS_WITH_BASE(_class, ctor)                                              \
static NPClass s##_class##_NPClass = {                                                              \
	NP_CLASS_STRUCT_VERSION_CTOR,     ctor,   ScriptablePluginObjectBase::Deallocate,               \
	ScriptablePluginObjectBase::Invalidate,   ScriptablePluginObjectBase::HasMethod,                \
	ScriptablePluginObjectBase::Invoke,       ScriptablePluginObjectBase::InvokeDefault,            \
	ScriptablePluginObjectBase::HasProperty,  ScriptablePluginObjectBase::GetProperty,              \
	ScriptablePluginObjectBase::SetProperty,  ScriptablePluginObjectBase::RemoveProperty,           \
	ScriptablePluginObjectBase::Enumerate,    ScriptablePluginObjectBase::Construct                 \
};
#define GET_NPOBJECT_CLASS(_class) &s##_class##_NPClass
DECLARE_NPOBJECT_CLASS_WITH_BASE(ScriptablePluginObjectBase, AllocateScriptablePluginObject);
NPError NS_PluginInitialize() {
    return NPERR_NO_ERROR;
}
void NS_PluginShutdown() {}
nsPluginInstanceBase* NS_NewPluginInstance(nsPluginCreateData *aCreateDataStruct) {
    if(!aCreateDataStruct) return NULL;
    Plugin *plugin = new Plugin(aCreateDataStruct->instance);
    return plugin;
}
void NS_DestroyPluginInstance(nsPluginInstanceBase * aPlugin) {
    if(aPlugin) delete (Plugin *)aPlugin;
}
Plugin::Plugin(NPP pNPInstance) : m_pNPInstance(pNPInstance),m_bInitialized(FALSE),m_pScriptableObject(NULL) {
	writeFile = NPN_GetStringIdentifier("writeFile");
	search = NPN_GetStringIdentifier("search");
	NPObject *m_pScriptableObject = NPN_CreateObject(m_pNPInstance, GET_NPOBJECT_CLASS(ScriptablePluginObjectBase));
}
Plugin::~Plugin(){
	if (m_pScriptableObject) NPN_ReleaseObject(m_pScriptableObject);
}
NPBool Plugin::init(NPWindow* pNPWindow) {
	m_bInitialized = TRUE;
	return TRUE;
}
void Plugin::shut() {
	m_bInitialized = FALSE;
}
NPBool Plugin::isInitialized() {
	return m_bInitialized;
}
NPObject* Plugin::GetScriptableObject() {
	if (!m_pScriptableObject) {
		m_pScriptableObject = NPN_CreateObject(m_pNPInstance, GET_NPOBJECT_CLASS(ScriptablePluginObjectBase));
	}
	if (m_pScriptableObject) {
		NPN_RetainObject(m_pScriptableObject);
	}
	return m_pScriptableObject;
}