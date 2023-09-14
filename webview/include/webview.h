
extern "C" {

	typedef void *webview_t;
	typedef void *webview_version_info_t;

	// Creates a new webview instance. If debug is non-zero - developer tools will
	// be enabled (if the platform supports them). Window parameter can be a
	// pointer to the native window handle. If it's non-null - then child WebView
	// is embedded into the given parent window. Otherwise a new window is created.
	// Depending on the platform, a GtkWindow, NSWindow or HWND pointer can be
	// passed here. Returns null on failure. Creation can fail for various reasons
	// such as when required runtime dependencies are missing or when window creation
	// fails.
	webview_t webview_create_ex(int debug, void *window, int style, int exstyle);
	webview_t webview_create(int debug, void *window);

	// Destroys a webview and closes the native window.
	void webview_destroy(webview_t w);

	// Runs the main loop until it's terminated. After this function exits - you
	// must destroy the webview.
	void webview_run(webview_t w);

	int webview_poll(webview_t w);

	void webview_resize(webview_t w);

	void set_virtual_hostname(webview_t w, const char *hostname, const char *datapath);
	void add_file_handling( webview_t w);

	// Stops the main loop. It is safe to call this function from another other
	// background thread.
	void webview_terminate(webview_t w);

	// Posts a function to be executed on the main thread. You normally do not need
	// to call this function, unless you want to tweak the native window.
	void webview_dispatch(webview_t w, void (*fn)(webview_t w, void *arg), void *arg);

	// Returns a native window handle pointer. When using GTK backend the pointer
	// is GtkWindow pointer, when using Cocoa backend the pointer is NSWindow
	// pointer, when using Win32 backend the pointer is HWND pointer.
	void *webview_get_window(webview_t w);

	// Updates the title of the native window. Must be called from the UI thread.
	void webview_set_title(webview_t w, const char *title);

	void webview_set_background_color(webview_t w, unsigned int color, int transparent);

	// Window size hints
	#define WEBVIEW_HINT_NONE 0  // Width and height are default size
	#define WEBVIEW_HINT_MIN 1   // Width and height are minimum bounds
	#define WEBVIEW_HINT_MAX 2   // Width and height are maximum bounds
	#define WEBVIEW_HINT_FIXED 3 // Window size can not be changed by a user
	// Updates native window size. See WEBVIEW_HINT constants.
	void webview_set_size(webview_t w, int width, int height, int hints);
	void webview_set_pos(webview_t w, int left, int top);

	// Navigates webview to the given URL. URL may be a properly encoded data URI.
	// Examples:
	// webview_navigate(w, "https://github.com/webview/webview");
	// webview_navigate(w, "data:text/html,%3Ch1%3EHello%3C%2Fh1%3E");
	// webview_navigate(w, "data:text/html;base64,PGgxPkhlbGxvPC9oMT4=");
	void webview_navigate(webview_t w, const char *url);

	// Set webview HTML directly.
	// Example: webview_set_html(w, "<h1>Hello</h1>");
	void webview_set_html(webview_t w, const char *html);

	// Injects JavaScript code at the initialization of the new page. Every time
	// the webview will open a the new page - this initialization code will be
	// executed. It is guaranteed that code is executed before window.onload.
	void webview_init(webview_t w, const char *js);

	// Evaluates arbitrary JavaScript code. Evaluation happens asynchronously, also
	// the result of the expression is ignored. Use RPC bindings if you want to
	// receive notifications about the results of the evaluation.
	void webview_eval(webview_t w, const char *js);

	// Binds a native C callback so that it will appear under the given name as a
	// global JavaScript function. Internally it uses webview_init(). Callback
	// receives a request string and a user-provided argument pointer. Request
	// string is a JSON array of all the arguments passed to the JavaScript
	// function.
	void webview_bind(webview_t w, const char *name,
		void (*fn)(const char *seq, const char *req,
			void *arg),
			void *arg);

	// Removes a native C callback that was previously set by webview_bind.
	void webview_unbind(webview_t w, const char *name);

	// Allows to return a value from the native binding. Original request pointer
	// must be provided to help internal RPC engine match requests with responses.
	// If status is zero - result is expected to be a valid JSON result value.
	// If status is not zero - result is an error JSON object.
	void webview_return(webview_t w, const char *seq, int status,
		const char *result);

	// Get the library's version information.
	// @since 0.10
	const webview_version_info_t *webview_version();
}