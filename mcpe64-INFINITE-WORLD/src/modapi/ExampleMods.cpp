/********************************************************************
	Example Mods - Demonstrating the Modding API
	
	Copy these examples to create your own mods!
********************************************************************/

#include "modapi/ModAPI.hpp"
#include "modapi/MicrophoneHandler.hpp"
#include "modapi/LANMultiplayer.hpp"
#include "modapi/NotificationManager.hpp"
#include "modapi/DimensionRegistry.hpp"
#include "modapi/PlayerProfile.hpp"
#include "modapi/StorageManager.hpp"

// ════════════════════════════════════════════════════════════════
// EXAMPLE 1: Voice Chat Mod
// ════════════════════════════════════════════════════════════════

class VoiceChatMod : public IMod
{
public:
	void onLoad() override
	{
		logInfo("Voice Chat Mod loading...");
	}
	
	void onEnable() override
	{
		// Request microphone permission
		if (!hasMicrophonePermission())
		{
			requestMicrophonePermission([](Permission perm, PermissionStatus status) {
				if (status == PermissionStatus::GRANTED)
				{
					notify("Microphone access granted! Press V to talk!");
				}
			});
		}
		
		// Set up voice detection
		MicrophoneHandler::getInstance().enableVoiceActivation(0.15f);
		
		// Callback when someone speaks
		MicrophoneHandler::getInstance().setAudioCallback([this](const AudioData& audio) {
			if (audio.isSpeaking())
			{
				// Broadcast voice data to other players
				if (LANMultiplayer::getInstance().isConnected())
				{
					std::vector<uint8_t> audioBytes(
						reinterpret_cast<const uint8_t*>(audio.samples.data()),
						reinterpret_cast<const uint8_t*>(audio.samples.data() + audio.samples.size())
					);
					
					LANMultiplayer::getInstance().broadcastPacket(
						PacketType::CUSTOM_DATA,
						audioBytes
					);
				}
			}
		});
		
		logInfo("Voice Chat Mod enabled!");
	}
	
	void onDisable() override
	{
		MicrophoneHandler::getInstance().stopRecording();
	}
	
	ModInfo getInfo() const override
	{
		ModInfo info;
		info.id = "voicechat";
		info.name = "Voice Chat";
		info.version = "1.0.0";
		info.author = "Example";
		info.description = "Real-time voice chat in multiplayer!";
		info.requiresMicrophone = true;
		info.requiresNetwork = true;
		return info;
	}
};

// ════════════════════════════════════════════════════════════════
// EXAMPLE 2: Custom Dimension Mod
// ════════════════════════════════════════════════════════════════

class SkyDimensionMod : public IMod
{
public:
	void onLoad() override
	{
		// Create custom sky dimension
		DimensionBuilder("skymod:floating_islands")
			.setName("Sky Islands")
			.setSkyLight(true)
			.setHeight(0, 256)
			.setSkyColor(0.5f, 0.8f, 1.0f)
			.setFogColor(0.7f, 0.9f, 1.0f)
			.setWeather(true)
			.setCanRespawn(true)
			.build([](Level* level, int dimId) -> ChunkSource* {
				// Custom chunk generator for floating islands
				// (Implementation would generate sky islands)
				return level->getDimension()->createRandomLevelSource();
			});
		
		// Create portal
		PortalInfo portal;
		portal.name = "Sky Portal";
		portal.fromDimension = 0; // Overworld
		portal.toDimension = 100; // Sky dimension
		portal.portalBlockId = 49; // Obsidian
		portal.frameBlockId = 41; // Gold blocks
		portal.requiresIgnition = true;
		portal.ignitionItemId = 259; // Flint & steel
		
		DimensionRegistry::getInstance().registerPortal(portal);
		
		notify("Sky Dimension added! Build a gold portal and ignite it!");
	}
	
	void onEnable() override {}
	void onDisable() override {}
	
	ModInfo getInfo() const override
	{
		ModInfo info;
		info.id = "skydimension";
		info.name = "Sky Islands Dimension";
		info.version = "1.0.0";
		info.author = "Example";
		info.description = "Adds a sky dimension with floating islands!";
		return info;
	}
};

// ════════════════════════════════════════════════════════════════
// EXAMPLE 3: Auto-Save Mod
// ════════════════════════════════════════════════════════════════

class AutoSaveMod : public IMod
{
private:
	double m_lastSaveTime = 0.0;
	const double SAVE_INTERVAL = 300.0; // 5 minutes
	
public:
	void onLoad() override {}
	
	void onEnable() override
	{
		notify("Auto-save enabled! World saves every 5 minutes.");
	}
	
	void onTick(Level* level) override
	{
		// Get current time (would need to implement this)
		double currentTime = 0.0; // level->getTime();
		
		if (currentTime - m_lastSaveTime >= SAVE_INTERVAL)
		{
			// Save world
			if (level)
			{
				// level->save();
				
				NotificationBuilder()
					.setTitle("World Saved")
					.setMessage("Your progress has been saved!")
					.setType(NotificationType::SUCCESS)
					.setDuration(2.0f)
					.show();
				
				m_lastSaveTime = currentTime;
			}
		}
	}
	
	void onDisable() override {}
	
	ModInfo getInfo() const override
	{
		ModInfo info;
		info.id = "autosave";
		info.name = "Auto-Save";
		info.version = "1.0.0";
		info.author = "Example";
		info.description = "Automatically saves world every 5 minutes";
		return info;
	}
};

// ════════════════════════════════════════════════════════════════
// EXAMPLE 4: Player Stats Tracker
// ════════════════════════════════════════════════════════════════

class StatsTrackerMod : public IMod
{
public:
	void onLoad() override
	{
		// Hook into player events
		ModManager::getInstance().registerHook(
			HookType::BLOCK_BREAK,
			"statstracker",
			[](void* event) -> bool {
				auto profile = getLocalPlayerProfile();
				if (profile)
				{
					profile->incrementStat("blocks_mined");
					
					int total = profile->getStat("blocks_mined");
					if (total % 100 == 0)
					{
						toast("Mined " + std::to_string(total) + " blocks!");
					}
				}
				return true;
			}
		);
		
		ModManager::getInstance().registerHook(
			HookType::BLOCK_PLACE,
			"statstracker",
			[](void* event) -> bool {
				auto profile = getLocalPlayerProfile();
				if (profile)
				{
					profile->incrementStat("blocks_placed");
				}
				return true;
			}
		);
	}
	
	void onEnable() override
	{
		auto profile = getLocalPlayerProfile();
		if (profile)
		{
			int mined = profile->getStat("blocks_mined");
			int placed = profile->getStat("blocks_placed");
			
			notify("Stats: " + std::to_string(mined) + " mined, " + 
			       std::to_string(placed) + " placed");
		}
	}
	
	void onDisable() override
	{
		// Save stats
		auto profile = getLocalPlayerProfile();
		if (profile)
		{
			profile->save();
		}
	}
	
	ModInfo getInfo() const override
	{
		ModInfo info;
		info.id = "statstracker";
		info.name = "Stats Tracker";
		info.version = "1.0.0";
		info.author = "Example";
		info.description = "Tracks blocks mined and placed";
		return info;
	}
};

// ════════════════════════════════════════════════════════════════
// EXAMPLE 5: LAN Party Host Mod
// ════════════════════════════════════════════════════════════════

class LANPartyMod : public IMod
{
public:
	void onLoad() override {}
	
	void onEnable() override
	{
		// Start LAN server
		auto& lan = LANMultiplayer::getInstance();
		
		auto profile = getLocalPlayerProfile();
		std::string serverName = "Playing Minecraft!";
		if (profile)
		{
			serverName = profile->getDisplayName() + "'s World";
		}
		
		if (lan.startHost(serverName, 8))
		{
			notify("LAN server started! Friends can join now!");
			
			// Set up callbacks
			lan.setPlayerJoinCallback([](const NetworkPlayerInfo& player) {
				notifySuccess(player.playerName + " joined the game!");
			});
			
			lan.setPlayerLeaveCallback([](const NetworkPlayerInfo& player) {
				notifyWarning(player.playerName + " left the game!");
			});
			
			lan.setChatMessageCallback([](const std::string& sender, const std::string& message) {
				// Show chat message
				toast(sender + ": " + message);
			});
		}
		else
		{
			notifyError("Failed to start LAN server!");
		}
	}
	
	void onDisable() override
	{
		LANMultiplayer::getInstance().stopHost();
		notify("LAN server stopped.");
	}
	
	ModInfo getInfo() const override
	{
		ModInfo info;
		info.id = "lanparty";
		info.name = "LAN Party Host";
		info.version = "1.0.0";
		info.author = "Example";
		info.description = "Easy LAN multiplayer hosting!";
		info.requiresNetwork = true;
		return info;
	}
};

// ════════════════════════════════════════════════════════════════
// EXAMPLE 6: World Backup Mod
// ════════════════════════════════════════════════════════════════

class WorldBackupMod : public IMod
{
public:
	void onLoad() override {}
	
	void onEnable() override
	{
		// Create backup button achievement
		Achievement backupAchievement;
		backupAchievement.id = "first_backup";
		backupAchievement.name = "Safety First!";
		backupAchievement.description = "Created your first world backup";
		backupAchievement.points = 10;
		
		NotificationManager::getInstance().registerAchievement(backupAchievement);
		
		notify("World Backup Mod loaded! Say '/backup' to backup your world!");
	}
	
	void backupWorld()
	{
		std::string worldPath = StorageManager::getWorldsPath() + "current_world/";
		std::string backupPath = StorageManager::getGameDataPath() + "backups/";
		
		// Create backups directory
		StorageManager::getInstance().createDirectory(backupPath);
		
		// Create timestamped backup
		std::string timestamp = "2026-04-27"; // Would get actual timestamp
		std::string zipFile = backupPath + "backup_" + timestamp + ".zip";
		
		// Backup world files
		auto worldFiles = StorageManager::getInstance().listFiles(worldPath, true);
		std::vector<std::string> filePaths;
		for (const auto& file : worldFiles)
		{
			filePaths.push_back(file.path);
		}
		
		if (StorageManager::getInstance().createZip(zipFile, filePaths))
		{
			notifySuccess("World backed up successfully!");
			NotificationManager::getInstance().unlockAchievement("first_backup");
		}
		else
		{
			notifyError("Backup failed!");
		}
	}
	
	void onDisable() override {}
	
	ModInfo getInfo() const override
	{
		ModInfo info;
		info.id = "worldbackup";
		info.name = "World Backup";
		info.version = "1.0.0";
		info.author = "Example";
		info.description = "Backup and restore your worlds!";
		info.requiresStorage = true;
		return info;
	}
};

// ════════════════════════════════════════════════════════════════
// REGISTER ALL EXAMPLE MODS
// ════════════════════════════════════════════════════════════════

void registerExampleMods()
{
	REGISTER_MOD(VoiceChatMod);
	REGISTER_MOD(SkyDimensionMod);
	REGISTER_MOD(AutoSaveMod);
	REGISTER_MOD(StatsTrackerMod);
	REGISTER_MOD(LANPartyMod);
	REGISTER_MOD(WorldBackupMod);
	
	logInfo("All example mods registered!");
}
