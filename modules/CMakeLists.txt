#[=============================================================================[
    target: stoneydsp_core, stoneydsp_audio
]=============================================================================]#

juce_add_modules(
    INSTALL_PATH "include/StoneyDSP/modules"
    ALIAS_NAMESPACE StoneyDSP
    stoneydsp_core
    stoneydsp_audio
)
