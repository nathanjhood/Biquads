# Biquads with CMake

## CMake Presets:

These commands assume you have Ninja installed on your system.

### Configure:

```.sh
cmake --fresh --preset=default
```

### Build:

```.sh
cmake --build --preset=default
```

### Test:

```.sh
ctest --preset=default
```

*uses [Tracktion Pluginval](https://github.com/Tracktion/pluginval) to validate the built plugin using a series of predetermined tests.*

### Install:

```.sh
cmake --install ./Builds --prefix=</some/location/on/disk>
```

### Package:
```.sh
cpack --preset=default
```

## More presets:

You may also check the other provided presets, such as using generators besides Ninja and other optionable configuration details which will often vary from machine to machine. For example, on Linux, these presets are currently provided and supported by ```Biquads```:

```.sh
cmake --list-presets=all

Available configure presets:

  "default"                  - Default
  "linux-make-debug"         - Make (Debug)
  "linux-make-release"       - Make (Release)
  "linux-ninja-debug"        - Ninja (Debug)
  "linux-ninja-release"      - Ninja (Release)
  "linux-ninja-multi-config" - Ninja Multi-Config

Available build presets:

  "default"                                  - Default
  "default-verbose"                          - Default (Verbose)
  "linux-make-debug"                         - Debug
  "linux-make-debug-verbose"                 - Debug (Verbose)
  "linux-make-release"                       - Release
  "linux-make-release-verbose"               - Release (Verbose)
  "linux-ninja-debug"                        - Debug
  "linux-ninja-debug-verbose"                - Debug (Verbose)
  "linux-ninja-release"                      - Release
  "linux-ninja-release-verbose"              - Release (Verbose)
  "linux-ninja-multi-config-debug"           - Debug
  "linux-ninja-multi-config-release"         - Release
  "linux-ninja-multi-config-debug-verbose"   - Debug (Verbose)
  "linux-ninja-multi-config-release-verbose" - Release (Verbose)

Available test presets:

  "default"                          - Default
  "linux-ninja-multi-config-debug"   - Debug
  "linux-ninja-multi-config-release" - Release

Available package presets:

  "default"                          - Default
  "linux-ninja-multi-config-source"  - Source
  "linux-ninja-multi-config-debug"   - Build (Debug)
  "linux-ninja-multi-config-release" - Build (Release)
  "linux-ninja-multi-config-all"     - Build (All)

Available workflow presets:

  "default"                          - Default
  "linux-ninja-multi-config-all"     - Linux Ninja Multi-Config (All)
  "linux-ninja-multi-config-debug"   - Linux Ninja Multi-Config (Debug)
  "linux-ninja-multi-config-release" - Linux Ninja Multi-Config (Release)

```

CMake presets still allow you manually specify (and/or override) your own definitions on the command line using the ```-D<SOME_VAR>=<SOME_VALUE>``` flag as usual.

Multi-config generators enable you to generate both a ```Debug``` *and* a ```Release``` build configuration at the same time, allowing you to decide whether you want to compile in ```Debug``` or ```Release``` mode, *without* running the configure step again. This does however require that you also specify a configuration for all commands besides the configuration generator.

Below is an example of how to use the provided extended CMake presets (beyond the ```default```flag), to easily build ```Biquads``` using some simple commands:


### Configure both ```Debug``` and ```Release``` builds at the same time, using Microsoft's ```vcpkg``` to find and acquire our dependencies (such as JUCE, etc)

```.sh
cmake --fresh --preset linux-ninja-multi-config --toolchain ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-linux
```

### Build the ```Debug``` configuration verbosely:

```.sh
cmake --build --preset linux-ninja-multi-config-debug-verbose
```

### Test the ```Debug``` build:

```.sh
ctest --preset=default -C Debug
```

### Package the ```Debug``` build:
```.sh
cpack --preset=default --config Debug
```

More presets will be added soon! Check out the GitHub actions for a more detailed overview of configuring and building this project - and it's requirements - with standard CMake commands for your platform.

## Options and variables

These have all been set to resonable defaults for each system, platform, etc.

However, they may be specified with ```-D<OPTION>=<CHOICE>``` on the command line to override any defaults.

Note that some options have requirements; for example, it is only possible to build ```AU``` formats on Apple machines, and so forth.

It is recommended to use modern CMake approaches such as specifiying an appropriate argument *type* for each option, where known. Here are some examples:

```.sh
-DSTONEYDSP_BIQUADS_USE_*:BOOL=<TRUE|FALSE>
```

```.sh
-DSTONEYDSP_BIQUADS_*_DIR:PATH=</some/path>
```

```.sh
-DSTONEYDSP_BIQUADS_*_FILE:FILEPATH=</some/path/to/file.txt>
```

This example argument will tell CMake to configure the VST3 for building (available by default on all platforms):

```.sh
-DSTONEYDSP_BIQUADS_BUILD_VST3:BOOL=TRUE
```

...and to copy it to the user's VST3 plugin folder after building (available by default on all platforms, here passing in a typical Windows-style VST3 installation folder)

```.sh
-DSTONEYDSP_BIQUADS_COPY_PLUGIN_AFTER_BUILD:BOOL=TRUE -DSTONEYDSP_BIQUADS_VST3_COPY_DIR:PATH="C:/Program Files/Common Data/VST3"
```

## Glossary

| FLAG | TYPE | CHOICES | DEPENDS | NOTES | DEFAULT |
| :-: | :-: | :-: | :-: | :-: | :-: |
| ```STONEYDSP_BIQUADS_USING_JUCE_RECOMMENDED_CONFIG_FLAGS``` | ```BOOL``` | ```ON``` ```OFF``` | | <p>Link with juce::juce_recommended_config_flags.</p> <p>Available by default on all platforms.</p> | ```ON``` |
| ```STONEYDSP_BIQUADS_USING_JUCE_RECOMMENDED_LTO_FLAGS``` | ```BOOL``` | ```ON``` ```OFF``` | | <p>Link with juce::juce_recommended_lto_flags.</p> <p>Available by default on all platforms.</p> | ```ON``` |
| ```STONEYDSP_BIQUADS_USING_JUCE_RECOMMENDED_WARNING_FLAGS``` | ```BOOL``` | ```ON``` ```OFF``` | | <p>Link with juce::juce_recommended_warning_flags.</p> <p>Available by default on all platforms.</p> | ```ON``` |
| ```STONEYDSP_BIQUADS_BUILD_STANDALONE``` | ```BOOL``` | ```ON``` ```OFF``` | | <p>Configure and build a Biquads native system application.</p> <p>Available by default on all platforms.</p> | ```ON``` |
| ```STONEYDSP_BIQUADS_BUILD_VST3``` | ```BOOL``` | ```ON``` ```OFF``` | | <p>Configure and build a Biquads VST3 plugin.</p> <p>Available by default on all platforms.</p> <p>A path to an alternative VST3 SDK may *optionally* be be passed in via ```STONEYDSP_BIQUADS_VST3_SDK_PATH```, otherwise the JUCE VST3 SDK will be used by default.</p> | ```ON``` |
| ```STONEYDSP_BIQUADS_BUILD_VST2``` | ```BOOL``` | ```ON``` ```OFF``` | ```STONEYDSP_BIQUADS_VST2_SDK_PATH``` | <p>Configure and build a Biquads VST2 plugin.</p>  <p>Available on all platforms.</p> <p>A path to a VST2 SDK *must* be passed in via ```STONEYDSP_BIQUADS_VST2_SDK_DIR```. | ```OFF``` |
| ```STONEYDSP_BIQUADS_BUILD_AAX``` | ```BOOL``` | ```ON``` ```OFF``` | ```STONEYDSP_BIQUADS_AAX_SDK_PATH``` | <p>Configure and build a Biquads AAX plugin (e.g., for Pro-Tools).</p>  <p>Available on all platforms.</p> <p>A path to an AAX SDK *must* be be passed in via ```STONEYDSP_BIQUADS_AAX_SDK_PATH```.</p> | ```OFF``` |
| ```STONEYDSP_BIQUADS_BUILD_AU``` | ```BOOL``` | ```ON``` ```OFF``` | ```DARWIN``` | <p>Configure and build a Biquads AU plugin.</p>  <p>Available only on macOS platforms (i.e., ```DARWIN```).</p> | ```ON``` |
| ```STONEYDSP_BIQUADS_BUILD_AUV3``` | ```BOOL``` | ```ON``` ```OFF``` | ```DARWIN``` | <p>Configure and build a Biquads AUv3 plugin.</p>  <p>Available only on macOS platforms (i.e., ```DARWIN```).</p> | ```OFF``` |
| ```STONEYDSP_BIQUADS_BUILD_LV2``` | ```BOOL``` | ```ON``` ```OFF``` | ```UNIX``` | <p>Configure and build a Biquads LV2 plugin.</p>  <p>Available only on Linux platforms (i.e., ```UNIX```).</p> | ```OFF``` |
| ```STONEYDSP_BIQUADS_COPY_PLUGIN_AFTER_BUILD``` | ```BOOL``` | ```ON``` ```OFF``` | | <p>Copy the built plugin formats specified above to their relevant directories, once the build is complete.</p>  <p>Available by default on all platforms.</p> <p>(May require elevated privileges to run.)</p> | ```OFF``` |
| ```STONEYDSP_BIQUADS_VST3_AUTO_MANIFEST``` | ```BOOL``` | ```ON``` ```OFF``` | ```STONEYDSP_BIQUADS_BUILD_VST3``` | <p>Generate a manifest file for VST3 plugin formats.</p>  <p>Available by default on all platforms.</p> | ```ON``` |
| ```STONEYDSP_BIQUADS_GENERATE_JUCE_HEADER``` | ```BOOL``` | ```ON``` ```OFF``` | | <p>Generate a ```JuceHeader.h``` file for inclusion.</p>  <p>Available by default on all platforms.</p> | ```ON``` |
