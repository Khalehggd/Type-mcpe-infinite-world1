/********************************************************************
	Minecraft: Pocket Edition - Player Profile System
	PlayerProfile.hpp - Names, nicknames, skins, statistics
	
	Full player identity and customization system
********************************************************************/

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

// ════════════════════════════════════════════════════════════════
// SKIN DATA
// ════════════════════════════════════════════════════════════════

struct PlayerSkin
{
	std::string skinId;           // Unique identifier
	std::string skinName;         // Display name
	std::string filePath;         // Path to skin texture (PNG)
	
	// Skin dimensions (classic = 64x32, new = 64x64)
	int width = 64;
	int height = 64;
	
	// Skin type
	enum class SkinType
	{
		CLASSIC,      // Steve/Alex
		SLIM,         // Alex model
		CUSTOM        // Custom uploaded
	} type = SkinType::CUSTOM;
	
	// Raw texture data (RGBA)
	std::vector<uint8_t> textureData;
	
	bool isValid() const { return !textureData.empty(); }
};

// ════════════════════════════════════════════════════════════════
// PLAYER STATISTICS
// ════════════════════════════════════════════════════════════════

struct PlayerStatistics
{
	// Time played
	int totalPlayTime = 0;        // Seconds
	int sessionsPlayed = 0;
	
	// Blocks
	int blocksMined = 0;
	int blocksPlaced = 0;
	
	// Travel
	float distanceWalked = 0.0f;  // Meters
	float distanceSprinted = 0.0f;
	float distanceSwam = 0.0f;
	float distanceFlown = 0.0f;
	
	// Combat
	int mobsKilled = 0;
	int deaths = 0;
	int playerKills = 0;
	
	// Items
	int itemsCrafted = 0;
	int itemsPickedUp = 0;
	int itemsDropped = 0;
	
	// World
	int worldsCreated = 0;
	int dimensionsVisited = 0;
	
	// Custom stats (for mods)
	std::map<std::string, int> customStats;
};

// ════════════════════════════════════════════════════════════════
// PLAYER PREFERENCES
// ════════════════════════════════════════════════════════════════

struct PlayerPreferences
{
	// Display
	bool showCoordinates = true;
	bool showPlayerNames = true;
	float uiScale = 1.0f;
	
	// Controls
	float mouseSensitivity = 1.0f;
	bool invertMouse = false;
	bool autoJump = true;
	
	// Audio
	float masterVolume = 1.0f;
	float musicVolume = 1.0f;
	float soundVolume = 1.0f;
	bool enableVoiceChat = true;
	
	// Multiplayer
	bool allowFriendRequests = true;
	bool showOnlineStatus = true;
	std::string preferredLanguage = "en_US";
	
	// Custom preferences (for mods)
	std::map<std::string, std::string> customPreferences;
};

// ════════════════════════════════════════════════════════════════
// PLAYER PROFILE
// ════════════════════════════════════════════════════════════════

class PlayerProfile
{
public:
	PlayerProfile(const std::string& playerId);
	~PlayerProfile();
	
	// ═══════════════════════════════════════════════════════════
	// IDENTITY
	// ═══════════════════════════════════════════════════════════
	
	std::string getPlayerId() const { return m_playerId; }
	
	// Player name (account name, cannot change easily)
	void setPlayerName(const std::string& name) { m_playerName = name; }
	std::string getPlayerName() const { return m_playerName; }
	
	// Display name / Nickname (can change anytime)
	void setDisplayName(const std::string& name) { m_displayName = name; }
	std::string getDisplayName() const { return m_displayName.empty() ? m_playerName : m_displayName; }
	
	// Alternative nicknames
	void addNickname(const std::string& nickname);
	void removeNickname(const std::string& nickname);
	std::vector<std::string> getNicknames() const { return m_nicknames; }
	
	// ═══════════════════════════════════════════════════════════
	// SKIN MANAGEMENT
	// ═══════════════════════════════════════════════════════════
	
	void setSkin(const PlayerSkin& skin);
	PlayerSkin getCurrentSkin() const { return m_currentSkin; }
	
	// Load skin from file
	bool loadSkin(const std::string& filePath);
	
	// Save current skin
	bool saveSkin(const std::string& filePath);
	
	// Skin library
	void addSkinToLibrary(const PlayerSkin& skin);
	std::vector<PlayerSkin> getSkinLibrary() const { return m_skinLibrary; }
	bool selectSkinFromLibrary(const std::string& skinId);
	
	// ═══════════════════════════════════════════════════════════
	// STATISTICS
	// ═══════════════════════════════════════════════════════════
	
	PlayerStatistics& getStatistics() { return m_statistics; }
	const PlayerStatistics& getStatistics() const { return m_statistics; }
	
	// Increment stats
	void incrementStat(const std::string& stat, int amount = 1);
	int getStat(const std::string& stat) const;
	
	// ═══════════════════════════════════════════════════════════
	// PREFERENCES
	// ═══════════════════════════════════════════════════════════
	
	PlayerPreferences& getPreferences() { return m_preferences; }
	const PlayerPreferences& getPreferences() const { return m_preferences; }
	
	void setPreference(const std::string& key, const std::string& value);
	std::string getPreference(const std::string& key, const std::string& defaultValue = "") const;
	
	// ═══════════════════════════════════════════════════════════
	// PERSISTENCE
	// ═══════════════════════════════════════════════════════════
	
	bool save();
	bool load();
	
	std::string getProfilePath() const;
	
private:
	std::string m_playerId;       // Unique UUID
	std::string m_playerName;     // Account name
	std::string m_displayName;    // Nickname
	std::vector<std::string> m_nicknames;
	
	PlayerSkin m_currentSkin;
	std::vector<PlayerSkin> m_skinLibrary;
	
	PlayerStatistics m_statistics;
	PlayerPreferences m_preferences;
	
	bool m_modified = false;
};

// ════════════════════════════════════════════════════════════════
// PROFILE MANAGER
// ════════════════════════════════════════════════════════════════

class ProfileManager
{
public:
	static ProfileManager& getInstance()
	{
		static ProfileManager instance;
		return instance;
	}
	
	// ═══════════════════════════════════════════════════════════
	// PROFILE MANAGEMENT
	// ═══════════════════════════════════════════════════════════
	
	// Create new profile
	std::shared_ptr<PlayerProfile> createProfile(const std::string& playerName);
	
	// Load existing profile
	std::shared_ptr<PlayerProfile> loadProfile(const std::string& playerId);
	
	// Get current local player profile
	std::shared_ptr<PlayerProfile> getLocalProfile();
	
	// Set local player profile
	void setLocalProfile(std::shared_ptr<PlayerProfile> profile);
	
	// ═══════════════════════════════════════════════════════════
	// NETWORK PROFILES (for multiplayer)
	// ═══════════════════════════════════════════════════════════
	
	// Cache remote player profile
	void cacheRemoteProfile(const std::string& playerId, const PlayerProfile& profile);
	
	// Get cached remote profile
	std::shared_ptr<PlayerProfile> getRemoteProfile(const std::string& playerId);
	
	// Clear remote profile cache
	void clearRemoteProfiles();
	
	// ═══════════════════════════════════════════════════════════
	// PROFILE QUERIES
	// ═══════════════════════════════════════════════════════════
	
	std::vector<std::string> getAllProfileIds() const;
	
	bool profileExists(const std::string& playerId) const;
	
	// ═══════════════════════════════════════════════════════════
	// DEFAULT SKINS
	// ═══════════════════════════════════════════════════════════
	
	static PlayerSkin getSteveSkin();
	static PlayerSkin getAlexSkin();
	
	// Get skins from resource packs
	std::vector<PlayerSkin> getAvailableSkins();
	
	// ═══════════════════════════════════════════════════════════
	// PROFILE DIRECTORY
	// ═══════════════════════════════════════════════════════════
	
	static std::string getProfileDirectory()
	{
		return "/storage/emulated/0/games/com.mojang/players/";
	}
	
	static std::string getSkinDirectory()
	{
		return "/storage/emulated/0/games/com.mojang/skins/";
	}
	
private:
	ProfileManager() {}
	
	std::shared_ptr<PlayerProfile> m_localProfile;
	std::map<std::string, std::shared_ptr<PlayerProfile>> m_remoteProfiles;
	
	std::string generatePlayerId();
};

// ════════════════════════════════════════════════════════════════
// CONVENIENCE FUNCTIONS
// ════════════════════════════════════════════════════════════════

inline std::shared_ptr<PlayerProfile> getLocalPlayerProfile()
{
	return ProfileManager::getInstance().getLocalProfile();
}

inline std::string getPlayerName()
{
	auto profile = getLocalPlayerProfile();
	return profile ? profile->getPlayerName() : "Player";
}

inline std::string getDisplayName()
{
	auto profile = getLocalPlayerProfile();
	return profile ? profile->getDisplayName() : "Player";
}

inline void setDisplayName(const std::string& name)
{
	auto profile = getLocalPlayerProfile();
	if (profile)
	{
		profile->setDisplayName(name);
		profile->save();
	}
}

inline PlayerSkin getCurrentSkin()
{
	auto profile = getLocalPlayerProfile();
	return profile ? profile->getCurrentSkin() : ProfileManager::getSteveSkin();
}
