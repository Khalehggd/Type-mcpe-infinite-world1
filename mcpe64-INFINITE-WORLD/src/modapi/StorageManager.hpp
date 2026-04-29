/********************************************************************
	Minecraft: Pocket Edition - File & Storage Manager
	StorageManager.hpp - Full file system access for mods
	
	Unrestricted access to files, folders, external storage
********************************************************************/

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <fstream>

#ifdef __ANDROID__
#include <jni.h>
#endif

// ════════════════════════════════════════════════════════════════
// FILE INFO
// ════════════════════════════════════════════════════════════════

struct FileInfo
{
	std::string path;             // Full file path
	std::string name;             // File name only
	std::string extension;        // File extension (e.g., "txt")
	
	bool isDirectory = false;
	bool isFile = false;
	bool isSymlink = false;
	bool isReadable = false;
	bool isWritable = false;
	
	size_t size = 0;              // File size in bytes
	double createdTime = 0.0;     // Creation timestamp
	double modifiedTime = 0.0;    // Last modified timestamp
};

// ════════════════════════════════════════════════════════════════
// STORAGE LOCATIONS
// ════════════════════════════════════════════════════════════════

enum class StorageLocation
{
	INTERNAL,         // App's internal storage
	EXTERNAL,         // External SD card
	DOWNLOADS,        // Downloads folder
	DOCUMENTS,        // Documents folder
	PICTURES,         // Pictures folder
	MUSIC,            // Music folder
	MOVIES,           // Movies/Videos folder
	DCIM,             // Camera photos
	GAME_DATA,        // games/com.mojang/
	CUSTOM            // Custom path
};

// ════════════════════════════════════════════════════════════════
// STORAGE MANAGER
// ════════════════════════════════════════════════════════════════

class StorageManager
{
public:
	static StorageManager& getInstance()
	{
		static StorageManager instance;
		return instance;
	}
	
#ifdef __ANDROID__
	void init(JNIEnv* env, jobject context);
#endif
	
	// ═══════════════════════════════════════════════════════════
	// FILE OPERATIONS
	// ═══════════════════════════════════════════════════════════
	
	// Check if file/folder exists
	bool exists(const std::string& path);
	
	// Get file info
	FileInfo getFileInfo(const std::string& path);
	
	// Create file
	bool createFile(const std::string& path);
	
	// Create directory (recursive)
	bool createDirectory(const std::string& path, bool recursive = true);
	
	// Delete file
	bool deleteFile(const std::string& path);
	
	// Delete directory (can be recursive)
	bool deleteDirectory(const std::string& path, bool recursive = false);
	
	// Copy file
	bool copyFile(const std::string& source, const std::string& destination);
	
	// Move file
	bool moveFile(const std::string& source, const std::string& destination);
	
	// Rename file
	bool renameFile(const std::string& oldPath, const std::string& newPath);
	
	// ═══════════════════════════════════════════════════════════
	// READING FILES
	// ═══════════════════════════════════════════════════════════
	
	// Read entire file as string
	std::string readFile(const std::string& path);
	
	// Read entire file as binary
	std::vector<uint8_t> readBinaryFile(const std::string& path);
	
	// Read file line by line
	std::vector<std::string> readLines(const std::string& path);
	
	// Read file chunk by chunk (for large files)
	typedef std::function<void(const std::vector<uint8_t>&)> ChunkCallback;
	bool readFileChunked(const std::string& path, size_t chunkSize, ChunkCallback callback);
	
	// ═══════════════════════════════════════════════════════════
	// WRITING FILES
	// ═══════════════════════════════════════════════════════════
	
	// Write string to file (overwrites)
	bool writeFile(const std::string& path, const std::string& content);
	
	// Write binary data to file
	bool writeBinaryFile(const std::string& path, const std::vector<uint8_t>& data);
	
	// Append to file
	bool appendToFile(const std::string& path, const std::string& content);
	
	// Write lines to file
	bool writeLines(const std::string& path, const std::vector<std::string>& lines);
	
	// ═══════════════════════════════════════════════════════════
	// DIRECTORY OPERATIONS
	// ═══════════════════════════════════════════════════════════
	
	// List files in directory
	std::vector<FileInfo> listFiles(const std::string& path, bool recursive = false);
	
	// List only files (no directories)
	std::vector<FileInfo> listFilesOnly(const std::string& path);
	
	// List only directories
	std::vector<FileInfo> listDirectories(const std::string& path);
	
	// Find files by extension
	std::vector<FileInfo> findFilesByExtension(const std::string& path, const std::string& extension, bool recursive = true);
	
	// Find files by pattern (simple wildcard matching)
	std::vector<FileInfo> findFilesByPattern(const std::string& path, const std::string& pattern, bool recursive = true);
	
	// ═══════════════════════════════════════════════════════════
	// STORAGE PATHS
	// ═══════════════════════════════════════════════════════════
	
	// Get standard storage paths
	std::string getStoragePath(StorageLocation location);
	
	// Get all available storage locations (internal + external)
	std::vector<std::string> getAvailableStoragePaths();
	
	// Get total/free space
	size_t getTotalSpace(const std::string& path);
	size_t getFreeSpace(const std::string& path);
	size_t getUsedSpace(const std::string& path);
	
	// Check if external storage is available
	bool isExternalStorageAvailable();
	bool isExternalStorageWritable();
	
	// ═══════════════════════════════════════════════════════════
	// COMMON GAME PATHS
	// ═══════════════════════════════════════════════════════════
	
	static std::string getGameDataPath()
	{
		return "/storage/emulated/0/games/com.mojang/";
	}
	
	static std::string getWorldsPath()
	{
		return getGameDataPath() + "minecraftWorlds/";
	}
	
	static std::string getResourcePacksPath()
	{
		return getGameDataPath() + "resource_packs/";
	}
	
	static std::string getBehaviorPacksPath()
	{
		return getGameDataPath() + "behavior_packs/";
	}
	
	static std::string getModsPath()
	{
		return getGameDataPath() + "mods/";
	}
	
	static std::string getSkinsPath()
	{
		return getGameDataPath() + "skins/";
	}
	
	// ═══════════════════════════════════════════════════════════
	// FILE WATCHERS
	// ═══════════════════════════════════════════════════════════
	
	typedef std::function<void(const std::string&)> FileChangeCallback;
	
	// Watch directory for changes
	void watchDirectory(const std::string& path, FileChangeCallback callback);
	void unwatchDirectory(const std::string& path);
	
	// ═══════════════════════════════════════════════════════════
	// COMPRESSION
	// ═══════════════════════════════════════════════════════════
	
	// Create ZIP archive
	bool createZip(const std::string& zipPath, const std::vector<std::string>& files);
	
	// Extract ZIP archive
	bool extractZip(const std::string& zipPath, const std::string& destination);
	
	// List files in ZIP
	std::vector<std::string> listZipContents(const std::string& zipPath);
	
	// ═══════════════════════════════════════════════════════════
	// UTILITIES
	// ═══════════════════════════════════════════════════════════
	
	// Get file size
	size_t getFileSize(const std::string& path);
	
	// Get file extension
	std::string getExtension(const std::string& path);
	
	// Get file name without extension
	std::string getFileNameWithoutExtension(const std::string& path);
	
	// Get directory from path
	std::string getDirectory(const std::string& path);
	
	// Join paths
	std::string joinPaths(const std::string& path1, const std::string& path2);
	
	// Normalize path (remove .., ., etc.)
	std::string normalizePath(const std::string& path);
	
private:
	StorageManager() {}
	
#ifdef __ANDROID__
	JNIEnv* m_env = nullptr;
	jobject m_context = nullptr;
#endif
	
	std::map<std::string, FileChangeCallback> m_watchers;
	
	void fileWatcherThread();
	bool m_shouldStopWatcher = false;
};

// ════════════════════════════════════════════════════════════════
// FILE STREAM HELPERS
// ════════════════════════════════════════════════════════════════

class FileReader
{
public:
	FileReader(const std::string& path) : m_path(path)
	{
		m_stream.open(path, std::ios::binary);
	}
	
	~FileReader() { close(); }
	
	bool isOpen() const { return m_stream.is_open(); }
	void close() { m_stream.close(); }
	
	std::string readAll()
	{
		return std::string((std::istreambuf_iterator<char>(m_stream)),
		                   std::istreambuf_iterator<char>());
	}
	
	std::string readLine()
	{
		std::string line;
		std::getline(m_stream, line);
		return line;
	}
	
	bool hasMore() const { return !m_stream.eof(); }
	
private:
	std::string m_path;
	std::ifstream m_stream;
};

class FileWriter
{
public:
	FileWriter(const std::string& path, bool append = false)
		: m_path(path)
	{
		auto mode = std::ios::binary;
		if (append) mode |= std::ios::app;
		m_stream.open(path, mode);
	}
	
	~FileWriter() { close(); }
	
	bool isOpen() const { return m_stream.is_open(); }
	void close() { m_stream.close(); }
	
	void write(const std::string& content)
	{
		m_stream << content;
	}
	
	void writeLine(const std::string& line)
	{
		m_stream << line << "\n";
	}
	
	void flush() { m_stream.flush(); }
	
private:
	std::string m_path;
	std::ofstream m_stream;
};

// ════════════════════════════════════════════════════════════════
// CONVENIENCE FUNCTIONS
// ════════════════════════════════════════════════════════════════

inline std::string readFile(const std::string& path)
{
	return StorageManager::getInstance().readFile(path);
}

inline bool writeFile(const std::string& path, const std::string& content)
{
	return StorageManager::getInstance().writeFile(path, content);
}

inline bool fileExists(const std::string& path)
{
	return StorageManager::getInstance().exists(path);
}

inline std::vector<FileInfo> listFiles(const std::string& path)
{
	return StorageManager::getInstance().listFiles(path);
}

inline bool deleteFile(const std::string& path)
{
	return StorageManager::getInstance().deleteFile(path);
}

inline bool copyFile(const std::string& source, const std::string& dest)
{
	return StorageManager::getInstance().copyFile(source, dest);
}
