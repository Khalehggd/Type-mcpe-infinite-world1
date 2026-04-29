/********************************************************************
	EXAMPLE MOD: Voice Chat with Push-to-Talk
	Demonstrates: Microphone access, LAN networking, permissions
********************************************************************/

#include "../modapi/ModAPI.hpp"
#include "../modapi/MicrophoneHandler.hpp"
#include "../modapi/LANMultiplayer.hpp"
#include "../modapi/PermissionManager.hpp"
#include "../modapi/NotificationManager.hpp"

class VoiceChatMod : public IMod
{
public:
	void onLoad() override
	{
		logInfo("[VoiceChat] Mod loading...");
		
		// Request microphone permission first
		auto& pm = PermissionManager::getInstance();
		
		if (!pm.hasPermission(Permission::MICROPHONE))
		{
			pm.requestPermissionWithRationale(
				Permission::MICROPHONE,
				"Voice chat requires microphone access",
				[this](Permission perm, PermissionStatus status) {
					if (status == PermissionStatus::GRANTED)
					{
						notify Success("Microphone permission granted!");
						initializeVoiceChat();
					}
					else
					{
						notifyError("Voice chat disabled: microphone permission denied");
					}
				}
			);
		}
		else
		{
			initializeVoiceChat();
		}
	}
	
	void onEnable() override
	{
		notifySuccess("Voice Chat Mod enabled! Hold 'V' to talk");
	}
	
	void onDisable() override
	{
		stopVoiceChat();
	}
	
	void onTick(Level* level) override
	{
		// Check if player pressed 'V' key for push-to-talk
		// (Key detection would be implemented in game input system)
		
		if (m_voiceChatEnabled)
		{
			updateVolumeIndicator();
		}
	}
	
	ModInfo getInfo() const override
	{
		ModInfo info;
		info.id = "voicechat";
		info.name = "Voice Chat";
		info.version = "1.0.0";
		info.author = "ModAPI Example";
		info.description = "Push-to-talk voice chat for multiplayer";
		info.requiresMicrophone = true;
		info.requiresNetwork = true;
		return info;
	}

private:
	bool m_voiceChatEnabled = false;
	bool m_isPushingToTalk = false;
	
	void initializeVoiceChat()
	{
		auto& mic = MicrophoneHandler::getInstance();
		auto& vc = VoiceChat::getInstance();
		
		// Configure audio settings
		AudioConfig config;
		config.sampleRate = 16000;  // Lower for voice (saves bandwidth)
		config.channelCount = 1;    // Mono
		mic.setAudioConfig(config);
		
		// Enable noise suppression for clearer voice
		mic.setNoiseSuppression(true);
		mic.setEchoCancellation(true);
		
		// Set up voice detection
		mic.setVolumeLevelCallback([this](float volume) {
			// Update volume indicator UI
			m_currentVolume = volume;
		});
		
		// Set up voice chat
		vc.setPushToTalk(true);
		vc.setVoiceChatCallback([](const std::string& playerName, const AudioData& audio) {
			// Received voice from another player
			logInfo("[VoiceChat] Received audio from %s", playerName.c_str());
			
			// Play the audio
			// (Audio playback would be implemented in game audio system)
			
			// Show who's talking
			toast(playerName + " is speaking");
		});
		
		// Start voice chat
		if (vc.startVoiceChat())
		{
			m_voiceChatEnabled = true;
			notify("Voice chat ready! Hold 'V' to talk");
		}
		else
		{
			notifyError("Failed to start voice chat");
		}
	}
	
	void stopVoiceChat()
	{
		VoiceChat::getInstance().stopVoiceChat();
		MicrophoneHandler::getInstance().stopRecording();
		m_voiceChatEnabled = false;
	}
	
	void startTransmitting()
	{
		if (!m_isPushingToTalk && m_voiceChatEnabled)
		{
			VoiceChat::getInstance().pressPushToTalk();
			m_isPushingToTalk = true;
			toast("🎤 Transmitting...");
		}
	}
	
	void stopTransmitting()
	{
		if (m_isPushingToTalk)
		{
			VoiceChat::getInstance().releasePushToTalk();
			m_isPushingToTalk = false;
		}
	}
	
	void updateVolumeIndicator()
	{
		float volume = MicrophoneHandler::getInstance().getCurrentVolumeLevel();
		
		// Show volume bar (would render in UI)
		// For now just log when speaking detected
		if (volume > 0.1f && m_isPushingToTalk)
		{
			// Player is speaking
			static int frameCount = 0;
			if (frameCount++ % 60 == 0)  // Log once per second
			{
				logInfo("[VoiceChat] Volume: %.2f", volume);
			}
		}
	}
	
	float m_currentVolume = 0.0f;
};

// Register the mod
REGISTER_MOD(VoiceChatMod);
