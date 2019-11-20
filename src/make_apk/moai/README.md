##NDK Hello World
This example project uses the Android NDK (Native Development Kit) and JNI (Java Native Interface) to call a function from a C++ module from an Android Activity.

###Prerequisites

- JDK
- Android SDK
- Android NDK
- Android Support Library (may not have been automatically installed with the Android SDK)

#####1. Clone the repository
```
git clone https://github.com/byronh/ndk-helloworld.git
```

If using Android Studio or IntelliJ IDEA, import the build.gradle file as a new project. Then you can skip everything after step 2 and just press the "Run" button in the top-right corner.

#####2. Specify android-sdk and android-ndk location

Create a file called local.properties in the root of the repository and specify the paths to the root folders of your Android SDK and Android NDK:

e.g. on Linux or Mac:

```
sdk.dir=/opt/android-sdk
ndk.dir=/opt/android-ndk
```

e.g. on Windows:

```
sdk.dir=C:\\Dev\android-sdk
ndk.dir=C:\\Dev\android-ndk
```

#####3. Build and install to Android device using Gradle
```
./gradlew installDebug
```

#####4. Run on Android device

Click on the application icon or run the following command:

```
adb shell am start -n com.byronh.ndk.helloworld/.MainActivity
```
