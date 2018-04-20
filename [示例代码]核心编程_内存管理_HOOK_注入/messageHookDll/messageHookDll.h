#ifdef MESSAGEHOOKDLL_EXPORTS
#define MESSAGEHOOKDLL_API extern"C" __declspec(dllexport)
#else
#define MESSAGEHOOKDLL_API extern"C" __declspec(dllimport)
#endif

#define FUNCTION_INSTALLHOOK	"installHook"
#define FUNCTION_UNINSTALLHOOK	"uninstallHook"

typedef int( __stdcall *cb_installHook )( );
typedef int( __stdcall *cb_uninstallHook )( );



// 安装钩子
MESSAGEHOOKDLL_API int installHook( );

// 卸载钩子
MESSAGEHOOKDLL_API int uninstallHook( ) ;