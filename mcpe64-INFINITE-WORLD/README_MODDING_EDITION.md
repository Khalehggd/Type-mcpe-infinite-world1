# 🎮 Minecraft PE 0.6.1 - Ultimate Modding Edition

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![License](https://img.shields.io/badge/license-BSD--1--Clause-blue)]()
[![Platform](https://img.shields.io/badge/platform-Android-green)]()

**The most moddable version of Minecraft Pocket Edition ever created!**

Transforms Minecraft PE 0.6.1 into a fully moddable platform with modern Bedrock features, complete mod API, and unrestricted access to phone capabilities.

---

## ✨ Features

### 🌍 **Infinite World Generation**
- No more 256×256 block limit
- Unlimited terrain in all horizontal directions
- Chunks generate on-demand

### 📦 **Resource & Behavior Packs**
- Modern Bedrock-style pack system
- Custom textures, sounds, models
- Custom entities, items, blocks
- Full JSON/JavaScript support

### 🔧 **Complete Mod API**
- Hook any game event
- Unrestricted game access
- C++ plugin system
- No obstacles, full freedom

### 🌌 **Custom Dimensions**
- Create unlimited dimensions
- Custom world generation
- Portal system
- Full environment control

### 🎤 **Microphone Access** ⭐
- Real-time audio recording
- Voice chat system
- Voice activation detection
- Full audio processing

### 📁 **File & Storage Access**
- Full file system access
- Read/write anywhere
- ZIP compression
- File watching

### 🔔 **Notification System**
- In-game notifications
- System notifications
- Achievement system
- Toast messages

### 🌐 **LAN Multiplayer**
- WiFi hosting/joining
- Automatic server discovery
- Player management
- Custom networking

### 👤 **Player Profiles**
- Custom names & nicknames
- Skin system
- Statistics tracking
- Preferences

### ⚙️ **Permission Manager**
- Android runtime permissions
- Microphone, storage, network
- Camera, notifications
- Permission callbacks

---

## 🚀 Quick Start

### Option 1: Build with GitHub Actions (Recommended)

1. **Fork this repository**
2. **Go to Actions tab**
3. **Click "Build Minecraft PE APK"**
4. **Click "Run workflow"**
5. **Wait 15-20 minutes**
6. **Download APK from Artifacts**

### Option 2: Build Locally

#### Requirements:
- Android NDK r14b
- Android SDK Build Tools
- Java JDK 8+

#### Windows:
```powershell
.\build.ps1
```

#### Linux:
```bash
./build-linux.sh
```

APK will be in `build/` directory.

---

## 📖 Creating Your First Mod

1. **Create a file** in `src/mods/MyMod.cpp`:

```cpp
#include "../modapi/ModAPI.hpp"

class MyFirstMod : public IMod
{
public:
    void onLoad() override
    {
        notify("My First Mod loaded!");
    }
    
    void onEnable() override
    {
        notifySuccess("Mod enabled!");
    }
    
    void onTick(Level* level) override
    {
        // Called every frame
    }
    
    ModInfo getInfo() const override
    {
        ModInfo info;
        info.id = "myfirstmod";
        info.name = "My First Mod";
        info.version = "1.0.0";
        info.author = "Your Name";
        return info;
    }
};

REGISTER_MOD(MyFirstMod);
```

2. **Rebuild the APK**
3. **Install and play!**

---

## 🎤 Voice Chat Example

```cpp
#include "../modapi/MicrophoneHandler.hpp"
#include "../modapi/LANMultiplayer.hpp"

class VoiceChatMod : public IMod
{
    void onLoad() override
    {
        // Request microphone permission
        requestMicrophonePermission([this](Permission p, PermissionStatus s) {
            if (s == PermissionStatus::GRANTED)
            {
                startVoiceChat();
            }
        });
    }
    
    void startVoiceChat()
    {
        auto& vc = VoiceChat::getInstance();
        vc.startVoiceChat();
        notify("Voice chat enabled!");
    }
    
    ModInfo getInfo() const override
    {
        ModInfo info;
        info.id = "voicechat";
        info.name = "Voice Chat";
        info.requiresMicrophone = true;
        info.requiresNetwork = true;
        return info;
    }
};

REGISTER_MOD(VoiceChatMod);
```

---

## 📚 Documentation

- **[Mod API Documentation](MOD_API_DOCUMENTATION.txt)** - Complete API reference
- **[Feature Summary](COMPLETE_FEATURE_SUMMARY.txt)** - All features explained
- **[Build Instructions](BUILD_INSTRUCTIONS.txt)** - How to compile

---

## 🏗️ Project Structure

```
mcpe64-INFINITE-WORLD/
├── src/
│   ├── modapi/              # Mod API framework
│   │   ├── ModAPI.hpp
│   │   ├── MicrophoneHandler.hpp
│   │   ├── LANMultiplayer.hpp
│   │   ├── PlayerProfile.hpp
│   │   └── ...
│   │
│   ├── mods/                # Your mods go here!
│   │   └── VoiceChatMod.cpp
│   │
│   └── world/level/
│       └── Level.cpp        # Modified for infinite world
│
├── project/android/
│   └── AndroidManifest_MODDED.xml
│
├── .github/workflows/
│   └── build.yml            # GitHub Actions build
│
└── README.md
```

---

## 🔧 Mod API Features

### Hook System
```cpp
// Hook block placement
ModManager::getInstance().registerHook(
    HookType::BLOCK_PLACE,
    "mymod",
    [](void* eventData) {
        // Custom logic here
        return true; // Allow event
    }
);
```

### Custom Dimensions
```cpp
DimensionBuilder("mymod:sky")
    .setName("Sky Islands")
    .setSkyColor(0.5f, 0.8f, 1.0f)
    .setHeight(0, 256)
    .build(generatorFactory);
```

### File Operations
```cpp
// Read file
string data = readFile("/sdcard/mydata.txt");

// Write file
writeFile("/sdcard/mydata.txt", "content");

// List files
auto files = listFiles("/sdcard/mods/");
```

### LAN Multiplayer
```cpp
// Host server
hostLANServer("My Server", 8);

// Find servers
findLANServers();

// Send chat
sendChat("Hello everyone!");
```

---

## 📱 Android Permissions

The modded APK requests these permissions:

✅ **Microphone** - Voice chat, audio recording  
✅ **Storage** - File access, world saving  
✅ **Network** - LAN multiplayer, internet  
✅ **Notifications** - Achievements, alerts  
✅ **Camera** - Future features  

All permissions are requested at runtime when needed.

---

## 🎯 What Makes This Special?

| Feature | Original PE 0.6.1 | This Version |
|---------|------------------|--------------|
| World Size | 256×256 blocks | **Infinite** ✨ |
| Mods | ❌ None | **Full API** ✨ |
| Voice Chat | ❌ No | **Yes!** ✨ |
| Custom Dimensions | ❌ No | **Yes!** ✨ |
| Resource Packs | ❌ No | **Yes!** ✨ |
| LAN Multiplayer | ✅ Basic | **Enhanced** ✨ |
| File Access | ⚠️ Limited | **Full** ✨ |
| Modding Freedom | ❌ None | **Unrestricted** ✨ |

---

## 🤝 Contributing

1. Fork the repository
2. Create your mod in `src/mods/`
3. Test thoroughly
4. Submit a pull request

---

## 📜 License

This project is licensed under the BSD 1-Clause License.

Original Minecraft PE decompilation by iProgramInCpp.

---

## ⚠️ Disclaimer

This is a decompilation and modification of Minecraft Pocket Edition 0.6.1 for educational and modding purposes. Minecraft is owned by Mojang Studios.

**For educational use only.**

---

## 🎮 Credits

- **Original Decompilation**: iProgramInCpp
- **Modding Framework**: Ultimate Modding Edition Team
- **Infinite World Mod**: Community contribution
- **Mod API Design**: Open source community

---

## 📞 Support

- **Issues**: Use GitHub Issues
- **Discussions**: GitHub Discussions
- **Documentation**: See `MOD_API_DOCUMENTATION.txt`

---

## 🌟 Star this repo if you find it useful!

**Happy Modding! 🎮🔥**
