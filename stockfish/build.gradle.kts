plugins {
    id("com.android.library")
    id("maven-publish")
}

android {
    namespace = "com.vayunmathur.stockfish"
    compileSdk = 37

    // Pin toolchain versions so the native .so is reproducible across builds
    // (local + JitPack). Matches the versions used by the consuming app.
    ndkVersion = "29.0.14206865"

    defaultConfig {
        minSdk = 31

        // Build Stockfish from source for arm64 only (matches the prior prebuilt .so).
        ndk {
            abiFilters += "arm64-v8a"
        }

        externalNativeBuild {
            cmake {
                // no extra args; flags live in CMakeLists.txt
            }
        }
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "4.1.2"
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }

    publishing {
        singleVariant("release")
    }
}

afterEvaluate {
    publishing {
        publications {
            create<MavenPublication>("release") {
                from(components["release"])
                groupId = "com.github.vayun-mathur.Stockfish-Library"
                artifactId = "stockfish"
                version = "1.0.0"
            }
        }
    }
}
