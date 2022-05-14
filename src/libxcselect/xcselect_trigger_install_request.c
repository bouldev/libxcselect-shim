#include <xcselect.h>
#include <os/log.h>
#include <libproc.h>

XC_EXPORT
bool xcselect_trigger_install_request(const char *tool_name)
{
	// Never return false except CoreFoundation missing or not callable
#if TARGET_OS_OSX
	typedef const void * CFDictionaryRef;
	typedef const void * CFAllocatorRef;
	typedef const void * CFStringRef;
	typedef const void * CFDataRef;
	typedef const void * CFTypeRef;
	typedef const void * CFPropertyListRef;
	typedef const void * CFMutableDictionaryRef;
	typedef signed long CFIndex;
	typedef unsigned long CFOptionFlags;
	typedef CFIndex CFPropertyListFormat;
	typedef unsigned int CFStringEncoding;
	typedef signed int SInt32;
	typedef double CFTimeInterval;
	typedef struct __CFError * CFErrorRef;
	typedef struct __CFMessagePort * CFMessagePortRef;

	void *CFFunctionCall;

	CFStringRef (*CFStringCreateWithCString)(CFAllocatorRef alloc, const char *cStr, CFStringEncoding encoding);
	void (*CFDictionarySetValue)(CFDictionaryRef theDict, CFStringRef key, CFStringRef value);
	CFDataRef (*CFPropertyListCreateData)(CFAllocatorRef allocator, CFPropertyListRef propertyList, CFPropertyListFormat format, CFOptionFlags options, CFErrorRef *error);
	CFMutableDictionaryRef (*CFDictionaryCreateMutable)(CFAllocatorRef allocator, CFIndex capacity, const void *keyCallBacks, const void *valueCallBacks);
	CFMessagePortRef (*CFMessagePortCreateRemote)(CFAllocatorRef allocator, CFStringRef name);
	SInt32 (*CFMessagePortSendRequest)(CFMessagePortRef remote, SInt32 msgid, CFDataRef data, CFTimeInterval sendTimeout, CFTimeInterval rcvTimeout, CFStringRef replyMode, CFDataRef *returnData);
	void (*CFRelease)(CFTypeRef cf);

	char buf[512] = {0};
	pid_t pid, ppid;
	char dst[512] = {0};
	os_log_t log_clt_install;

	CFMutableDictionaryRef xc_dict;
	CFStringRef xc_tool_name_label, xc_tool_name;

	CFFunctionCall = lazyCFSymbol("CFStringCreateWithCString");
	if (CFFunctionCall) {
		CFStringCreateWithCString = (CFStringRef (*)(CFAllocatorRef alloc, const char *cStr, CFStringEncoding encoding))CFFunctionCall;
		CFFunctionCall = lazyCFSymbol("CFDictionarySetValue");
	}
	if (CFFunctionCall) {
		CFDictionarySetValue = (void (*)(CFDictionaryRef theDict, CFStringRef key, CFStringRef value))CFFunctionCall;
		CFFunctionCall = lazyCFSymbol("CFPropertyListCreateData");
	}
	if (CFFunctionCall) {
		CFPropertyListCreateData = (CFDataRef (*)(CFAllocatorRef allocator, CFPropertyListRef propertyList, CFPropertyListFormat format, CFOptionFlags options, CFErrorRef *error))CFFunctionCall;
		CFFunctionCall = lazyCFSymbol("CFDictionaryCreateMutable");
	}
	if (CFFunctionCall) {
		CFDictionaryCreateMutable = (CFMutableDictionaryRef (*)(CFAllocatorRef allocator, CFIndex capacity, const void *keyCallBacks, const void *valueCallBacks))CFFunctionCall;
		CFFunctionCall = lazyCFSymbol("CFMessagePortCreateRemote");
	}
	if (CFFunctionCall) {
		CFMessagePortCreateRemote = (CFMessagePortRef (*)(CFAllocatorRef allocator, CFStringRef name))CFFunctionCall;
		CFFunctionCall = lazyCFSymbol("CFMessagePortSendRequest");
	}
	if (CFFunctionCall) {
		CFMessagePortSendRequest = (SInt32 (*)(CFMessagePortRef remote, SInt32 msgid, CFDataRef data, CFTimeInterval sendTimeout, CFTimeInterval rcvTimeout, CFStringRef replyMode, CFDataRef *returnData))CFFunctionCall;
		CFFunctionCall = lazyCFSymbol("CFRelease");
	}
	if (CFFunctionCall) {
		CFRelease = (void (*)(CFTypeRef cf))CFFunctionCall;

/*	FIXME: Abort Trap: 6 Due to memcpy() and os_log()
		memcpy(buf, "<unknown>", sizeof(buf));
		pid = getpid();
		proc_pidpath(pid, buf, 0x1000);
		memcpy(dst, "<unknown>", sizeof(dst));
		ppid = getppid();
		proc_pidpath(ppid, dst, 0x1000);
		log_clt_install = os_log_create("com.apple.dt.CommandLineTools.installondemand", "trace");
		os_log(log_clt_install, "Command Line Tools installation request from '%s' (PID %d), parent process '%s' (parent PID %d)", buf, pid, dst, ppid);
		os_release(log_clt_install);
*/
		xc_dict = CFDictionaryCreateMutable(NULL, 0, NULL, NULL);
		xc_tool_name_label = CFStringCreateWithCString(NULL, "tool-name", 0);
		xc_tool_name = CFStringCreateWithCString(NULL, tool_name, 0);
		CFDictionarySetValue(xc_dict, xc_tool_name_label, xc_tool_name);
		CFErrorRef errorOutput;
		CFDataRef pData = CFPropertyListCreateData(NULL, xc_dict, 200, 0, &errorOutput);
		CFRelease(xc_tool_name);
		CFRelease(xc_tool_name_label);
		CFRelease(xc_dict);
		if (pData) {
			CFStringRef installondemandString = CFStringCreateWithCString(NULL, "com.apple.dt.CommandLineTools.installondemand", 0);
			CFMessagePortRef port = CFMessagePortCreateRemote(0, installondemandString);
			if (port) {
				SInt32 stat = CFMessagePortSendRequest(port, 0, pData, 5.0, 5.0, NULL, NULL);
				CFRelease(pData);
				CFRelease(installondemandString);
				CFRelease(port);
				return stat == 0;
			}
			CFRelease(pData);
			CFRelease(installondemandString);
		}
	}
#else
// TODO: Trigger APT/dpkg for installation on iOS
#define CFFunctionCall true
#endif
	return (CFFunctionCall);
}
