/********************************************************************
	Minecraft: Pocket Edition - File & Storage System
	FileManager.hpp - Full file system access for mods
	
	Unrestricted file access, document reading, media access
********************************************************************/

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <memory>

#ifdef __ANDROID__
#include <jni.h>
#endif

// ════════════════════════════════════════════════════════════════
// FILE INFO
// ════════════════════════════════════════════════════════════════

struct FileInfo
{
	std::string path;             // Full path
	std::string name;             // File name
	std::string extension;        // File extension (e.g., ".txt")
	
	long long size = 0;           // Size in bytes
	bool isDirectory = false;
	bool isReadable = true;
	bool isWritable = true;
	
	// Timestamps
	long long createdTime = 0;
	long long modifiedTime = 0;
	long long accessedTime = 0;
	
	// MIME type (for Android)
	std::string mimeType;
};

// ════════════════════════════════════════════════════════════════
// DIRECTORY LISTING
// ════════════════════════════════════════════════════════════════

struct DirectoryListing
{
	std::string path;
	std::vector<FileInfo> files;
	std::vector<FileInfo> directories;
	
	int getTotalCount() const { return files.size() + directories.size(); }
};

// ════════════════════════════════════════════════════════════════
// FILE OPERATION RESULT
// ════════════════════════════════════════════════════════════════

struct FileOperationResult
{
	bool success = false;
	std::string errorMessage;
	long long bytesProcessed = 0;
};

// ════════════════════════════════════════════════════════════════
// FILE MANAGER
// ════════════════════════════════════════════════════════════════

class FileManager
{
public:
	static FileManager& getInstance()
	{
		static FileManager instance;
		return instance;
	}
	
#ifdef __ANDROID__
	void init(JNIEnv* env, jobject context);
#endif
	
	// ═══════════════════════════════════════════════════════════
	// FILE READING
	// ═══════════════════════════════════════════════════════════
	
	// Read entire file as string
	std::string readFile(const std::string& path);
	
	// Read file as binary data
	std::vector<uint8_t> readBinaryFile(const std::string& path);
	
	// Read file line by line
	std::vector<std::string> readLines(const std::string& path);
	
	// Stream file (for large files)
	std::ifstream openFileForReading(const std::string& path);
	
	// ═══════════════════════════════════════════════════════════
	// FILE WRITING
	// ═══════════════════════════════════════════════════════════
	
	// Write string to file (overwrites)
	FileOperationResult writeFile(const std::string& path, const std::string& content);
	
	// Write binary data
	FileOperationResult writeBinaryFile(const std::string& path, const std::vector<uint8_t>& data);
	
	// Append to file
	FileOperationResult appendToFile(const std::string& path, const std::string& content);
	
	// Stream file (for large files)
	std::ofstream openFileForWriting(const std::string& path, bool append = false);
	
	// ═══════════════════════════════════════════════════════════
	// FILE OPERATIONS
	// ═══════════════════════════════════════════════════════════
	
	// Copy file
	FileOperationResult copyFile(const std::string& source, const std::string& destination);
	
	// Move/rename file
	FileOperationResult moveFile(const std::string& source, const std::string& destination);
	
	// Delete file
	FileOperationResult deleteFile(const std::string& path);
	
	// Check if file exists
	bool fileExists(const std::string& path);
	
	// Get file info
	FileInfo getFileInfo(const std::string& path);
	
	// ═══════════════════════════════════════════════════════════
	// DIRECTORY OPERATIONS
	// ═══════════════════════════════════════════════════════════
	
	// Create directory
	FileOperationResult createDirectory(const std::string& path, bool createParents = true);
	
	// Delete directory
	FileOperationResult deleteDirectory(const std::string& path, bool recursive = false);
	
	// List directory contents
	DirectoryListing listDirectory(const std::string& path, bool recursive = false);
	
	// Check if directory exists
	bool directoryExists(const std::string& path);
	
	// ═══════════════════════════════════════════════════════════
	// SEARCH & FILTER
	// ═══════════════════════════════════════════════════════════
	
	// Find files by pattern
	std::vector<FileInfo> findFiles(const std::string& directory, const std::string& pattern);
	
	// Find files by extension
	std::vector<FileInfo> findFilesByExtension(const std::string& directory, const std::string& extension);
	
	// Search file contents
	std::vector<std::string> searchInFiles(const std::string& directory, const std::string& searchText);
	
	// ═══════════════════════════════════════════════════════════
	// COMMON PATHS (Android)
	// ═══════════════════════════════════════════════════════════
	
	static std::string getInternalStoragePath()
	{
		return "/storage/emulated/0/";
	}
	
	static std::string getDownloadsPath()
	{
		return "/storage/emulated/0/Download/";
	}
	
	static std::string getDocumentsPath()
	{
		return "/storage/emulated/0/Documents/";
	}
	
	static std::string getPicturesPath()
	{
		return "/storage/emulated/0/Pictures/";
	}
	
	static std::string getMusicPath()
	{
		return "/storage/emulated/0/Music/";
	}
	
	static std::string getVideosPath()
	{
		return "/storage/emulated/0/Movies/";
	}
	
	static std::string getMinecraftPath()
	{
		return "/storage/emulated/0/games/com.mojang/";
	}
	
	static std::string getModsPath()
	{
		return "/storage/emulated/0/games/com.mojang/mods/";
	}
	
	static std::string getWorldsPath()
	{
		return "/storage/emulated/0/games/com.mojang/minecraftWorlds/";
	}
	
	// ═══════════════════════════════════════════════════════════
	// UTILITY FUNCTIONS
	// ═══════════════════════════════════════════════════════════
	
	// Get file size
	long long getFileSize(const std::string& path);
	
	// Get file extension
	std::string getFileExtension(const std::string& path);
	
	// Get file name without extension
	std::string getFileNameWithoutExtension(const std::string& path);
	
	// Get directory from path
	std::string getDirectoryFromPath(const std::string& path);
	
	// Combine paths
	std::string combinePath(const std::string& path1, const std::string& path2);
	
	// Get available storage space (bytes)
	long long getAvailableSpace(const std::string& path = "/storage/emulated/0/");
	
	// Get total storage space (bytes)
	long long getTotalSpace(const std::string& path = "/storage/emulated/0/");
	
	// ═══════════════════════════════════════════════════════════
	// MEDIA SCANNER (Android)
	// ═══════════════════════════════════════════════════════════
	
	// Notify Android that a file was created (shows in gallery, etc.)
	void scanMediaFile(const std::string& path);
	
	// ═══════════════════════════════════════════════════════════
	// FILE PICKER (Android)
	// ═══════════════════════════════════════════════════════════
	
	typedef std::function<void(const std::string& path)> FilePickerCallback;
	
	// Open file picker dialog
	void openFilePicker(const std::string& mimeType, FilePickerCallback callback);
	
	// Open directory picker
	void openDirectoryPicker(FilePickerCallback callback);
	
	// Open save file dialog
	void openSaveDialog(const std::string& defaultName, FilePickerCallback callback);
	
private:
	FileManager() {}
	
#ifdef __ANDROID__
	JNIEnv* m_env = nullptr;
	jobject m_context = nullptr;
#endif
	
	std::map<int, FilePickerCallback> m_pickerCallbacks;
	int m_nextPickerRequestCode = 2000;
};

// ════════════════════════════════════════════════════════════════
// JSON HELPER (for config files)
// ════════════════════════════════════════════════════════════════

class JSONHelper
{
public:
	// Simple JSON parser (basic implementation)
	static std::map<std::string, std::string> parseJSON(const std::string& json);
	
	// Convert map to JSON string
	static std::string toJSON(const std::map<std::string, std::string>& data);
	
	// Read JSON from file
	static std::map<std::string, std::string> readJSONFile(const std::string& path);
	
	// Write JSON to file
	static bool writeJSONFile(const std::string& path, const std::map<std::string, std::string>& data);
};

// ════════════════════════════════════════════════════════════════
// CONFIG FILE MANAGER
// ════════════════════════════════════════════════════════════════

class ConfigManager
{
public:
	static ConfigManager& getInstance()
	{
		static ConfigManager instance;
		return instance;
	}
	
	// Load config
	bool loadConfig(const std::string& modId);
	
	// Save config
	bool saveConfig(const std::string& modId);
	
	// Get/set values
	std::string getValue(const std::string& modId, const std::string& key, const std::string& defaultValue = "");
	void setValue(const std::string& modId, const std::string& key, const std::string& value);
	
	int getInt(const std::string& modId, const std::string& key, int defaultValue = 0);
	void setInt(const std::string& modId, const std::string& key, int value);
	
	float getFloat(const std::string& modId, const std::string& key, float defaultValue = 0.0f);
	void setFloat(const std::string& modId, const std::string& key, float value);
	
	bool getBool(const std::string& modId, const std::string& key, bool defaultValue = false);
	void setBool(const std::string& modId, const std::string& key, bool value);
	
private:
	ConfigManager() {}
	
	std::map<std::string, std::map<std::string, std::string>> m_configs;
	
	std::string getConfigPath(const std::string& modId);
};

// ════════════════════════════════════════════════════════════════
// CONVENIENCE FUNCTIONS
// ════════════════════════════════════════════════════════════════

inline std::string readFile(const std::string& path)
{
	return FileManager::getInstance().readFile(path);
}

inline bool writeFile(const std::string& path, const std::string& content)
{
	return FileManager::getInstance().writeFile(path, content).success;
}

inline bool fileExists(const std::string& path)
{
	return FileManager::getInstance().fileExists(path);
}

inline DirectoryListing listFiles(const std::string& path)
{
	return FileManager::getInstance().listDirectory(path);
}

inline bool createFolder(const std::string& path)
{
	return FileManager::getInstance().createDirectory(path).success;
}
