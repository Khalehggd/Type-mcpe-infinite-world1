/********************************************************************
	Minecraft: Pocket Edition - Behavior Pack System
	BehaviorPackManager.hpp - Custom game logic, entities, items
	
	Allows mods to change game behavior without C++ code
********************************************************************/

#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

// ════════════════════════════════════════════════════════════════
// BEHAVIOR PACK MANIFEST
// ════════════════════════════════════════════════════════════════

struct BehaviorPackManifest
{
	std::string uuid;
	std::string name;
	std::string description;
	std::string version;
	int format_version = 1;
	
	// Dependencies
	std::vector<std::string> dependencies;
	
	// Capabilities
	bool hasScripts = false;
	bool hasEntities = false;
	bool hasItems = false;
	bool hasBlocks = false;
	bool hasRecipes = false;
	bool hasLootTables = false;
};

// ════════════════════════════════════════════════════════════════
// CUSTOM ENTITY DEFINITION
// ════════════════════════════════════════════════════════════════

struct EntityBehavior
{
	std::string identifier;
	std::string name;
	
	// Properties
	float health = 20.0f;
	float speed = 0.25f;
	bool canSwim = true;
	bool canClimb = false;
	
	// AI behaviors (simple function callbacks)
	std::function<void(void* entity)> onTick;
	std::function<void(void* entity, void* player)> onInteract;
	std::function<void(void* entity)> onDeath;
};

// ════════════════════════════════════════════════════════════════
// CUSTOM ITEM DEFINITION
// ════════════════════════════════════════════════════════════════

struct ItemBehavior
{
	std::string identifier;
	std::string name;
	int maxStackSize = 64;
	bool canDestroyInCreative = true;
	
	// Callbacks
	std::function<void(void* item, void* player)> onUse;
	std::function<void(void* item, void* block)> onUseOnBlock;
};

// ════════════════════════════════════════════════════════════════
// CUSTOM BLOCK DEFINITION
// ════════════════════════════════════════════════════════════════

struct BlockBehavior
{
	std::string identifier;
	std::string name;
	float destroyTime = 1.0f;
	float explosionResistance = 0.0f;
	bool solid = true;
	
	// Callbacks
	std::function<void(int x, int y, int z)> onPlace;
	std::function<void(int x, int y, int z)> onBreak;
	std::function<void(int x, int y, int z)> onTick;
};

// ════════════════════════════════════════════════════════════════
// BEHAVIOR PACK CLASS
// ════════════════════════════════════════════════════════════════

class BehaviorPack
{
public:
	BehaviorPack(const std::string& path);
	~BehaviorPack();
	
	bool load();
	bool isValid() const { return m_valid; }
	
	BehaviorPackManifest getManifest() const { return m_manifest; }
	std::string getPath() const { return m_path; }
	
	// Get definitions
	std::vector<EntityBehavior> getEntities() const { return m_entities; }
	std::vector<ItemBehavior> getItems() const { return m_items; }
	std::vector<BlockBehavior> getBlocks() const { return m_blocks; }
	
private:
	std::string m_path;
	BehaviorPackManifest m_manifest;
	bool m_valid = false;
	
	std::vector<EntityBehavior> m_entities;
	std::vector<ItemBehavior> m_items;
	std::vector<BlockBehavior> m_blocks;
	
	bool loadManifest();
	void loadBehaviors();
};

// ════════════════════════════════════════════════════════════════
// BEHAVIOR PACK MANAGER
// ════════════════════════════════════════════════════════════════

class BehaviorPackManager
{
public:
	static BehaviorPackManager& getInstance()
	{
		static BehaviorPackManager instance;
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
	// BEHAVIOR LOADING
	// ═══════════════════════════════════════════════════════════
	
	void loadAllPacks();
	
	// Register behaviors into game
	void registerAllBehaviors();
	
	// ═══════════════════════════════════════════════════════════
	// QUERIES
	// ═══════════════════════════════════════════════════════════
	
	std::vector<BehaviorPackManifest> getAvailablePacks() const;
	std::vector<BehaviorPackManifest> getEnabledPacks() const;
	
	// Get custom definitions
	EntityBehavior* getEntity(const std::string& identifier);
	ItemBehavior* getItem(const std::string& identifier);
	BlockBehavior* getBlock(const std::string& identifier);
	
	// ═══════════════════════════════════════════════════════════
	// PACK DIRECTORY
	// ═══════════════════════════════════════════════════════════
	
	static std::string getDefaultPackPath()
	{
		return "/storage/emulated/0/games/com.mojang/behavior_packs/";
	}
	
private:
	BehaviorPackManager() {}
	
	std::map<std::string, std::shared_ptr<BehaviorPack>> m_availablePacks;
	std::vector<std::string> m_enabledPacks; // UUIDs in priority order
	
	// Merged behaviors from all packs
	std::map<std::string, EntityBehavior> m_entities;
	std::map<std::string, ItemBehavior> m_items;
	std::map<std::string, BlockBehavior> m_blocks;
	
	void rebuildBehaviors();
};
