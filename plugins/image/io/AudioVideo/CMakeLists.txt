# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

tuttle_ofx_plugin_target(AudioVideo)

# Add external libraries
tuttle_ofx_plugin_add_library(AudioVideo FFMPEG)
tuttle_ofx_plugin_add_library(AudioVideo sequenceParser)