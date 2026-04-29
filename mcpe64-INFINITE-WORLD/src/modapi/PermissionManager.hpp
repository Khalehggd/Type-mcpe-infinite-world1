/********************************************************************
	Minecraft: Pocket Edition - Android Permission System
	PermissionManager.hpp - Handle microphone, storage, notifications
	
	Manages runtime permissions for Android 6.0+
********************************************************************/

#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

#ifdef __ANDROID__
#include <jni.h>
#endif

// ════════════════════════════════════════════════════════════════
// PERMISSION TYPES
// ════════════════════════════════════════════════════════════════

enum class Permission
{
	MICROPHONE,           // android.permission.RECORD_AUDIO
	STORAGE_READ,         // android.permission.READ_EXTERNAL_STORAGE
	STORAGE_WRITE,        // android.permission.WRITE_EXTERNAL_STORAGE
	NOTIFICATIONS,        // android.permission.POST_NOTIFICATIONS (Android 13+)
	CAMERA,               // android.permission.CAMERA
	NETWORK_STATE,        // android.permission.ACCESS_NETWORK_STATE
	WIFI_STATE,           // android.permission.ACCESS_WIFI_STATE
	INTERNET             // android.permission.INTERNET
};

enum class PermissionStatus
{
	GRANTED,
	DENIED,
	NOT_REQUESTED
};

// ════════════════════════════════════════════════════════════════
// PERMISSION REQUEST CALLBACK
// ════════════════════════════════════════════════════════════════

typedef std::function<void(Permission, PermissionStatus)> PermissionCallback;

// ════════════════════════════════════════════════════════════════
// PERMISSION MANAGER
// ════════════════════════════════════════════════════════════════

class PermissionManager
{
public:
	static PermissionManager& getInstance()
	{
		static PermissionManager instance;
		return instance;
	}
	
#ifdef __ANDROID__
	// Initialize with JNI environment
	void init(JNIEnv* env, jobject activity);
#endif
	
	// ═══════════════════════════════════════════════════════════
	// PERMISSION CHECKING
	// ═══════════════════════════════════════════════════════════
	
	PermissionStatus checkPermission(Permission permission);
	bool hasPermission(Permission permission);
	
	// Check multiple permissions at once
	bool hasAllPermissions(const std::vector<Permission>& permissions);
	
	// ═══════════════════════════════════════════════════════════
	// PERMISSION REQUESTING
	// ═══════════════════════════════════════════════════════════
	
	void requestPermission(Permission permission, PermissionCallback callback);
	void requestPermissions(const std::vector<Permission>& permissions, PermissionCallback callback);
	
	// Request with explanation
	void requestPermissionWithRationale(
		Permission permission,
		const std::string& rationale,
		PermissionCallback callback
	);
	
	// ═══════════════════════════════════════════════════════════
	// PERMISSION UTILITIES
	// ═══════════════════════════════════════════════════════════
	
	// Open app settings if user denied permission
	void openAppSettings();
	
	// Get human-readable permission name
	std::string getPermissionName(Permission permission);
	
	// Get Android permission string
	std::string getAndroidPermissionString(Permission permission);
	
	// ═══════════════════════════════════════════════════════════
	// REQUIRED PERMISSIONS FOR FEATURES
	// ═══════════════════════════════════════════════════════════
	
	static std::vector<Permission> getMicrophonePermissions()
	{
		return {Permission::MICROPHONE};
	}
	
	static std::vector<Permission> getStoragePermissions()
	{
		return {Permission::STORAGE_READ, Permission::STORAGE_WRITE};
	}
	
	static std::vector<Permission> getNetworkPermissions()
	{
		return {Permission::INTERNET, Permission::NETWORK_STATE, Permission::WIFI_STATE};
	}
	
	static std::vector<Permission> getNotificationPermissions()
	{
		return {Permission::NOTIFICATIONS};
	}
	
	// ═══════════════════════════════════════════════════════════
	// CALLBACK HANDLING (Called from JNI)
	// ═══════════════════════════════════════════════════════════
	
	void onPermissionResult(int requestCode, Permission permission, PermissionStatus status);
	
private:
	PermissionManager() {}
	
#ifdef __ANDROID__
	JNIEnv* m_env = nullptr;
	jobject m_activity = nullptr;
#endif
	
	std::map<Permission, PermissionStatus> m_permissionCache;
	std::map<int, PermissionCallback> m_pendingCallbacks;
	int m_nextRequestCode = 1000;
	
	int getNextRequestCode() { return m_nextRequestCode++; }
};

// ════════════════════════════════════════════════════════════════
// CONVENIENCE FUNCTIONS
// ════════════════════════════════════════════════════════════════

inline bool hasMicrophonePermission()
{
	return PermissionManager::getInstance().hasPermission(Permission::MICROPHONE);
}

inline bool hasStoragePermission()
{
	return PermissionManager::getInstance().hasAllPermissions(
		PermissionManager::getStoragePermissions()
	);
}

inline bool hasNetworkPermission()
{
	return PermissionManager::getInstance().hasAllPermissions(
		PermissionManager::getNetworkPermissions()
	);
}

inline void requestMicrophonePermission(PermissionCallback callback)
{
	PermissionManager::getInstance().requestPermission(Permission::MICROPHONE, callback);
}

inline void requestStoragePermission(PermissionCallback callback)
{
	PermissionManager::getInstance().requestPermissions(
		PermissionManager::getStoragePermissions(),
		callback
	);
}
