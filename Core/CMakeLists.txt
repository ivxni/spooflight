# ═══════════════════════════════════════════════════════════════════════════
#  SPOOFLIGHT - CMake Build Configuration
# ═══════════════════════════════════════════════════════════════════════════
# 
# 📚 LEKTION: Was ist CMake?
# 
# CMake ist wie package.json + webpack für C++.
# Es sagt dem Compiler:
#   - Welche Dateien kompiliert werden sollen
#   - Welche Libraries gelinkt werden müssen
#   - Welche Einstellungen gelten

cmake_minimum_required(VERSION 3.20)
project(Spooflight)

# C++20 Standard verwenden (modern!)
set(CMAKE_CXX_STANDARD 20)

# ═══════════════════════════════════════════════════════════════════════════
# 📚 LEKTION: Preprocessor Definitions
# 
# SPOOFLIGHT_EXPORTS wird definiert wenn wir die DLL BAUEN.
# Das aktiviert __declspec(dllexport) in exports.h
# ═══════════════════════════════════════════════════════════════════════════
add_definitions(-DSPOOFLIGHT_EXPORTS)

# ═══════════════════════════════════════════════════════════════════════════
# 📚 LEKTION: Include Directories
# 
# Sagt dem Compiler wo er nach .h Dateien suchen soll.
# Ohne das müsstest du immer den vollen Pfad angeben.
# ═══════════════════════════════════════════════════════════════════════════
include_directories(${CMAKE_SOURCE_DIR}/include)

# ═══════════════════════════════════════════════════════════════════════════
# Source Files - Alle .cpp Dateien die kompiliert werden
# ═══════════════════════════════════════════════════════════════════════════
set(SOURCES
    src/spoofer.cpp
)

set(HEADERS
    include/exports.h
    include/spoofer.h
)

# ═══════════════════════════════════════════════════════════════════════════
# DLL erstellen (SHARED = dynamisch ladbare Library)
# ═══════════════════════════════════════════════════════════════════════════
add_library(Spooflight SHARED ${SOURCES} ${HEADERS})

# ═══════════════════════════════════════════════════════════════════════════
# 📚 LEKTION: Windows Libraries linken
# 
# Diese Libraries sind Teil von Windows und enthalten Funktionen die wir brauchen:
#   - advapi32: Registry Funktionen (RegOpenKeyEx, RegSetValueEx, etc.)
#   - iphlpapi: Netzwerk Funktionen (für MAC Spoofing später)
#   - setupapi: Geräte-Informationen
#   - wbemuuid: WMI (für Hardware-IDs später)
# ═══════════════════════════════════════════════════════════════════════════
target_link_libraries(Spooflight
    advapi32      # Registry API
    iphlpapi      # Network API (Phase 3) ✅ Aktiviert!
    # setupapi    # Device API (Phase 4)
    # wbemuuid    # WMI API (Phase 4)
)

# Output-Name der DLL
set_target_properties(Spooflight PROPERTIES
    OUTPUT_NAME "SpooflightCore"
    PREFIX ""
)

# ═══════════════════════════════════════════════════════════════════════════
# TEST EXECUTABLE
# Ein kleines Programm zum Testen unserer DLL-Funktionen
# ═══════════════════════════════════════════════════════════════════════════
add_executable(SpooflightTest
    src/test_main.cpp
    src/spoofer.cpp      # Direkt einbinden (einfacher als DLL laden)
)

# WICHTIG: SPOOFLIGHT_EXPORTS definieren damit die Funktionen richtig kompiliert werden
target_compile_definitions(SpooflightTest PRIVATE SPOOFLIGHT_EXPORTS)

target_link_libraries(SpooflightTest
    advapi32
    iphlpapi      # Für MAC Spoofing
)
