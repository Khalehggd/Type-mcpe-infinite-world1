# 🎮 Minecraft PE 0.6.1 - Ultimate Modding Edition

**The Most Advanced Minecraft PE Mod Ever Created**

---

## 🌟 Features

### ✅ What's Included:

1. **🌍 Infinite World Generation** - No more 256×256 limits!
2. **📦 Resource Pack System** - Load custom textures, sounds, models
3. **⚙️ Behavior Pack System** - Modify game logic without coding
4. **🎤 Full Microphone Access** - Voice chat, recording, sound detection
5. **📁 Complete File Access** - Read/write files anywhere on device
6. **🌐 LAN Multiplayer** - Host/join games over WiFi
7. **🎨 Player Profiles** - Custom names, nicknames, skins
8. **🔔 Notification System** - In-game and Android notifications
9. **🌌 Custom Dimensions** - Add unlimited custom dimensions with portals
10. **🔧 Complete Mod API** - Full unrestricted access to game systems

---

## 🚀 Quick Start

### For Players:

1. Install the APK
2. Grant permissions when asked (microphone, storage, etc.)
3. Create a new world - it will be INFINITE!
4. Enjoy enhanced features!

### For Mod Developers:

See `MODDING_GUIDE.md` for complete API documentation!

---

## 📖 Features Explained

### 🌍 Infinite World Generation

**Before:** 256×256 block limit  
**After:** Unlimited terrain in all directions!

The game now generates chunks on-demand as you explore. No boundaries!

**Files Modified:**
- `src/world/level/Level.cpp` - Removed X/Z boundary checks

---

### 🎤 Microphone System

**Full voice support!**

```cpp
#include "modapi/MicrophoneHandler.hpp"

// Start recording
MicrophoneHandler::getInstance().startRecording();

// Get volume level
float volume = getMicrophoneVolume();

// Voice chat
VoiceChat::getInstance().startVoiceChat();
```

**Features:**
- Real-time audio recording
- Volume level detection
- Voice activation detection
- Noise suppression
- Echo cancellation
- Voice chat system

**Permissions Required:**
- `RECORD_AUDIO` - Auto-requested on first use

---

### 📦 Resource Packs

Load custom textures, sounds, and models!

**Directory Structure:**
```
/storage/emulated/0/games/com.mojang/resource_packs/
└── MyPack/
    ├── manifest.json
    ├── textures/
    │   ├── blocks/
    │   └── items/
    ├── sounds/
    └── models/
```

**Usage:**
```cpp
#include "modapi/ResourcePackManager.hpp"

auto& manager = ResourcePackManager::getInstance();
manager.scanForPacks(ResourcePackManager::getDefaultPackPath());
manager.enablePack("pack-uuid-here");
manager.loadAllPacks();
```

---

### ⚙️ Behavior Packs

Modify game mechanics without C++ code!

**Features:**
- Custom entities
- Custom items
- Custom blocks
- Recipes
- Loot tables

**Example - Custom Entity:**
```cpp
EntityBehavior zombie;
zombie.identifier = "mymod:super_zombie";
zombie.name = "Super Zombie";
zombie.health = 50.0f;
zombie.speed = 0.5f;

BehaviorPackManager::getInstance().registerBehavior(zombie);
```

---

### 🌐 LAN Multiplayer

Play with friends on the same WiFi!

**Host a Server:**
```cpp
#include "modapi/LANMultiplayer.hpp"

LANMultiplayer::getInstance().startHost("My Server", 8);
```

**Join a Server:**
```cpp
LANMultiplayer::getInstance().startServerDiscovery();
auto servers = LANMultiplayer::getInstance().getDiscoveredServers();

// Join first server found
if (!servers.empty())
    LANMultiplayer::getInstance().joinServer(servers[0].ipAddress);
```

**Features:**
- Automatic server discovery
- Host/join via WiFi
- Custom packet system
- Player management
- Chat system

---

### 🎨 Player Profiles

**Custom identities!**

```cpp
#include "modapi/PlayerProfile.hpp"

auto profile = getLocalPlayerProfile();
profile->setDisplayName("MyNickname");
profile->loadSkin("/path/to/skin.png");
profile->save();
```

**Features:**
- Player names
- Nicknames
- Custom skins (64×64 or 64×32)
- Statistics tracking
- Preferences

---

### 🔔 Notifications

**In-game alerts and achievements!**

```cpp
#include "modapi/NotificationManager.hpp"

// Simple notification
notify("Hello World!");

// Achievement
NotificationManager::getInstance().unlockAchievement("first_diamond");

// Custom notification
NotificationBuilder()
    .setTitle("Warning!")
    .setMessage("Low health!")
    .setType(NotificationType::WARNING)
    .setDuration(5.0f)
    .show();
```

---

### 🌌 Custom Dimensions

**Add unlimited dimensions!**

```cpp
#include "modapi/DimensionRegistry.hpp"

// Create void dimension
DimensionBuilder("mymod:void")
    .setName("The Void")
    .setSkyLight(false)
    .setFogColor(0, 0, 0)
    .build(myVoidGenerator);

// Teleport player
DimensionRegistry::getInstance().teleportPlayer(player, 100, 0, 64, 0);
```

**Features:**
- Unlimited custom dimensions
- Portal system
- Custom world generation per dimension
- Custom sky/fog colors
- Custom physics

---

### 📁 File Access

**Complete storage access!**

```cpp
#include "modapi/StorageManager.hpp"

// Read file
std::string content = readFile("/path/to/file.txt");

// Write file
writeFile("/path/to/output.txt", "Hello!");

// List files
auto files = listFiles("/storage/emulated/0/Download");

// Create ZIP
StorageManager::getInstance().createZip("backup.zip", {"world1", "world2"});
```

---

## 🔧 Mod API Overview

### Core Systems:

| System | Header | Description |
|--------|--------|-------------|
| Mod API | `ModAPI.hpp` | Core mod registration & hooks |
| Resource Packs | `ResourcePackManager.hpp` | Texture/sound loading |
| Behavior Packs | `BehaviorPackManager.hpp` | Game logic modification |
| Permissions | `PermissionManager.hpp` | Android permissions |
| Microphone | `MicrophoneHandler.hpp` | Audio recording |
| LAN Multiplayer | `LANMultiplayer.hpp` | Network play |
| Player Profiles | `PlayerProfile.hpp` | Identity & skins |
| Notifications | `NotificationManager.hpp` | Alerts & achievements |
| Dimensions | `DimensionRegistry.hpp` | Custom worlds |
| Storage | `StorageManager.hpp` | File operations |

---

## 📝 Creating Your First Mod

**1. Create mod class:**

```cpp
#include "modapi/ModAPI.hpp"

class MyFirstMod : public IMod
{
public:
    void onLoad() override
    {
        logInfo("MyFirstMod loading!");
    }
    
    void onEnable() override
    {
        logInfo("MyFirstMod enabled!");
        
        // Register hook
        ModManager::getInstance().registerHook(
            HookType::BLOCK_PLACE,
            "myfirstmod",
            [](void* event) -> bool {
                notify("Block placed!");
                return true; // Allow the block placement
            }
        );
    }
    
    void onDisable() override
    {
        logInfo("MyFirstMod disabled!");
    }
    
    ModInfo getInfo() const override
    {
        ModInfo info;
        info.id = "myfirstmod";
        info.name = "My First Mod";
        info.version = "1.0.0";
        info.author = "You!";
        info.description = "My awesome mod!";
        return info;
    }
};

// Register the mod
REGISTER_MOD(MyFirstMod);
```

**2. Compile and test!**

---

## ⚙️ Building from Source

### Requirements:

- Android NDK r14b
- Android SDK Build Tools
- Java JDK 11+
- CMake (optional)

### Build Steps:

**Windows:**
```powershell
.\build.ps1
```

**Linux:**
```bash
./build-linux.sh
```

**Output:** APK file in `build/` directory

---

## 🔐 Permissions

This app requests the following permissions:

| Permission | Why Needed |
|------------|------------|
| `RECORD_AUDIO` | Voice chat, sound recording |
| `READ/WRITE_EXTERNAL_STORAGE` | Save worlds, mods, resource packs |
| `INTERNET` | LAN multiplayer (local network only) |
| `ACCESS_WIFI_STATE` | Discover LAN servers |
| `POST_NOTIFICATIONS` | Show achievements & alerts |

**All permissions are optional!** The game works without them, but features will be limited.

---

## 🐛 Troubleshooting

### Game crashes on startup
- Check logcat: `adb logcat | grep MinecraftPE`
- Ensure storage permission is granted
- Clear app data and reinstall

### Microphone not working
- Grant microphone permission in Settings
- Check if another app is using microphone
- Test with voice recorder app first

### LAN multiplayer not finding servers
- Ensure both devices on same WiFi
- Check firewall settings
- Try manual IP connection

### Infinite world not generating
- Create a NEW world (old worlds still limited)
- Check coordinates with F3
- Walk past 256 blocks in any direction

---

## 📄 License

Based on the Minecraft PE 0.6.1 decompilation project.  
Modding framework: BSD 1-Clause License

---

## 🤝 Contributing

Want to add features? Create a pull request!

**Ideas for future mods:**
- Custom mobs
- Magic system
- Tech tree
- Quests
- More dimensions
- Custom biomes

---

## 🎉 Credits

**Original Decompilation:** iProgramInCpp  
**Infinite World Mod:** Created for Khalil  
**Modding Framework:** Ultimate Edition  

---

## 📞 Support

Having issues? Check:
- `MODDING_GUIDE.md` - Complete API docs
- `EXAMPLES.md` - Example mods
- GitHub Issues - Report bugs

---

**Enjoy unlimited Minecraft PE with full modding support!** 🚀
