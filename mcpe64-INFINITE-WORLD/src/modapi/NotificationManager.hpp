/********************************************************************
	Minecraft: Pocket Edition - Notification System
	NotificationManager.hpp - In-game and system notifications
	
	Shows notifications, achievements, alerts
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
// NOTIFICATION TYPES
// ════════════════════════════════════════════════════════════════

enum class NotificationType
{
	INFO,             // General information
	SUCCESS,          // Achievement/success
	WARNING,          // Warning message
	ERROR,            // Error message
	CHAT,             // Chat message
	SYSTEM,           // System notification (Android notification tray)
	ACHIEVEMENT       // Achievement unlocked
};

// ════════════════════════════════════════════════════════════════
// NOTIFICATION PRIORITY
// ════════════════════════════════════════════════════════════════

enum class NotificationPriority
{
	LOW,              // Can be dismissed easily
	NORMAL,           // Standard priority
	HIGH,             // Important, harder to dismiss
	URGENT            // Critical, requires acknowledgment
};

// ════════════════════════════════════════════════════════════════
// NOTIFICATION DATA
// ════════════════════════════════════════════════════════════════

struct Notification
{
	std::string id;               // Unique identifier
	NotificationType type;
	NotificationPriority priority;
	
	std::string title;
	std::string message;
	std::string icon;             // Path to icon texture
	
	float duration = 3.0f;        // How long to show (seconds, 0 = manual dismiss)
	bool playSound = true;
	std::string soundPath;        // Custom sound (empty = default)
	
	// For system notifications
	bool showInNotificationTray = false;
	
	// Callbacks
	std::function<void()> onClick;
	std::function<void()> onDismiss;
	
	// Timestamp
	double createdTime = 0.0;
	
	// Custom data (for mods)
	std::map<std::string, std::string> customData;
};

// ════════════════════════════════════════════════════════════════
// ACHIEVEMENT DEFINITION
// ════════════════════════════════════════════════════════════════

struct Achievement
{
	std::string id;
	std::string name;
	std::string description;
	std::string icon;             // Icon texture path
	
	int points = 10;              // Achievement points
	bool isSecret = false;        // Hidden until unlocked
	
	// Requirements (optional)
	std::function<bool()> checkUnlock;
	
	// Unlock callback
	std::function<void()> onUnlock;
};

// ════════════════════════════════════════════════════════════════
// NOTIFICATION MANAGER
// ════════════════════════════════════════════════════════════════

class NotificationManager
{
public:
	static NotificationManager& getInstance()
	{
		static NotificationManager instance;
		return instance;
	}
	
#ifdef __ANDROID__
	void init(JNIEnv* env, jobject context);
#endif
	
	// ═══════════════════════════════════════════════════════════
	// SHOW NOTIFICATIONS
	// ═══════════════════════════════════════════════════════════
	
	// Simple notification
	std::string showNotification(
		const std::string& title,
		const std::string& message,
		NotificationType type = NotificationType::INFO,
		float duration = 3.0f
	);
	
	// Advanced notification
	std::string showNotification(const Notification& notification);
	
	// Quick helpers
	void showInfo(const std::string& message);
	void showSuccess(const std::string& message);
	void showWarning(const std::string& message);
	void showError(const std::string& message);
	
	// ═══════════════════════════════════════════════════════════
	// SYSTEM NOTIFICATIONS (Android notification tray)
	// ═══════════════════════════════════════════════════════════
	
	void showSystemNotification(
		const std::string& title,
		const std::string& message,
		const std::string& icon = ""
	);
	
	void cancelSystemNotification(int notificationId);
	void cancelAllSystemNotifications();
	
	// ═══════════════════════════════════════════════════════════
	// NOTIFICATION MANAGEMENT
	// ═══════════════════════════════════════════════════════════
	
	void dismissNotification(const std::string& notificationId);
	void dismissAll();
	
	std::vector<Notification> getActiveNotifications() const;
	Notification* getNotification(const std::string& id);
	
	// ═══════════════════════════════════════════════════════════
	// ACHIEVEMENTS
	// ═══════════════════════════════════════════════════════════
	
	void registerAchievement(const Achievement& achievement);
	
	void unlockAchievement(const std::string& achievementId);
	bool isAchievementUnlocked(const std::string& achievementId) const;
	
	std::vector<Achievement> getAllAchievements() const;
	std::vector<Achievement> getUnlockedAchievements() const;
	
	int getTotalAchievementPoints() const;
	
	// ═══════════════════════════════════════════════════════════
	// NOTIFICATION SETTINGS
	// ═══════════════════════════════════════════════════════════
	
	void setNotificationsEnabled(bool enabled);
	bool areNotificationsEnabled() const { return m_enabled; }
	
	void setSoundEnabled(bool enabled);
	bool isSoundEnabled() const { return m_soundEnabled; }
	
	void setMaxNotifications(int max);
	int getMaxNotifications() const { return m_maxNotifications; }
	
	// ═══════════════════════════════════════════════════════════
	// NOTIFICATION QUEUE
	// ═══════════════════════════════════════════════════════════
	
	void update(float deltaTime);  // Call every frame to handle timers
	
	// ═══════════════════════════════════════════════════════════
	// TOAST MESSAGES (quick bottom messages)
	// ═══════════════════════════════════════════════════════════
	
	void showToast(const std::string& message, float duration = 2.0f);
	
private:
	NotificationManager() {}
	
#ifdef __ANDROID__
	JNIEnv* m_env = nullptr;
	jobject m_context = nullptr;
	jobject m_notificationManager = nullptr;
#endif
	
	std::vector<Notification> m_activeNotifications;
	std::map<std::string, Achievement> m_achievements;
	std::vector<std::string> m_unlockedAchievements;
	
	bool m_enabled = true;
	bool m_soundEnabled = true;
	int m_maxNotifications = 5;
	
	int m_nextNotificationId = 1;
	int m_nextSystemNotificationId = 1000;
	
	std::string generateNotificationId();
	void cleanupOldNotifications();
};

// ════════════════════════════════════════════════════════════════
// NOTIFICATION BUILDER - Fluent API
// ════════════════════════════════════════════════════════════════

class NotificationBuilder
{
public:
	NotificationBuilder()
	{
		m_notification.type = NotificationType::INFO;
		m_notification.priority = NotificationPriority::NORMAL;
	}
	
	NotificationBuilder& setTitle(const std::string& title)
	{
		m_notification.title = title;
		return *this;
	}
	
	NotificationBuilder& setMessage(const std::string& message)
	{
		m_notification.message = message;
		return *this;
	}
	
	NotificationBuilder& setType(NotificationType type)
	{
		m_notification.type = type;
		return *this;
	}
	
	NotificationBuilder& setPriority(NotificationPriority priority)
	{
		m_notification.priority = priority;
		return *this;
	}
	
	NotificationBuilder& setDuration(float duration)
	{
		m_notification.duration = duration;
		return *this;
	}
	
	NotificationBuilder& setIcon(const std::string& icon)
	{
		m_notification.icon = icon;
		return *this;
	}
	
	NotificationBuilder& setSound(const std::string& soundPath)
	{
		m_notification.soundPath = soundPath;
		m_notification.playSound = !soundPath.empty();
		return *this;
	}
	
	NotificationBuilder& enableSound(bool enabled = true)
	{
		m_notification.playSound = enabled;
		return *this;
	}
	
	NotificationBuilder& showInTray(bool show = true)
	{
		m_notification.showInNotificationTray = show;
		return *this;
	}
	
	NotificationBuilder& onClick(std::function<void()> callback)
	{
		m_notification.onClick = callback;
		return *this;
	}
	
	NotificationBuilder& onDismiss(std::function<void()> callback)
	{
		m_notification.onDismiss = callback;
		return *this;
	}
	
	std::string show()
	{
		return NotificationManager::getInstance().showNotification(m_notification);
	}
	
private:
	Notification m_notification;
};

// ════════════════════════════════════════════════════════════════
// CONVENIENCE FUNCTIONS
// ════════════════════════════════════════════════════════════════

inline void notify(const std::string& message)
{
	NotificationManager::getInstance().showInfo(message);
}

inline void notifySuccess(const std::string& message)
{
	NotificationManager::getInstance().showSuccess(message);
}

inline void notifyWarning(const std::string& message)
{
	NotificationManager::getInstance().showWarning(message);
}

inline void notifyError(const std::string& message)
{
	NotificationManager::getInstance().showError(message);
}

inline void toast(const std::string& message)
{
	NotificationManager::getInstance().showToast(message);
}

inline void unlockAchievement(const std::string& id)
{
	NotificationManager::getInstance().unlockAchievement(id);
}
