/********************************************************************
	Minecraft: Pocket Edition - Microphone System
	MicrophoneHandler.hpp - Record audio, voice chat, sound detection
	
	FULL MICROPHONE ACCESS FOR MODS
********************************************************************/

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>

#ifdef __ANDROID__
#include <jni.h>
#endif

// ════════════════════════════════════════════════════════════════
// AUDIO CONFIGURATION
// ════════════════════════════════════════════════════════════════

struct AudioConfig
{
	int sampleRate = 44100;      // Hz (CD quality)
	int channelCount = 1;        // Mono
	int bitsPerSample = 16;      // 16-bit audio
	int bufferSize = 4096;       // Samples per buffer
};

// ════════════════════════════════════════════════════════════════
// AUDIO DATA
// ════════════════════════════════════════════════════════════════

struct AudioData
{
	std::vector<short> samples;  // Raw audio samples
	int sampleRate;
	int channelCount;
	double timestamp;            // Recording timestamp
	
	// Helper methods
	int getSampleCount() const { return samples.size(); }
	double getDuration() const { return (double)samples.size() / sampleRate; }
	
	// Get volume level (0.0 - 1.0)
	float getVolumeLevel() const;
	
	// Detect if someone is speaking
	bool isSpeaking(float threshold = 0.1f) const;
};

// ════════════════════════════════════════════════════════════════
// MICROPHONE CALLBACKS
// ════════════════════════════════════════════════════════════════

typedef std::function<void(const AudioData&)> AudioCallback;
typedef std::function<void(float)> VolumeLevelCallback;

// ════════════════════════════════════════════════════════════════
// MICROPHONE HANDLER
// ════════════════════════════════════════════════════════════════

class MicrophoneHandler
{
public:
	static MicrophoneHandler& getInstance()
	{
		static MicrophoneHandler instance;
		return instance;
	}
	
#ifdef __ANDROID__
	// Initialize with JNI
	void init(JNIEnv* env, jobject context);
#endif
	
	// ═══════════════════════════════════════════════════════════
	// RECORDING CONTROL
	// ═══════════════════════════════════════════════════════════
	
	bool startRecording(const AudioConfig& config = AudioConfig());
	void stopRecording();
	bool isRecording() const { return m_isRecording; }
	
	// Pause/Resume (keeps buffer)
	void pauseRecording();
	void resumeRecording();
	bool isPaused() const { return m_isPaused; }
	
	// ═══════════════════════════════════════════════════════════
	// REAL-TIME AUDIO CALLBACKS
	// ═══════════════════════════════════════════════════════════
	
	// Called for each audio buffer (real-time)
	void setAudioCallback(AudioCallback callback)
	{
		m_audioCallback = callback;
	}
	
	// Called with volume level updates (for visualizers)
	void setVolumeLevelCallback(VolumeLevelCallback callback)
	{
		m_volumeCallback = callback;
	}
	
	// ═══════════════════════════════════════════════════════════
	// AUDIO DATA ACCESS
	// ═══════════════════════════════════════════════════════════
	
	// Get recorded audio
	AudioData getRecordedAudio();
	
	// Get current volume level (0.0 - 1.0)
	float getCurrentVolumeLevel();
	
	// Clear recorded buffer
	void clearBuffer();
	
	// ═══════════════════════════════════════════════════════════
	// VOICE DETECTION
	// ═══════════════════════════════════════════════════════════
	
	// Is someone currently speaking?
	bool isSpeakingDetected(float threshold = 0.1f);
	
	// Voice activation detection (automatic start/stop)
	void enableVoiceActivation(float threshold = 0.1f);
	void disableVoiceActivation();
	bool isVoiceActivationEnabled() const { return m_voiceActivation; }
	
	// ═══════════════════════════════════════════════════════════
	// AUDIO PROCESSING
	// ═══════════════════════════════════════════════════════════
	
	// Save recorded audio to file
	bool saveToFile(const std::string& path, const std::string& format = "wav");
	
	// Load audio from file
	bool loadFromFile(const std::string& path);
	
	// ═══════════════════════════════════════════════════════════
	// CONFIGURATION
	// ═══════════════════════════════════════════════════════════
	
	void setAudioConfig(const AudioConfig& config);
	AudioConfig getAudioConfig() const { return m_config; }
	
	// Enable/disable noise suppression
	void setNoiseSuppression(bool enabled);
	
	// Enable/disable echo cancellation
	void setEchoCancellation(bool enabled);
	
	// Set gain/amplification (1.0 = normal, 2.0 = double volume)
	void setGain(float gain);
	
	// ═══════════════════════════════════════════════════════════
	// MICROPHONE INFO
	// ═══════════════════════════════════════════════════════════
	
	bool isMicrophoneAvailable();
	std::vector<std::string> getAvailableMicrophones();
	std::string getCurrentMicrophone();
	void setMicrophone(int index);
	
private:
	MicrophoneHandler() {}
	
#ifdef __ANDROID__
	JNIEnv* m_env = nullptr;
	jobject m_context = nullptr;
	jobject m_audioRecord = nullptr;  // Android AudioRecord object
#endif
	
	bool m_isRecording = false;
	bool m_isPaused = false;
	bool m_voiceActivation = false;
	float m_voiceThreshold = 0.1f;
	
	AudioConfig m_config;
	std::vector<short> m_recordBuffer;
	
	AudioCallback m_audioCallback;
	VolumeLevelCallback m_volumeCallback;
	
	float m_currentVolume = 0.0f;
	float m_gain = 1.0f;
	
	// Android audio recording thread
	void recordingThread();
	bool m_shouldStopThread = false;
	
	// Process audio buffer
	void processAudioBuffer(const std::vector<short>& buffer);
};

// ════════════════════════════════════════════════════════════════
// VOICE CHAT HELPER
// ════════════════════════════════════════════════════════════════

class VoiceChat
{
public:
	static VoiceChat& getInstance()
	{
		static VoiceChat instance;
		return instance;
	}
	
	// Start voice chat (auto records and transmits)
	void startVoiceChat();
	void stopVoiceChat();
	bool isActive() const { return m_active; }
	
	// Set voice chat callback (receives audio from other players)
	typedef std::function<void(const std::string& playerName, const AudioData&)> VoiceChatCallback;
	void setVoiceChatCallback(VoiceChatCallback callback);
	
	// Mute/unmute
	void setMuted(bool muted) { m_muted = muted; }
	bool isMuted() const { return m_muted; }
	
	// Push-to-talk mode
	void setPushToTalk(bool enabled) { m_pushToTalk = enabled; }
	void pressPushToTalk();
	void releasePushToTalk();
	
private:
	VoiceChat() {}
	
	bool m_active = false;
	bool m_muted = false;
	bool m_pushToTalk = false;
	bool m_isPressing = false;
	
	VoiceChatCallback m_callback;
};

// ════════════════════════════════════════════════════════════════
// CONVENIENCE FUNCTIONS
// ════════════════════════════════════════════════════════════════

inline void startMicrophone()
{
	MicrophoneHandler::getInstance().startRecording();
}

inline void stopMicrophone()
{
	MicrophoneHandler::getInstance().stopRecording();
}

inline float getMicrophoneVolume()
{
	return MicrophoneHandler::getInstance().getCurrentVolumeLevel();
}

inline bool isSpeaking()
{
	return MicrophoneHandler::getInstance().isSpeakingDetected();
}
