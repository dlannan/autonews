#if !defined(DM_PLATFORM_ANDROID) && !defined(DM_PLATFORM_IOS) && !defined(DM_PLATFORM_OSX)

#include <stdio.h>
#include <stdlib.h>
#include <sym_webview.h>

#include <string>
#include <map>

namespace dmWebView
{
//	static const int MAX_NUM_WEBVIEWS = 4;

	enum CallbackResult
	{
		CALLBACK_RESULT_URL_OK = 0,
		CALLBACK_RESULT_URL_ERROR = -1,
		CALLBACK_RESULT_EVAL_OK = 1,
		CALLBACK_RESULT_EVAL_ERROR = -2,
		CALLBACK_RESULT_URL_LOADING = -3,
	};


	static int GetDesktopSize(lua_State *L)
	{
		int width  = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);
		lua_pushnumber(L, width);
		lua_pushnumber(L, height);
		return 2;
	}

	static int SetDesktopPosition(lua_State* L)
	{
		const int x = luaL_checknumber(L, 1);
		const int y = luaL_checknumber(L, 2);
		const int w = luaL_checknumber(L, 3);
		const int h = luaL_checknumber(L, 4);
		HWND hwnd = dmGraphics::GetNativeWindowsHWND();

		SetWindowPos(hwnd, HWND_TOP, x, y, w, h, 0);
		return 0;
	}

	static int SetDesktopMode(lua_State* L)
	{
		const int x = luaL_checknumber(L, 1);
		HWND hwnd = dmGraphics::GetNativeWindowsHWND();
		ShowWindow(hwnd, SW_MAXIMIZE);
		return 0;
	}
	
	/** Creates a web view
	@param callback callback to use for requests
	@return the web view id
	*/
	static int Create(lua_State* L)
	{
		int debug = luaL_checknumber(L,1);
		HWND hwnd = dmGraphics::GetNativeWindowsHWND();
		webview_t wvobj = (webview_t)sym_webview_create_ex(debug, &hwnd, WS_OVERLAPPED, WS_EX_LAYERED);
		lua_pushlightuserdata(L, wvobj);
		return 1;
	}

	static int SetBackgroundColor(lua_State* L)
	{
		webview_t wvobj = (webview_t)lua_touserdata(L, 1);
		int color = luaL_checknumber(L,2);
		sym_webview_set_background_color( wvobj, color, 1 );
		return 0;
	}
	
	/** Closes a web view
	@param id the web view id
	@return -1 if an error occurred. 0 if it was destroyed
	*/
	static int Destroy(lua_State* L)
	{
		webview_t wvobj = (webview_t)lua_touserdata(L, 1);
		sym_webview_destroy(wvobj);
		return 0;
	}

	/** Opens an url in the view
	@param url url
	@return the request id
	*/
	static int Open(lua_State* L)
	{
		webview_t wvobj = (webview_t)lua_touserdata(L, 1);
		const char* url = luaL_checkstring(L, 2);

		sym_webview_navigate(wvobj, url);
		return 0;
	}

	static int OpenRaw(lua_State* L)
	{
		webview_t wvobj = (webview_t)lua_touserdata(L, 1);
		const char* url = luaL_checkstring(L, 2);

		sym_webview_set_html(wvobj, url);
		return 0;
	}

	static int Eval(lua_State* L)
	{
		webview_t wvobj = (webview_t)lua_touserdata(L, 1);
		const char* code = luaL_checkstring(L, 2);

		sym_webview_eval(wvobj, code);
		return 0;
	}

	static int SetVisible(lua_State* L)
	{
		webview_t wvobj = (webview_t)lua_touserdata(L, 1);
		const int visible = luaL_checknumber(L, 2);

		HWND hwnd = (HWND)sym_webview_get_window(wvobj);
		if(visible == 0)
			ShowWindow(hwnd, 0);
		else
			ShowWindow(hwnd, 5);
		return 0;
	}

	static int IsVisible(lua_State* L)
	{
		webview_t wvobj = (webview_t)lua_touserdata(L, 1);

		HWND hwnd = (HWND)sym_webview_get_window(wvobj);
		int visible = IsWindowVisible(hwnd)?1:0;
		lua_pushnumber(L, visible);
		return 1;
	}

	static int SetPosition(lua_State* L)
	{
		webview_t wvobj = (webview_t)lua_touserdata(L, 1);
		const int x = luaL_checknumber(L, 2);
		const int y = luaL_checknumber(L, 3);
		const int width = luaL_checknumber(L, 4);
		const int height = luaL_checknumber(L, 5);

		if(x >= 0 && y >= 0) sym_webview_set_pos(wvobj, x, y);
		sym_webview_set_size(wvobj, width, height, 0);
		return 0;
	}

	static int AddFileHandling(lua_State *L)
	{
		webview_t wvobj = (webview_t)lua_touserdata(L, 1);
		sym_add_file_handling(wvobj);
		return 0;
	}

	static int Poll(lua_State* L)
	{
		webview_t wvobj = (webview_t)lua_touserdata(L, 1);
		sym_webview_poll(wvobj);
		return 0;
	}

	// This handles the bind call from JS - seq, req and arg are populate from JS
	typedef struct _callerInfo_t { 
		int event;
		lua_State *lstate; 
		std::string	name;
	} callerInfo_t;
		
	static std::map<std::string, callerInfo_t *> 	registeredFuncs;
	
	static void BindFunctionCaller( const char *seq, const char *req, void *arg)
	{
		callerInfo_t *cinfo = (callerInfo_t *)arg;
		lua_State *L = (lua_State *)cinfo->lstate;
		lua_rawgeti( L, LUA_REGISTRYINDEX, cinfo->event );
		lua_pushstring(L, seq);
		lua_pushstring(L, req);
		if ( 0 != lua_pcall( L, 2, 0, 0 ) ) {
			printf("Failed to call the callback!\n %s\n", lua_tostring(L, -1));
			return;
		}
		//data->event = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	
	static int Bind(lua_State* L)
	{
		webview_t wvobj = (webview_t)lua_touserdata(L, 1);
		const char * name = luaL_checkstring(L, 2);
		if(!lua_isfunction(L, -1)) { 
			printf("[Webview] Function not in correct pos on stack?\n"); 
		}
		int event = luaL_ref(L, LUA_REGISTRYINDEX);
		auto iter = registeredFuncs.find(name);
		if (iter == registeredFuncs.end()) 
		{
			callerInfo_t *cinfo = new callerInfo_t{ event, L, name };
			registeredFuncs.insert({std::string(name), cinfo});
			
			sym_webview_bind(wvobj, name, BindFunctionCaller, (void *)cinfo);
		} else { 
			printf("[Webview] Function cant be bound to %s\n", name); 
		}
		return 0;
	}

	static int Result(lua_State* L)
	{
		webview_t wvobj = (webview_t)lua_touserdata(L, 1);
		const char * seq = luaL_checkstring(L, 2);
		int status = luaL_checknumber(L,3);
		const char * results = luaL_checkstring(L, 4);
		sym_webview_return(wvobj, seq, status, results);
		return 0;
	}
	
	static const luaL_reg WebView_methods[] =
	{
		{"create", Create},
		{"destroy", Destroy},
		{"open", Open},
		{"open_raw", OpenRaw},
		{"eval", Eval},
		{"set_visible", SetVisible},
		{"set_position", SetPosition},
		{"add_filehandling", AddFileHandling},
		{"set_background_color", SetBackgroundColor},
		{"is_visible", IsVisible},
		{"poll", Poll},
		{"bind", Bind},
		{"result", Result},
		// {"get_desktop_size", GetDesktopSize},
		// {"set_desktop_pos", SetDesktopPosition},
		{"set_desktop_mode", SetDesktopMode},
		{0, 0}
	};

	static void LuaInit(lua_State* L)
	{
		int top = lua_gettop(L);
		luaL_register(L, "webview", WebView_methods);

		#define SETCONSTANT(name) \
		lua_pushnumber(L, (lua_Number) name); \
		lua_setfield(L, -2, #name);\

		SETCONSTANT(CALLBACK_RESULT_URL_OK)
		SETCONSTANT(CALLBACK_RESULT_URL_ERROR)
		SETCONSTANT(CALLBACK_RESULT_EVAL_OK)
		SETCONSTANT(CALLBACK_RESULT_EVAL_ERROR)
		SETCONSTANT(CALLBACK_RESULT_URL_LOADING)

		#undef SETCONSTANT
		
		lua_pop(L, 1);
		assert(top == lua_gettop(L));
	}

	static dmExtension::Result WebView_AppInitialize(dmExtension::AppParams* params)
	{
		return dmExtension::RESULT_OK;
	}

	static dmExtension::Result WebView_AppFinalize(dmExtension::AppParams* params)
	{
		return dmExtension::RESULT_OK;
	}

	static dmExtension::Result WebView_Initialize(dmExtension::Params* params)
	{
		Webview_openLibrary(params->m_ConfigFile);
		
		// Init Lua
		LuaInit(params->m_L);
		printf("Registered %s Extension\n", "webview");
		return dmExtension::RESULT_OK;
	}

	static dmExtension::Result WebView_Finalize(dmExtension::Params* params)
	{
		Webview_closeLibrary();
		return dmExtension::RESULT_OK;
	}

	static dmExtension::Result WebView_Update(dmExtension::Params* params)
	{
		return dmExtension::RESULT_OK;
	}

	DM_DECLARE_EXTENSION(WebViewExternal, "WebView", WebView_AppInitialize, WebView_AppFinalize, WebView_Initialize, WebView_Update, 0, WebView_Finalize)


} // namespace

#endif
