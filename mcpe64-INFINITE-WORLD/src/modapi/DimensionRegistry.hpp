/********************************************************************
	Minecraft: Pocket Edition - Custom Dimension System
	DimensionRegistry.hpp - Add custom dimensions with portals
	
	Allows mods to create infinite custom dimensions
********************************************************************/

#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

// Forward declarations
class Level;
class ChunkSource;
class Dimension;

// ════════════════════════════════════════════════════════════════
// DIMENSION DEFINITION
// ════════════════════════════════════════════════════════════════

struct DimensionInfo
{
	int dimensionId;              // Unique ID (0=overworld, -1=nether, 1=end)
	std::string name;             // Display name
	std::string identifier;       // Mod identifier (e.g., "mymod:void_dimension")
	
	// World generation
	bool hasSkyLight = true;      // Does sun/moon affect lighting?
	bool hasCeiling = false;      // Bedrock ceiling like Nether?
	int maxHeight = 128;          // Build height
	int minHeight = 0;            // Minimum Y level
	
	// Environment
	float fogColor[3] = {0.5f, 0.7f, 1.0f};  // RGB fog color
	float skyColor[3] = {0.5f, 0.7f, 1.0f};   // RGB sky color
	bool hasWeather = true;       // Rain/snow?
	bool hasDay = true;           // Day/night cycle?
	
	// Spawn behavior
	bool canRespawnHere = false;  // Can players respawn here?
	int spawnX = 0;
	int spawnY = 64;
	int spawnZ = 0;
};

// ════════════════════════════════════════════════════════════════
// PORTAL DEFINITION
// ════════════════════════════════════════════════════════════════

struct PortalInfo
{
	std::string name;
	int fromDimension;            // Source dimension
	int toDimension;              // Target dimension
	int portalBlockId;            // Block that creates portal
	int frameBlockId;             // Block for portal frame
	
	// Portal size
	int minWidth = 2;
	int minHeight = 3;
	int maxWidth = 21;
	int maxHeight = 21;
	
	// Activation
	bool requiresIgnition = false;  // Like Nether portal
	int ignitionItemId = 0;         // Item to ignite with
	
	// Teleportation
	float coordinateScale = 8.0f;   // Nether = 8:1 ratio
	
	// Callbacks
	std::function<void(void* player, int x, int y, int z)> onTeleport;
};

// ════════════════════════════════════════════════════════════════
// CUSTOM CHUNK SOURCE FACTORY
// ════════════════════════════════════════════════════════════════

typedef std::function<ChunkSource*(Level*, int dimensionId)> DimensionGeneratorFactory;

// ════════════════════════════════════════════════════════════════
// DIMENSION REGISTRY
// ════════════════════════════════════════════════════════════════

class DimensionRegistry
{
public:
	static DimensionRegistry& getInstance()
	{
		static DimensionRegistry instance;
		return instance;
	}
	
	// ═══════════════════════════════════════════════════════════
	// DIMENSION REGISTRATION
	// ═══════════════════════════════════════════════════════════
	
	bool registerDimension(const DimensionInfo& info, DimensionGeneratorFactory generator);
	
	bool unregisterDimension(int dimensionId);
	
	bool isDimensionRegistered(int dimensionId) const;
	
	// ═══════════════════════════════════════════════════════════
	// PORTAL REGISTRATION
	// ═══════════════════════════════════════════════════════════
	
	void registerPortal(const PortalInfo& portal);
	
	PortalInfo* getPortal(int fromDim, int toDim);
	
	// ═══════════════════════════════════════════════════════════
	// DIMENSION QUERIES
	// ═══════════════════════════════════════════════════════════
	
	DimensionInfo* getDimensionInfo(int dimensionId);
	
	std::vector<DimensionInfo> getAllDimensions() const;
	
	int getNextFreeDimensionId() const;
	
	// ═══════════════════════════════════════════════════════════
	// DIMENSION CREATION
	// ═══════════════════════════════════════════════════════════
	
	Dimension* createDimension(Level* level, int dimensionId);
	
	ChunkSource* createChunkSource(Level* level, int dimensionId);
	
	// ═══════════════════════════════════════════════════════════
	// TELEPORTATION
	// ═══════════════════════════════════════════════════════════
	
	// Teleport player to dimension
	void teleportPlayer(void* player, int dimensionId, int x, int y, int z);
	
	// Find or create portal in target dimension
	void findOrCreatePortal(int fromDim, int toDim, int x, int y, int z, int& outX, int& outY, int& outZ);
	
	// ═══════════════════════════════════════════════════════════
	// BUILT-IN DIMENSIONS
	// ═══════════════════════════════════════════════════════════
	
	static const int DIMENSION_OVERWORLD = 0;
	static const int DIMENSION_NETHER = -1;
	static const int DIMENSION_END = 1;
	
	// Custom dimensions start from ID 100
	static const int DIMENSION_CUSTOM_START = 100;
	
private:
	DimensionRegistry() {}
	
	std::map<int, DimensionInfo> m_dimensions;
	std::map<int, DimensionGeneratorFactory> m_generators;
	std::vector<PortalInfo> m_portals;
	
	int m_nextCustomDimensionId = DIMENSION_CUSTOM_START;
};

// ════════════════════════════════════════════════════════════════
// DIMENSION BUILDER - Fluent API for creating dimensions
// ════════════════════════════════════════════════════════════════

class DimensionBuilder
{
public:
	DimensionBuilder(const std::string& identifier)
	{
		m_info.identifier = identifier;
		m_info.name = identifier;
		m_info.dimensionId = DimensionRegistry::getInstance().getNextFreeDimensionId();
	}
	
	DimensionBuilder& setName(const std::string& name)
	{
		m_info.name = name;
		return *this;
	}
	
	DimensionBuilder& setSkyLight(bool enabled)
	{
		m_info.hasSkyLight = enabled;
		return *this;
	}
	
	DimensionBuilder& setCeiling(bool enabled)
	{
		m_info.hasCeiling = enabled;
		return *this;
	}
	
	DimensionBuilder& setHeight(int min, int max)
	{
		m_info.minHeight = min;
		m_info.maxHeight = max;
		return *this;
	}
	
	DimensionBuilder& setFogColor(float r, float g, float b)
	{
		m_info.fogColor[0] = r;
		m_info.fogColor[1] = g;
		m_info.fogColor[2] = b;
		return *this;
	}
	
	DimensionBuilder& setSkyColor(float r, float g, float b)
	{
		m_info.skyColor[0] = r;
		m_info.skyColor[1] = g;
		m_info.skyColor[2] = b;
		return *this;
	}
	
	DimensionBuilder& setWeather(bool enabled)
	{
		m_info.hasWeather = enabled;
		return *this;
	}
	
	DimensionBuilder& setDayNightCycle(bool enabled)
	{
		m_info.hasDay = enabled;
		return *this;
	}
	
	DimensionBuilder& setCanRespawn(bool enabled)
	{
		m_info.canRespawnHere = enabled;
		return *this;
	}
	
	DimensionBuilder& setSpawnPoint(int x, int y, int z)
	{
		m_info.spawnX = x;
		m_info.spawnY = y;
		m_info.spawnZ = z;
		return *this;
	}
	
	bool build(DimensionGeneratorFactory generator)
	{
		return DimensionRegistry::getInstance().registerDimension(m_info, generator);
	}
	
	int getDimensionId() const { return m_info.dimensionId; }
	
private:
	DimensionInfo m_info;
};

// ════════════════════════════════════════════════════════════════
// EXAMPLE CUSTOM DIMENSIONS
// ════════════════════════════════════════════════════════════════

// Void dimension (empty space)
inline DimensionInfo createVoidDimension()
{
	DimensionInfo info;
	info.dimensionId = 100;
	info.name = "The Void";
	info.identifier = "minecraft:void";
	info.hasSkyLight = false;
	info.hasCeiling = false;
	info.fogColor[0] = 0.0f; info.fogColor[1] = 0.0f; info.fogColor[2] = 0.0f;
	info.skyColor[0] = 0.0f; info.skyColor[1] = 0.0f; info.skyColor[2] = 0.0f;
	return info;
}

// Sky dimension (floating islands)
inline DimensionInfo createSkyDimension()
{
	DimensionInfo info;
	info.dimensionId = 101;
	info.name = "Sky Islands";
	info.identifier = "minecraft:sky";
	info.hasSkyLight = true;
	info.maxHeight = 256;
	info.fogColor[0] = 0.7f; info.fogColor[1] = 0.9f; info.fogColor[2] = 1.0f;
	info.skyColor[0] = 0.5f; info.skyColor[1] = 0.8f; info.skyColor[2] = 1.0f;
	return info;
}

// Cave dimension (underground)
inline DimensionInfo createCaveDimension()
{
	DimensionInfo info;
	info.dimensionId = 102;
	info.name = "Deep Caves";
	info.identifier = "minecraft:caves";
	info.hasSkyLight = false;
	info.hasCeiling = true;
	info.hasWeather = false;
	info.hasDay = false;
	info.fogColor[0] = 0.1f; info.fogColor[1] = 0.1f; info.fogColor[2] = 0.1f;
	return info;
}
