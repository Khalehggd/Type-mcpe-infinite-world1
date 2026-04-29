/********************************************************************
	Minecraft: Pocket Edition - JNI Bridge
	AndroidBridge.cpp - Connects C++ Mod API to Android
	
	Handles all Android-specific functionality
********************************************************************/

#ifdef __ANDROID__

#include <jni.h>
#include <android/log.h>
#include <string>

#include "../modapi/ModAPI.hpp"
#include "../modapi/PermissionManager.hpp"
#include "../modapi/MicrophoneHandler.hpp"
#include "../modapi/LANMultiplayer.hpp"
#include "../modapi/NotificationManager.hpp"
#include "../modapi/StorageManager.hpp"
#include "../modapi/PlayerProfile.hpp"

#define LOG_TAG "MinecraftPE-Mods"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ════════════════════════════════════════════════════════════════
// GLOBAL JNI ENVIRONMENT
// ════════════════════════════════════════════════════════════════

static JavaVM* g_JavaVM = nullptr;
static jobject g_Activity = nullptr;
static jobject g_Context = nullptr;

// ════════════════════════════════════════════════════════════════
// JNI INITIALIZATION
// ════════════════════════════════════════════════════════════════

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	LOGI("JNI_OnLoad called");
	g_JavaVM = vm;
	
	JNIEnv* env;
	if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
	{
		LOGE("Failed to get JNI environment");
		return JNI_ERR;
	}
	
	LOGI("JNI initialized successfully");
	return JNI_VERSION_1_6;
}

// ════════════════════════════════════════════════════════════════
// INITIALIZATION - Called from Java MainActivity
// ════════════════════════════════════════════════════════════════

extern "C" JNIEXPORT void JNICALL
Java_com_mojang_minecraftpe_MainActivity_nativeInit(
	JNIEnv* env,
	jobject activity,
	jobject context)
{
	LOGI("Native initialization started");
	
	// Store global references
	g_Activity = env->NewGlobalRef(activity);
	g_Context = env->NewGlobalRef(context);
	
	// Initialize all mod systems with Android context
	PermissionManager::getInstance().init(env, activity);
	MicrophoneHandler::getInstance().init(env, context);
	LANMultiplayer::getInstance().init(env, context);
	NotificationManager::getInstance().init(env, context);
	StorageManager::getInstance().init(env, context);
	
	LOGI("All mod systems initialized!");
	
	// Request critical permissions on startup
	auto& permManager = PermissionManager::getInstance();
	
	// Request storage permission (needed for worlds, mods)
	if (!permManager.hasAllPermissions(PermissionManager::getStoragePermissions()))
	{
		LOGI("Requesting storage permissions...");
		permManager.requestPermissions(
			PermissionManager::getStoragePermissions(),
			[](Permission perm, PermissionStatus status) {
				if (status == PermissionStatus::GRANTED)
				{
					LOGI("Storage permission granted!");
				}
				else
				{
					LOGE("Storage permission denied!");
				}
			}
		);
	}
}

// ════════════════════════════════════════════════════════════════
// PERMISSION CALLBACKS - Called from Java
// ════════════════════════════════════════════════════════════════

extern "C" JNIEXPORT void JNICALL
Java_com_mojang_minecraftpe_MainActivity_nativeOnPermissionResult(
	JNIEnv* env,
	jobject thiz,
	jint requestCode,
	jint permission,
	jboolean granted)
{
	PermissionStatus status = granted ? PermissionStatus::GRANTED : PermissionStatus::DENIED;
	Permission perm = static_cast<Permission>(permission);
	
	PermissionManager::getInstance().onPermissionResult(requestCode, perm, status);
	
	if (granted)
	{
		LOGI("Permission %d granted!", permission);
	}
	else
	{
		LOGE("Permission %d denied!", permission);
	}
}

// ════════════════════════════════════════════════════════════════
// MICROPHONE - Called from Java AudioRecord
// ════════════════════════════════════════════════════════════════

extern "C" JNIEXPORT void JNICALL
Java_com_mojang_minecraftpe_MicrophoneRecorder_nativeOnAudioData(
	JNIEnv* env,
	jobject thiz,
	jshortArray audioData,
	jint length)
{
	// Convert Java short[] to C++ vector
	jshort* data = env->GetShortArrayElements(audioData, nullptr);
	std::vector<short> samples(data, data + length);
	env->ReleaseShortArrayElements(audioData, data, JNI_ABORT);
	
	// Process audio data in MicrophoneHandler
	// (Implementation would call registered callbacks)
	
	LOGI("Received %d audio samples", length);
}

// ════════════════════════════════════════════════════════════════
// NETWORK - Called from Java NetworkManager
// ════════════════════════════════════════════════════════════════

extern "C" JNIEXPORT void JNICALL
Java_com_mojang_minecraftpe_NetworkManager_nativeOnServerDiscovered(
	JNIEnv* env,
	jobject thiz,
	jstring serverName,
	jstring ipAddress,
	jint port,
	jint playerCount)
{
	const char* nameChars = env->GetStringUTFChars(serverName, nullptr);
	const char* ipChars = env->GetStringUTFChars(ipAddress, nullptr);
	
	LANServerInfo server;
	server.serverName = nameChars;
	server.ipAddress = ipChars;
	server.port = port;
	server.currentPlayers = playerCount;
	
	env->ReleaseStringUTFChars(serverName, nameChars);
	env->ReleaseStringUTFChars(ipAddress, ipChars);
	
	LOGI("Discovered LAN server: %s at %s:%d", server.serverName.c_str(), server.ipAddress.c_str(), port);
	
	// Notify LANMultiplayer system
	// (Implementation would call registered callbacks)
}

// ════════════════════════════════════════════════════════════════
// UTILITY FUNCTIONS - For mods to call Java from C++
// ════════════════════════════════════════════════════════════════

JNIEnv* getJNIEnv()
{
	JNIEnv* env = nullptr;
	if (g_JavaVM)
	{
		g_JavaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
	}
	return env;
}

jobject getActivity()
{
	return g_Activity;
}

jobject getContext()
{
	return g_Context;
}

// Call Java method from C++
void callJavaMethod(const char* className, const char* methodName, const char* signature, ...)
{
	JNIEnv* env = getJNIEnv();
	if (!env) return;
	
	jclass clazz = env->FindClass(className);
	if (!clazz)
	{
		LOGE("Failed to find class: %s", className);
		return;
	}
	
	jmethodID method = env->GetStaticMethodID(clazz, methodName, signature);
	if (!method)
	{
		LOGE("Failed to find method: %s", methodName);
		return;
	}
	
	va_list args;
	va_start(args, signature);
	env->CallStaticVoidMethodV(clazz, method, args);
	va_end(args);
}

// ════════════════════════════════════════════════════════════════
// MOD API - Request permissions from C++
// ════════════════════════════════════════════════════════════════

extern "C" void requestMicrophonePermissionJNI()
{
	callJavaMethod(
		"com/mojang/minecraftpe/PermissionHelper",
		"requestMicrophone",
		"()V"
	);
}

extern "C" void requestStoragePermissionJNI()
{
	callJavaMethod(
		"com/mojang/minecraftpe/PermissionHelper",
		"requestStorage",
		"()V"
	);
}

extern "C" void showNotificationJNI(const char* title, const char* message)
{
	JNIEnv* env = getJNIEnv();
	if (!env) return;
	
	jstring jTitle = env->NewStringUTF(title);
	jstring jMessage = env->NewStringUTF(message);
	
	callJavaMethod(
		"com/mojang/minecraftpe/NotificationHelper",
		"showNotification",
		"(Ljava/lang/String;Ljava/lang/String;)V",
		jTitle,
		jMessage
	);
	
	env->DeleteLocalRef(jTitle);
	env->DeleteLocalRef(jMessage);
}

// ════════════════════════════════════════════════════════════════
// LOGGING - For mods
// ════════════════════════════════════════════════════════════════

extern "C" void logInfo(const char* message)
{
	LOGI("%s", message);
}

extern "C" void logError(const char* message)
{
	LOGE("%s", message);
}

#endif // __ANDROID__
