#!/bin/bash
# ════════════════════════════════════════════════════════════════
#  MCPE 0.6.1 INFINITE WORLD - Linux/Termux Build Script
# ════════════════════════════════════════════════════════════════

set -e  # Exit on error

echo "════════════════════════════════════════════════════════════"
echo "  Building Minecraft PE 0.6.1 - Infinite World Mod"
echo "════════════════════════════════════════════════════════════"

# Check requirements
command -v ndk-build >/dev/null 2>&1 || { echo "❌ ndk-build not found. Install Android NDK r14b"; exit 1; }
command -v javac >/dev/null 2>&1 || { echo "❌ javac not found. Install Java JDK"; exit 1; }
command -v aapt >/dev/null 2>&1 || { echo "❌ aapt not found. Install Android SDK Build Tools"; exit 1; }

# Paths
REPO_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$HOME/mcpe-build"
NDK_DIR="${NDK_HOME:-$HOME/android-ndk-r14b}"
ANDROID_JAR="${ANDROID_HOME}/platforms/android-21/android.jar"

mkdir -p "$BUILD_DIR"
mkdir -p "$BUILD_DIR/lib/arm64-v8a"

echo ""
echo "==> Step 1: Building native library (C++)"
cd "$REPO_DIR/project/android/jni"
$NDK_DIR/ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=Android.mk

# Copy .so file
cp "$REPO_DIR/project/android/libs/arm64-v8a/libminecraftpe.so" "$BUILD_DIR/lib/arm64-v8a/"

echo ""
echo "==> Step 2: Compiling Java code"
cd "$REPO_DIR/project/android_java"
mkdir -p "$BUILD_DIR/classes"
find src -name "*.java" -exec javac -d "$BUILD_DIR/classes" -classpath "$ANDROID_JAR" {} +

echo ""
echo "==> Step 3: Creating DEX file"
cd "$BUILD_DIR"
dx --dex --output=classes.dex classes/

echo ""
echo "==> Step 4: Generating R.java"
mkdir -p "$BUILD_DIR/gen"
aapt package -f -m -J "$BUILD_DIR/gen" \
    -S "$REPO_DIR/project/android_java/res" \
    -M "$REPO_DIR/project/android_java/AndroidManifest.xml" \
    -I "$ANDROID_JAR"

echo ""
echo "==> Step 5: Packaging APK"
aapt package -f \
    -M "$REPO_DIR/project/android_java/AndroidManifest.xml" \
    -S "$REPO_DIR/project/android_java/res" \
    -I "$ANDROID_JAR" \
    -F "$BUILD_DIR/minecraftpe-unsigned.apk"

# Add DEX
cd "$BUILD_DIR"
aapt add minecraftpe-unsigned.apk classes.dex

# Add native library
aapt add minecraftpe-unsigned.apk lib/arm64-v8a/libminecraftpe.so

# Add assets
cd "$REPO_DIR/data"
zip -r "$BUILD_DIR/minecraftpe-unsigned.apk" * -x "*.git*"

echo ""
echo "==> Step 6: Signing APK"
KEYSTORE="$BUILD_DIR/debug.keystore"
if [ ! -f "$KEYSTORE" ]; then
    keytool -genkeypair -keystore "$KEYSTORE" \
        -storepass android -keypass android \
        -alias androiddebugkey -keyalg RSA -keysize 2048 \
        -validity 10000 -dname "CN=Android Debug"
fi

# Sign and align
zipalign -f 4 "$BUILD_DIR/minecraftpe-unsigned.apk" "$BUILD_DIR/minecraftpe-aligned.apk"
apksigner sign --ks "$KEYSTORE" --ks-pass pass:android \
    --out "$BUILD_DIR/minecraftpe-INFINITE-debug.apk" \
    "$BUILD_DIR/minecraftpe-aligned.apk"

echo ""
echo "════════════════════════════════════════════════════════════"
echo "✅ BUILD SUCCESS!"
echo "════════════════════════════════════════════════════════════"
echo ""
echo "APK Location:"
echo "  $BUILD_DIR/minecraftpe-INFINITE-debug.apk"
echo ""
echo "To install on connected device:"
echo "  adb install -r $BUILD_DIR/minecraftpe-INFINITE-debug.apk"
echo ""
echo "Or transfer to your phone and install manually!"
echo "════════════════════════════════════════════════════════════"
