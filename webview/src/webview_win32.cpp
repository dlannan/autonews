#if !defined(DM_PLATFORM_ANDROID) && !defined(DM_PLATFORM_IOS) && !defined(DM_PLATFORM_OSX)

#include <stdio.h>
#include <sym_webview.h>


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
	
	/** Creates a web view
	@param callback callback to use for requests
	@return the web view id
	*/
	static int Create(lua_State* L)
	{
		int debug = luaL_checknumber(L,1);

		HWND hwnd = dmGraphics::GetNativeWindowsHWND();
		webview_t wvobj = (webview_t)sym_webview_create_ex(debug, &hwnd, WS_OVERLAPPED, WS_EX_LAYERED);
		sym_webview_set_background_color( wvobj, 0xffffff, 1 );
		lua_pushlightuserdata(L, wvobj);
		return 1;
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

		sym_webview_set_pos(wvobj, x, y);
		sym_webview_set_size(wvobj, width, height, 0);
		sym_webview_resize(wvobj);

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
		{"is_visible", IsVisible},
		{"poll", Poll},
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
