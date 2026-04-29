/********************************************************************
	Minecraft: Pocket Edition - LAN Multiplayer System
	LANMultiplayer.hpp - WiFi hosting and joining
	
	Allows players on same WiFi to play together
********************************************************************/

#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

#ifdef __ANDROID__
#include <jni.h>
#endif

// ════════════════════════════════════════════════════════════════
// PLAYER INFO
// ════════════════════════════════════════════════════════════════

struct NetworkPlayerInfo
{
	std::string playerId;         // Unique player ID
	std::string playerName;       // Display name
	std::string ipAddress;        // IP address
	int ping = 0;                 // Latency in ms
	bool isHost = false;          // Is this player the host?
	
	// Connection status
	bool isConnected = true;
	double lastHeartbeat = 0.0;   // Last ping timestamp
};

// ════════════════════════════════════════════════════════════════
// SERVER INFO
// ════════════════════════════════════════════════════════════════

struct LANServerInfo
{
	std::string serverName;       // Display name
	std::string hostName;         // Host player name
	std::string ipAddress;        // Server IP
	int port = 19132;             // Server port
	
	int currentPlayers = 0;
	int maxPlayers = 8;
	
	std::string worldName;
	std::string gameMode;         // "survival" or "creative"
	
	bool requiresPassword = false;
	
	// Discovery
	double lastSeen = 0.0;        // Last discovery timestamp
	int signalStrength = 100;     // WiFi signal strength %
};

// ════════════════════════════════════════════════════════════════
// NETWORK CALLBACKS
// ════════════════════════════════════════════════════════════════

typedef std::function<void(const NetworkPlayerInfo&)> PlayerJoinCallback;
typedef std::function<void(const NetworkPlayerInfo&)> PlayerLeaveCallback;
typedef std::function<void(const std::string&, const std::string&)> ChatMessageCallback;
typedef std::function<void(const LANServerInfo&)> ServerDiscoveredCallback;

// ════════════════════════════════════════════════════════════════
// PACKET TYPES (for mod communication)
// ════════════════════════════════════════════════════════════════

enum class PacketType
{
	PLAYER_MOVE,
	BLOCK_UPDATE,
	CHAT_MESSAGE,
	PLAYER_ACTION,
	ENTITY_UPDATE,
	CUSTOM_DATA          // For mods
};

struct NetworkPacket
{
	PacketType type;
	std::string senderId;
	std::vector<uint8_t> data;
	double timestamp;
};

typedef std::function<void(const NetworkPacket&)> PacketCallback;

// ════════════════════════════════════════════════════════════════
// LAN MULTIPLAYER MANAGER
// ════════════════════════════════════════════════════════════════

class LANMultiplayer
{
public:
	static LANMultiplayer& getInstance()
	{
		static LANMultiplayer instance;
		return instance;
	}
	
#ifdef __ANDROID__
	void init(JNIEnv* env, jobject context);
#endif
	
	// ═══════════════════════════════════════════════════════════
	// HOST SERVER
	// ═══════════════════════════════════════════════════════════
	
	bool startHost(const std::string& serverName, int maxPlayers = 8, int port = 19132);
	void stopHost();
	bool isHosting() const { return m_isHosting; }
	
	// Set server password (empty = no password)
	void setServerPassword(const std::string& password);
	
	// Kick player
	void kickPlayer(const std::string& playerId, const std::string& reason = "");
	
	// Ban player
	void banPlayer(const std::string& playerId);
	void unbanPlayer(const std::string& playerId);
	std::vector<std::string> getBannedPlayers() const;
	
	// Server settings
	void setMaxPlayers(int max);
	void setServerName(const std::string& name);
	
	// ═══════════════════════════════════════════════════════════
	// JOIN SERVER
	// ═══════════════════════════════════════════════════════════
	
	bool joinServer(const std::string& ipAddress, int port = 19132, const std::string& password = "");
	void disconnect();
	bool isConnected() const { return m_isConnected; }
	
	// ═══════════════════════════════════════════════════════════
	// SERVER DISCOVERY
	// ═══════════════════════════════════════════════════════════
	
	// Start scanning for LAN servers
	void startServerDiscovery();
	void stopServerDiscovery();
	bool isDiscovering() const { return m_isDiscovering; }
	
	// Get discovered servers
	std::vector<LANServerInfo> getDiscoveredServers() const;
	
	// Set callback for when server is found
	void setServerDiscoveredCallback(ServerDiscoveredCallback callback)
	{
		m_serverDiscoveredCallback = callback;
	}
	
	// ═══════════════════════════════════════════════════════════
	// PLAYER MANAGEMENT
	// ═══════════════════════════════════════════════════════════
	
	std::vector<NetworkPlayerInfo> getConnectedPlayers() const;
	NetworkPlayerInfo* getPlayer(const std::string& playerId);
	int getPlayerCount() const;
	
	bool isPlayerHost(const std::string& playerId) const;
	std::string getHostPlayerId() const { return m_hostPlayerId; }
	
	// ═══════════════════════════════════════════════════════════
	// CALLBACKS
	// ═══════════════════════════════════════════════════════════
	
	void setPlayerJoinCallback(PlayerJoinCallback callback)
	{
		m_playerJoinCallback = callback;
	}
	
	void setPlayerLeaveCallback(PlayerLeaveCallback callback)
	{
		m_playerLeaveCallback = callback;
	}
	
	void setChatMessageCallback(ChatMessageCallback callback)
	{
		m_chatMessageCallback = callback;
	}
	
	// ═══════════════════════════════════════════════════════════
	// COMMUNICATION
	// ═══════════════════════════════════════════════════════════
	
	// Send chat message
	void sendChatMessage(const std::string& message);
	
	// Send custom packet to all players
	void broadcastPacket(PacketType type, const std::vector<uint8_t>& data);
	
	// Send custom packet to specific player
	void sendPacket(const std::string& playerId, PacketType type, const std::vector<uint8_t>& data);
	
	// Register packet handler
	void registerPacketHandler(PacketType type, PacketCallback callback);
	
	// ═══════════════════════════════════════════════════════════
	// NETWORK INFO
	// ═══════════════════════════════════════════════════════════
	
	std::string getLocalIPAddress();
	int getPing(const std::string& playerId) const;
	
	// Network stats
	struct NetworkStats
	{
		int bytesSent = 0;
		int bytesReceived = 0;
		int packetsSent = 0;
		int packetsReceived = 0;
		float uploadSpeed = 0.0f;   // KB/s
		float downloadSpeed = 0.0f; // KB/s
	};
	
	NetworkStats getNetworkStats() const { return m_stats; }
	
private:
	LANMultiplayer() {}
	
#ifdef __ANDROID__
	JNIEnv* m_env = nullptr;
	jobject m_context = nullptr;
#endif
	
	bool m_isHosting = false;
	bool m_isConnected = false;
	bool m_isDiscovering = false;
	
	std::string m_hostPlayerId;
	std::string m_localPlayerId;
	
	int m_serverPort = 19132;
	int m_maxPlayers = 8;
	std::string m_serverName;
	std::string m_serverPassword;
	
	std::map<std::string, NetworkPlayerInfo> m_players;
	std::vector<LANServerInfo> m_discoveredServers;
	std::vector<std::string> m_bannedPlayers;
	
	// Callbacks
	PlayerJoinCallback m_playerJoinCallback;
	PlayerLeaveCallback m_playerLeaveCallback;
	ChatMessageCallback m_chatMessageCallback;
	ServerDiscoveredCallback m_serverDiscoveredCallback;
	
	std::map<PacketType, PacketCallback> m_packetHandlers;
	
	NetworkStats m_stats;
	
	// Network threads
	void serverThread();
	void clientThread();
	void discoveryThread();
	
	bool m_shouldStopThreads = false;
};

// ════════════════════════════════════════════════════════════════
// CONVENIENCE FUNCTIONS
// ════════════════════════════════════════════════════════════════

inline bool hostLANServer(const std::string& name, int maxPlayers = 8)
{
	return LANMultiplayer::getInstance().startHost(name, maxPlayers);
}

inline bool joinLANServer(const std::string& ip)
{
	return LANMultiplayer::getInstance().joinServer(ip);
}

inline void findLANServers()
{
	LANMultiplayer::getInstance().startServerDiscovery();
}

inline void sendChat(const std::string& message)
{
	LANMultiplayer::getInstance().sendChatMessage(message);
}

inline int getPlayerCount()
{
	return LANMultiplayer::getInstance().getPlayerCount();
}
