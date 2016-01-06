#include "Plugin.h"

char* NPP_GetMIMEDescription() { return "application/save-plugin::save your file";}
NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved) {   
	if (!instance) return NPERR_INVALID_INSTANCE_ERROR;
	// create a new plugin instance object
	// initialization will be done when the associated window is ready
	nsPluginCreateData ds;
	ds.instance = instance;
	ds.type     = pluginType; 
	ds.mode     = mode; 
	ds.argc     = argc; 
	ds.argn     = argn; 
	ds.argv     = argv; 
	ds.saved    = saved;
	Plugin * plugin = (Plugin*)NS_NewPluginInstance(&ds);
	if (!plugin) return NPERR_OUT_OF_MEMORY_ERROR;
	// associate the plugin instance object with NPP instance
	instance->pdata = (void *)plugin;
	return NPERR_NO_ERROR;
}
// here is the place to clean up and destroy the nsPluginInstance object
NPError NPP_Destroy (NPP instance, NPSavedData** save) {
	if (!instance) return NPERR_INVALID_INSTANCE_ERROR;
	Plugin * plugin = (Plugin *)instance->pdata;
	if (plugin) {
		plugin->shut();
		NS_DestroyPluginInstance(plugin);
	}
	return NPERR_NO_ERROR;
}
// during this call we know when the plugin window is ready or
// is about to be destroyed so we can do some gui specific
// initialization and shutdown
NPError NPP_SetWindow (NPP instance, NPWindow* pNPWindow) {    
	if (!instance) return NPERR_INVALID_INSTANCE_ERROR;
	if (!pNPWindow) return NPERR_GENERIC_ERROR;
	Plugin * plugin = (Plugin *)instance->pdata;
	if (!plugin) return NPERR_GENERIC_ERROR;
	// window just created
	if (!plugin->isInitialized() && pNPWindow->window) { 
		if (!plugin->init(pNPWindow)) {
			NS_DestroyPluginInstance(plugin);
			return NPERR_MODULE_LOAD_FAILED_ERROR;
		}
	}
	// window goes away
	if (!pNPWindow->window && plugin->isInitialized()) return plugin->SetWindow(pNPWindow);
	// window resized?
	if (pNPWindow->window && plugin->isInitialized()) return plugin->SetWindow(pNPWindow);
	// this should not happen, nothing to do
	if (!pNPWindow->window && !plugin->isInitialized()) return plugin->SetWindow(pNPWindow);
	return NPERR_NO_ERROR;
}
NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype) {
	if (!instance) return NPERR_INVALID_INSTANCE_ERROR;
	Plugin * plugin = (Plugin *)instance->pdata;
	if (!plugin) return NPERR_GENERIC_ERROR;
	return plugin->NewStream(type, stream, seekable, stype);
}
int32_t NPP_WriteReady (NPP instance, NPStream *stream) {
	if(instance == NULL) return NPERR_INVALID_INSTANCE_ERROR;
	Plugin * plugin = (Plugin *)instance->pdata;
	if (!plugin) return 0x0fffffff;
	return plugin->WriteReady(stream);
}
int32_t NPP_Write (NPP instance, NPStream *stream, int32_t offset, int32_t len, void *buffer) {   
	if (!instance) return NPERR_INVALID_INSTANCE_ERROR;
	Plugin * plugin = (Plugin *)instance->pdata;
	if (!plugin) return len;
	return plugin->Write(stream, offset, len, buffer);
}
NPError NPP_DestroyStream (NPP instance, NPStream *stream, NPError reason) {
	if (!instance) return NPERR_INVALID_INSTANCE_ERROR;
	Plugin * plugin = (Plugin *)instance->pdata;
	if (!plugin) return NPERR_GENERIC_ERROR;
	return plugin->DestroyStream(stream, reason);
}
void NPP_StreamAsFile (NPP instance, NPStream* stream, const char* fname) {
	if (!instance) return;
	Plugin * plugin = (Plugin *)instance->pdata;
	if (!plugin) return;
	plugin->StreamAsFile(stream, fname);
}
void NPP_Print (NPP instance, NPPrint* printInfo){
	if (!instance) return;
	Plugin * plugin = (Plugin *)instance->pdata;
	if (!plugin) return;
	plugin->Print(printInfo);
}
void NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData) {
	if (!instance) return;
	Plugin * plugin = (Plugin *)instance->pdata;
	if (!plugin) return;
	plugin->URLNotify(url, reason, notifyData);
}
NPError	NPP_GetValue(NPP instance, NPPVariable variable, void *value) {
	if (!instance) return NPERR_INVALID_INSTANCE_ERROR;
	Plugin * plugin = (Plugin *)instance->pdata;
	if (!plugin) return NPERR_GENERIC_ERROR;
	NPError rv = NPERR_NO_ERROR;
	switch(variable){
		case NPPVpluginNameString:
			*((char **)value) = "NPRuntimeTest";
			break;
		case NPPVpluginDescriptionString:
			*((char **)value) = "NPRuntime scriptability API test plugin";
			break;
		case NPPVpluginScriptableNPObject:
			*(NPObject **)value = plugin->GetScriptableObject();
			break;
		default:
			rv = NPERR_GENERIC_ERROR;
	}
	return rv;
}
NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value) {
	if (!instance) return NPERR_INVALID_INSTANCE_ERROR;
	Plugin * plugin = (Plugin *)instance->pdata;
	if (!plugin) return NPERR_GENERIC_ERROR;
	return plugin->SetValue(variable, value);
}
int16_t	NPP_HandleEvent(NPP instance, void* event) {
	if (!instance) return 0;
	Plugin * plugin = (Plugin *)instance->pdata;
	if (!plugin) return 0;
	return plugin->HandleEvent(event);
}
NPObject *NPP_GetScriptableInstance(NPP instance) {
	if(!instance) return 0;
	NPObject *npobj = 0;
	Plugin * pPlugin = (Plugin *)instance->pdata;
	if (!pPlugin) npobj = pPlugin->GetScriptableObject();
	return npobj;
}