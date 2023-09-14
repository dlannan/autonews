#ifndef _SYM_WEBVIEW_H_
#define _SYM_WEBVIEW_H_

#include <dmsdk/dlib/log.h>

// Defold SDK
#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_LINUX)
#define WEBVIEW_SUPPORTED

extern "C" 
{
	typedef void *webview_t;
	typedef void *webview_version_info_t;

	extern webview_t (*sym_webview_create_ex)(int debug, void *window, int style, int exstyle);
	extern webview_t (*sym_webview_create)(int debug, void *window);
	extern void (*sym_webview_destroy)(webview_t w);
	extern int (*sym_webview_poll)(webview_t w);
	extern void (*sym_webview_resize)(webview_t w);
	extern void (*sym_set_virtual_hostname)(webview_t w, const char *hostname, const char *datapath);
	extern void (*sym_add_file_handling)( webview_t w);	
	extern void (*sym_webview_terminate)(webview_t w);
	extern void (*sym_webview_dispatch)(webview_t w, void (*fn)(webview_t w, void *arg), void *arg);
	extern void *(*sym_webview_get_window)(webview_t w);
	extern void (*sym_webview_set_title)(webview_t w, const char *title);
	extern void (*sym_webview_set_background_color)(webview_t w, unsigned int color, int transparent);

	// Window size hints
	#define WEBVIEW_HINT_NONE 0  // Width and height are default size
	#define WEBVIEW_HINT_MIN 1   // Width and height are minimum bounds
	#define WEBVIEW_HINT_MAX 2   // Width and height are maximum bounds
	#define WEBVIEW_HINT_FIXED 3 // Window size can not be changed by a user
	// Updates native window size. See WEBVIEW_HINT constants.
	extern void (*sym_webview_set_size)(webview_t w, int width, int height, int hints);
	extern void (*sym_webview_set_pos)(webview_t w, int left, int top);	
	extern void (*sym_webview_navigate)(webview_t w, const char *url);
	extern void (*sym_webview_set_html)(webview_t w, const char *html);
	extern void (*sym_webview_init)(webview_t w, const char *js);
	extern void (*sym_webview_eval)(webview_t w, const char *js);
	extern void (*sym_webview_bind)(webview_t w, const char *name,
		void (*fn)(const char *seq, const char *req,
			void *arg),
			void *arg);	
	extern void (*sym_webview_unbind)(webview_t w, const char *name);	
	extern void (*sym_webview_return)(webview_t w, const char *seq, int status,
		const char *result);	
	extern const webview_version_info_t *(*sym_webview_version)();		
}

void Webview_openLibrary(dmConfigFile::HConfig appConfig);
void Webview_closeLibrary();

#endif


#endif
