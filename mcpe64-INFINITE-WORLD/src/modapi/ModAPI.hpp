/********************************************************************
	Minecraft: Pocket Edition - Ultimate Modding Framework
	ModAPI.hpp - Core Modding System
	
	Provides unrestricted access to game systems for mods
********************************************************************/

#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

// Forward declarations
class Level;
class Player;
class Dimension;
class Tile;

// ════════════════════════════════════════════════════════════════
// MOD INFO - Metadata about a mod
// ════════════════════════════════════════════════════════════════

struct ModInfo
{
	std::string id;              // Unique mod identifier
	std::string name;            // Display name
	std::string version;         // Version string (e.g., "1.0.0")
	std::string author;          // Mod creator
	std::string description;     // What the mod does
	
	// Permissions requested
	bool requiresMicrophone = false;
	bool requiresStorage = false;
	bool requiresNetwork = false;
	bool requiresNotifications = false;
};

// ════════════════════════════════════════════════════════════════
// MOD LIFECYCLE CALLBACKS
// ════════════════════════════════════════════════════════════════

class IMod
{
public:
	virtual ~IMod() {}
	
	// Called when mod is loaded
	virtual void onLoad() = 0;
	
	// Called when mod is enabled
	virtual void onEnable() = 0;
	
	// Called when mod is disabled
	virtual void onDisable() = 0;
	
	// Called every game tick
	virtual void onTick(Level* level) {}
	
	// Called when player joins
	virtual void onPlayerJoin(Player* player) {}
	
	// Called when player leaves
	virtual void onPlayerLeave(Player* player) {}
	
	// Get mod info
	virtual ModInfo getInfo() const = 0;
};

// ════════════════════════════════════════════════════════════════
// GAME HOOK SYSTEM - Intercept game events
// ════════════════════════════════════════════════════════════════

enum class HookType
{
	BLOCK_PLACE,
	BLOCK_BREAK,
	PLAYER_MOVE,
	PLAYER_DAMAGE,
	ENTITY_SPAWN,
	WORLD_GENERATE,
	DIMENSION_CHANGE,
	CHAT_MESSAGE,
	COMMAND_EXECUTE
};

typedef std::function<bool(void* eventData)> HookCallback;

// ════════════════════════════════════════════════════════════════
// MOD MANAGER - Central mod registry and loader
// ════════════════════════════════════════════════════════════════

class ModManager
{
public:
	static ModManager& getInstance()
	{
		static ModManager instance;
		return instance;
	}
	
	// ═══════════════════════════════════════════════════════════
	// MOD REGISTRATION
	// ═══════════════════════════════════════════════════════════
	
	void registerMod(std::shared_ptr<IMod> mod)
	{
		ModInfo info = mod->getInfo();
		m_mods[info.id] = mod;
		m_modLoadOrder.push_back(info.id);
	}
	
	// ═══════════════════════════════════════════════════════════
	// MOD LIFECYCLE
	// ═══════════════════════════════════════════════════════════
	
	void loadAllMods()
	{
		for (const auto& modId : m_modLoadOrder)
		{
			auto mod = m_mods[modId];
			mod->onLoad();
			m_loadedMods.push_back(modId);
		}
	}
	
	void enableAllMods()
	{
		for (const auto& modId : m_loadedMods)
		{
			auto mod = m_mods[modId];
			mod->onEnable();
			m_enabledMods.insert(modId);
		}
	}
	
	void disableMod(const std::string& modId)
	{
		if (m_enabledMods.count(modId))
		{
			m_mods[modId]->onDisable();
			m_enabledMods.erase(modId);
		}
	}
	
	// ═══════════════════════════════════════════════════════════
	// HOOK REGISTRATION
	// ═══════════════════════════════════════════════════════════
	
	void registerHook(HookType type, const std::string& modId, HookCallback callback)
	{
		m_hooks[type].push_back({modId, callback});
	}
	
	bool triggerHook(HookType type, void* eventData)
	{
		if (m_hooks.find(type) == m_hooks.end())
			return true; // No hooks registered, allow event
		
		for (auto& hook : m_hooks[type])
		{
			// If any hook returns false, cancel the event
			if (!hook.callback(eventData))
				return false;
		}
		return true;
	}
	
	// ═══════════════════════════════════════════════════════════
	// TICK SYSTEM
	// ═══════════════════════════════════════════════════════════
	
	void tickAllMods(Level* level)
	{
		for (const auto& modId : m_loadedMods)
		{
			if (m_enabledMods.count(modId))
			{
				m_mods[modId]->onTick(level);
			}
		}
	}
	
	// ═══════════════════════════════════════════════════════════
	// UNRESTRICTED API ACCESS
	// ═══════════════════════════════════════════════════════════
	
	// Allow mods to access ANYTHING
	template<typename T>
	void setGlobalPointer(const std::string& key, T* ptr)
	{
		m_globalPointers[key] = static_cast<void*>(ptr);
	}
	
	template<typename T>
	T* getGlobalPointer(const std::string& key)
	{
		if (m_globalPointers.find(key) != m_globalPointers.end())
		{
			return static_cast<T*>(m_globalPointers[key]);
		}
		return nullptr;
	}
	
	// ═══════════════════════════════════════════════════════════
	// MOD QUERIES
	// ═══════════════════════════════════════════════════════════
	
	bool isModLoaded(const std::string& modId) const
	{
		return m_mods.find(modId) != m_mods.end();
	}
	
	bool isModEnabled(const std::string& modId) const
	{
		return m_enabledMods.count(modId) > 0;
	}
	
	std::vector<ModInfo> getAllModInfo() const
	{
		std::vector<ModInfo> infos;
		for (const auto& pair : m_mods)
		{
			infos.push_back(pair.second->getInfo());
		}
		return infos;
	}

private:
	ModManager() {}
	
	std::map<std::string, std::shared_ptr<IMod>> m_mods;
	std::vector<std::string> m_modLoadOrder;
	std::vector<std::string> m_loadedMods;
	std::set<std::string> m_enabledMods;
	
	// Hook system
	struct Hook {
		std::string modId;
		HookCallback callback;
	};
	std::map<HookType, std::vector<Hook>> m_hooks;
	
	// Global pointer registry for unrestricted access
	std::map<std::string, void*> m_globalPointers;
};

// ════════════════════════════════════════════════════════════════
// CONVENIENCE MACROS
// ════════════════════════════════════════════════════════════════

#define REGISTER_MOD(ModClass) \
	ModManager::getInstance().registerMod(std::make_shared<ModClass>())

#define GET_GAME_LEVEL() \
	ModManager::getInstance().getGlobalPointer<Level>("level")

#define GET_LOCAL_PLAYER() \
	ModManager::getInstance().getGlobalPointer<Player>("local_player")
