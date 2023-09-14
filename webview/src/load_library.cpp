#if !defined(DM_PLATFORM_ANDROID) && !defined(DM_PLATFORM_IOS) && !defined(DM_PLATFORM_OSX)

// Extension lib defines
#define EXTENSION_NAME WebViewExternal
#define LIB_NAME "WebView"
#define MODULE_NAME "webview"
#ifndef DLIB_LOG_DOMAIN
#define DLIB_LOG_DOMAIN LIB_NAME
#endif
#include <dmsdk/dlib/log.h>

// Defold SDK
#include <dmsdk/sdk.h>

#if !defined(DM_PLATFORM_WINDOWS)
#include <dlfcn.h>
#include <libgen.h>
#else
#include <Shlwapi.h>
#endif

#include <string.h>
#include <stdlib.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
#ifdef __linux__
#include <unistd.h>
#endif

#if !defined(DM_PLATFORM_WINDOWS)
#include <dlfcn.h>
#define dlModuleT void *
#else
#define dlModuleT HMODULE
#endif

#define STRINGIFY(x) #x
#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)

#if defined(DM_PLATFORM_WINDOWS)
#define getSymbol GetProcAddress
#define getSymbolPrintError(fname) dmLogError("GetProcAddress(\"%s\"): %lu", STRINGIFY(fname), GetLastError())
#else
#define getSymbol dlsym
#define getSymbolPrintError(fname) dmLogError("dlsym(\"%s\"): %s", STRINGIFY(fname), dlerror())
#endif

static dlModuleT Webview_dllHandle = NULL;
static dlModuleT WebviewLoader_dllHandle = NULL;

extern "C" {

typedef void *webview_t;
typedef void *webview_version_info_t;

webview_t (*sym_webview_create_ex)(int , void *, int , int ) = NULL;
webview_t (*sym_webview_create)(int debug, void *window) = NULL;
void (*sym_webview_destroy)(webview_t w) = NULL;
int (*sym_webview_poll)(webview_t w) = NULL;
void (*sym_webview_resize)(webview_t w) = NULL;
void (*sym_set_virtual_hostname)(webview_t w, const char *hostname, const char *datapath) = NULL;
void (*sym_add_file_handling)( webview_t w) = NULL;	
void (*sym_webview_terminate)(webview_t w) = NULL;
void (*sym_webview_dispatch)(webview_t w, void (*fn)(webview_t w, void *arg), void *arg) = NULL;
void *(*sym_webview_get_window)(webview_t w) = NULL;
void (*sym_webview_set_title)(webview_t w, const char *title) = NULL;
void (*sym_webview_set_background_color)(webview_t w, unsigned int color, int transparent) = NULL;
void (*sym_webview_set_size)(webview_t w, int width, int height, int hints) = NULL;
void (*sym_webview_set_pos)(webview_t w, int left, int top) = NULL;	
void (*sym_webview_navigate)(webview_t w, const char *url) = NULL;
void (*sym_webview_set_html)(webview_t w, const char *html) = NULL;
void (*sym_webview_init)(webview_t w, const char *js) = NULL;
void (*sym_webview_eval)(webview_t w, const char *js) = NULL;
void (*sym_webview_bind)(webview_t w, const char *name,
		void (*fn)(const char *seq, const char *req, void *arg),
		void *arg) = NULL;	
void (*sym_webview_unbind)(webview_t w, const char *name) = NULL;	
void (*sym_webview_return)(webview_t w, const char *seq, int status, const char *result) = NULL;	
const webview_version_info_t *(*sym_webview_version)() = NULL;	

}


#if defined(DM_PLATFORM_WINDOWS)
#define SEP "\\"
#define SEPCH '\\'
#else
#define SEP "/"
#define SEPCH '/'
#endif

#if defined(DM_PLATFORM_WINDOWS)
static char * dirname(char * path) {
	size_t i = strlen(path);
	do {
		i -= 1;
		if (path[i] == SEPCH) {
			path[i] = 0;
			break;
		}
	} while (i);
	return path;
}
#endif

static bool endsIn(const char * haystack, const char * needle) {
	size_t needleLen = strlen(needle);
	size_t haystackLen = strlen(haystack);
	return (haystackLen >= needleLen && 0 == strcmp(needle, haystack + haystackLen - needleLen));
}

void Webview_openLibrary(dmConfigFile::HConfig appConfig)
{
	dmLogInfo("Calling OpenLibrary");
	if (Webview_dllHandle) { return; }

	char *exePath = NULL;
	const char *libPath = ".";
	bool mustFreeLibPath = false;

	{
		#ifdef __APPLE__
		uint32_t bufsize = 0;
		_NSGetExecutablePath(NULL, &bufsize);
		exePath = new char[bufsize];
		_NSGetExecutablePath(exePath, &bufsize);
		libPath = dirname(exePath);
		char* newPath = new char[strlen(libPath) + 1];
		strcpy(newPath, libPath);
		libPath = newPath;
		mustFreeLibPath = true;

		#elif defined(__linux__)
		exePath = new char[PATH_MAX + 2];
		ssize_t ret = readlink("/proc/self/exe", exePath, PATH_MAX + 2);
		if (ret >= 0 && ret <= PATH_MAX + 1) 
		{
			exePath[ret] = 0;
			char* newPath = new char[ret + 1];
			strcpy(newPath, exePath);
			libPath = dirname(newPath); // dirname() clobbers newPath
			char* finalPath = new char[ret + 1];
			strcpy(finalPath, libPath);
			libPath = finalPath;
			mustFreeLibPath = true;
			delete[] newPath;
		} 
		else 
		{
			exePath[0] = 0;
		}

		#elif defined(DM_PLATFORM_WINDOWS)
		exePath = new char[MAX_PATH];
		size_t ret = GetModuleFileNameA(GetModuleHandle(NULL), exePath, MAX_PATH);
		if (ret > 0 && ret < MAX_PATH) 
		{
			char* newPath = new char[MAX_PATH];
			strcpy(newPath, exePath);
			dirname(newPath);
			libPath = newPath;
			mustFreeLibPath = true;
		} 
		else 
		{
			exePath[0] = 0;
		}
		#endif

		// Detect if the game is running in the editor

		#if defined(__APPLE__)
		#define FMB_PLATFORM "osx"
		#define FMB_PLATFORM_ALT "darwin"
		#define FMB_EXT ""
		#elif defined(__linux__)
		#define FMB_PLATFORM "linux"
		#define FMB_EXT ""
		#elif defined(DM_PLATFORM_WINDOWS)
		#define FMB_PLATFORM "win32"
		#define FMB_EXT ".exe"
		#endif

		#if defined(__x86_64__) || defined(_M_X64)
		#define FMB_ARCH "x86_64"
		#elif defined(__i386) || defined(_M_IX86)
		#define FMB_ARCH "x86"
		#endif

		#if defined(FMB_PLATFORM) && defined(FMB_ARCH)
		#define FMB_EDITOR_SUFFIX SEP "build" SEP FMB_ARCH "-" FMB_PLATFORM SEP "dmengine" FMB_EXT

		#ifdef FMB_PLATFORM_ALT
		#define FMB_EDITOR_SUFFIX_ALT SEP "build" SEP FMB_ARCH "-" FMB_PLATFORM_ALT SEP "dmengine" FMB_EXT
		if (endsIn(exePath, FMB_EDITOR_SUFFIX) || endsIn(exePath, FMB_EDITOR_SUFFIX_ALT)) 
		{
		#else
		if (endsIn(exePath, FMB_EDITOR_SUFFIX)) 
		{
		#endif
			dmLogInfo("Running in the editor. Will attempt to load libraries from project");

			const char* projPath = dirname(dirname(dirname(exePath)));
			const char* resPath = dmConfigFile::GetString(appConfig, "webview.lib_path", "");

			if (!resPath[0]) 
			{
				dmLogWarning("webview.lib_path not found in game.project. See README for details");
			}

			#ifdef __APPLE__
			#define FMB_LIB_PATH SEP FMB_ARCH "-" FMB_PLATFORM SEP "Contents" SEP "MacOS"
			#else
			#define FMB_LIB_PATH SEP FMB_ARCH "-" FMB_PLATFORM
			#endif

			size_t projPathLen = strlen(projPath);
			size_t resPathLen = strlen(resPath);
			size_t libPathLen = strlen(FMB_LIB_PATH);
			size_t len = 0;
			char* newPath = new char[projPathLen + 1 + resPathLen + libPathLen + 1];

			strcpy(newPath, projPath);
			len += projPathLen;

			if (resPath[0] != '/') 
			{
				strcat(newPath, SEP);
				len += 1;
			}

			strcat(newPath + len, resPath);
			#if defined(DM_PLATFORM_WINDOWS)
			for (size_t i = len; i < len + resPathLen; i++) 
			{
				if (newPath[i] == '/') { newPath[i] = SEPCH; }
			}
			#endif
			len += resPathLen;

			strcat(newPath + len, FMB_LIB_PATH);

			if (mustFreeLibPath) { delete[] libPath; }
			libPath = newPath;
			mustFreeLibPath = true;
		}
		#endif
	}
	#if defined(__APPLE__)
	#define LIBEXT "dylib"
	#elif defined(DM_PLATFORM_WINDOWS)
	#define LIBEXT "dll"
	#else
	#define LIBEXT "so"
	#endif

	#if defined(DM_PLATFORM_WINDOWS)
	#define LIBPREFIX ""
	#define libOpen(var, path) \
	var = LoadLibraryA(path); \
	if (!var) { dmLogWarning("LoadLibrary(\"%s\") failed with error code %lu", path, GetLastError()); }
	#else
	#define LIBPREFIX "lib"
	#define libOpen(var, path) \
	var = dlopen(path, RTLD_NOW | RTLD_GLOBAL); \
	if (!var) { dmLogWarning("%s", dlerror()); }
	#endif

	if (exePath) { delete[] exePath; }
	size_t maxPathLen = strlen(libPath) + 20;
	exePath = new char[maxPathLen + 1];

	strcpy(exePath, libPath);
	strncat(exePath, SEP LIBPREFIX "WebView2Loader." LIBEXT, maxPathLen);
	libOpen(WebviewLoader_dllHandle, exePath);

	strcpy(exePath, libPath);
	strncat(exePath, SEP LIBPREFIX "webview." LIBEXT, maxPathLen);
	libOpen(Webview_dllHandle, exePath);
	
	if (mustFreeLibPath) { delete[] libPath; }
	delete[] exePath;

	#define ensure(fname, retType, ...) \
	CONCAT(sym_, fname) = (retType (*)(__VA_ARGS__))getSymbol(Webview_dllHandle, STRINGIFY(fname)); \
	if (!CONCAT(sym_, fname)) { \
		getSymbolPrintError(fname); \
	}

	if (Webview_dllHandle) 
	{
		ensure(webview_create_ex, webview_t, int , void *, int, int);
		ensure(webview_create, webview_t, int , void *);
		ensure(webview_destroy, void, void *);
		ensure(webview_poll,int , webview_t);
		ensure(webview_resize, void, webview_t);
		ensure(set_virtual_hostname, void, webview_t, const char *, const char *);
		ensure(add_file_handling, void, webview_t);	
		ensure(webview_terminate, void, webview_t);
		ensure(webview_dispatch, void, webview_t, void (*fn)(webview_t w, void *arg), void *);
		ensure(webview_get_window, void *, webview_t);
		ensure(webview_set_title,void, webview_t, const char *);
		ensure(webview_set_background_color, void, webview_t, unsigned int, int);
		ensure(webview_set_size, void, webview_t, int, int, int);
		ensure(webview_set_pos, void, webview_t, int, int);	
		ensure(webview_navigate, void, webview_t, const char *);
		ensure(webview_set_html, void, webview_t, const char *);
		ensure(webview_init, void, webview_t, const char *);
		ensure(webview_eval, void, webview_t, const char *);
		ensure(webview_bind, void, webview_t, const char *,
				void (*fn)(const char *seq, const char *req, void *arg),
				void *);	
		ensure(webview_unbind, void, webview_t, const char *);	
		ensure(webview_return, void, webview_t, const char *, int, const char *);	
		ensure(webview_version, const webview_version_info_t *);
	}

	if (WebviewLoader_dllHandle) 
	{
	}
}

void Webview_closeLibrary()
{
	sym_webview_create_ex = NULL;
	sym_webview_create = NULL;
	sym_webview_destroy = NULL;
	sym_webview_poll = NULL;
	sym_webview_resize = NULL;
	sym_set_virtual_hostname = NULL;
	sym_add_file_handling = NULL;	
	sym_webview_terminate = NULL;
	sym_webview_get_window = NULL;
	sym_webview_set_title = NULL;
	sym_webview_set_background_color = NULL;
	sym_webview_set_size = NULL;
	sym_webview_set_pos = NULL;	
	sym_webview_navigate = NULL;
	sym_webview_set_html = NULL;
	sym_webview_init = NULL;
	sym_webview_eval = NULL;
	sym_webview_bind = NULL;	
	sym_webview_unbind = NULL;	
	sym_webview_return = NULL;	
	sym_webview_version = NULL;	

	#if defined(DM_PLATFORM_WINDOWS)
	if (Webview_dllHandle) { FreeLibrary(Webview_dllHandle); }
	#else
	if (Webview_dllHandle) { dlclose(Webview_dllHandle); }
	#endif
}

#endif
