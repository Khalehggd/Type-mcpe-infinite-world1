/********************************************************************
	Minecraft: Pocket Edition - Resource Pack System
	ResourcePackManager.hpp - Load textures, sounds, models
	
	Supports modern Bedrock-style resource packs
********************************************************************/

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

// ════════════════════════════════════════════════════════════════
// RESOURCE PACK STRUCTURE
// ════════════════════════════════════════════════════════════════

struct ResourcePackManifest
{
	std::string uuid;
	std::string name;
	std::string description;
	std::string version;
	int format_version = 1;
	
	// Capabilities
	bool hasTextures = false;
	bool hasSounds = false;
	bool hasModels = false;
	bool hasAnimations = false;
};

// ════════════════════════════════════════════════════════════════
// RESOURCE TYPES
// ════════════════════════════════════════════════════════════════

enum class ResourceType
{
	TEXTURE,      // PNG/JPG images
	SOUND,        // OGG/WAV audio
	MODEL,        // JSON models
	ANIMATION,    // JSON animations
	PARTICLE,     // JSON particles
	LANGUAGE,     // Translation files
	UI           // UI definitions
};

// ════════════════════════════════════════════════════════════════
// RESOURCE PACK CLASS
// ════════════════════════════════════════════════════════════════

class ResourcePack
{
public:
	ResourcePack(const std::string& path);
	~ResourcePack();
	
	bool load();
	bool isValid() const { return m_valid; }
	
	ResourcePackManifest getManifest() const { return m_manifest; }
	std::string getPath() const { return m_path; }
	
	// Get resources
	std::string getTexturePath(const std::string& name) const;
	std::string getSoundPath(const std::string& name) const;
	std::string getModelData(const std::string& name) const;
	
private:
	std::string m_path;
	ResourcePackManifest m_manifest;
	bool m_valid = false;
	
	std::map<std::string, std::string> m_textures;
	std::map<std::string, std::string> m_sounds;
	std::map<std::string, std::string> m_models;
	
	bool loadManifest();
	void scanResources();
};

// ════════════════════════════════════════════════════════════════
// RESOURCE PACK MANAGER
// ════════════════════════════════════════════════════════════════

class ResourcePackManager
{
public:
	static ResourcePackManager& getInstance()
	{
		static ResourcePackManager instance;
		return instance;
	}
	
	// ═══════════════════════════════════════════════════════════
	// PACK MANAGEMENT
	// ═══════════════════════════════════════════════════════════
	
	void scanForPacks(const std::string& directory);
	
	bool addPack(const std::string& path);
	bool removePack(const std::string& uuid);
	
	void enablePack(const std::string& uuid);
	void disablePack(const std::string& uuid);
	
	bool isPackEnabled(const std::string& uuid) const;
	
	// ═══════════════════════════════════════════════════════════
	// RESOURCE LOADING
	// ═══════════════════════════════════════════════════════════
	
	// Load all enabled packs
	void loadAllPacks();
	
	// Get resource from active packs (checks in priority order)
	std::string getTexture(const std::string& name);
	std::string getSound(const std::string& name);
	std::string getModel(const std::string& name);
	
	// ═══════════════════════════════════════════════════════════
	// PACK QUERIES
	// ═══════════════════════════════════════════════════════════
	
	std::vector<ResourcePackManifest> getAvailablePacks() const;
	std::vector<ResourcePackManifest> getEnabledPacks() const;
	
	// ═══════════════════════════════════════════════════════════
	// PACK DIRECTORY STRUCTURE
	// ═══════════════════════════════════════════════════════════
	
	static std::string getDefaultPackPath()
	{
		return "/storage/emulated/0/games/com.mojang/resource_packs/";
	}
	
private:
	ResourcePackManager() {}
	
	std::map<std::string, std::shared_ptr<ResourcePack>> m_availablePacks;
	std::vector<std::string> m_enabledPacks; // UUIDs in priority order
	
	std::map<std::string, std::string> m_textureCache;
	std::map<std::string, std::string> m_soundCache;
	std::map<std::string, std::string> m_modelCache;
	
	void rebuildCache();
};

// ════════════════════════════════════════════════════════════════
// CONVENIENCE FUNCTIONS
// ════════════════════════════════════════════════════════════════

inline std::string getResourceTexture(const std::string& name)
{
	return ResourcePackManager::getInstance().getTexture(name);
}

inline std::string getResourceSound(const std::string& name)
{
	return ResourcePackManager::getInstance().getSound(name);
}
