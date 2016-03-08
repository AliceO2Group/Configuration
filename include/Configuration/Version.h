///
/// @file    Configuration/Version.h
/// @brief   Report the version for this package.
/// @author  Barthélémy von Haller
///

#ifndef CONFIGURATION_CONFIGURATION_VERSION_H
#define CONFIGURATION_CONFIGURATION_VERSION_H

#include <string>
#include <sstream>

namespace AliceO2 {
namespace ProjectTemplate {
namespace Configuration {
/// The current major version.
#define CONFIGURATION_VERSION_MAJOR 0

/// The current minor version.
#define CONFIGURATION_VERSION_MINOR 0

/// The current patch level.
#define CONFIGURATION_VERSION_PATCH 0

/// The current VCS revision.
#define CONFIGURATION_VCS_REVISION "06c0417eaa3f8a4f339cd637a6e00fb12ad56553"

/// True if the current version is newer than the given one.
#define CONFIGURATION_VERSION_GT(MAJOR, MINOR, PATCH) \
  ((CONFIGURATION_VERSION_MAJOR > MAJOR) ||           \
   (CONFIGURATION_VERSION_MAJOR ==                    \
    MAJOR&&(CONFIGURATION_VERSION_MINOR > MINOR || (CONFIGURATION_VERSION_MINOR == MINOR&& CONFIGURATION_VERSION_PATCH > PATCH))))

/// True if the current version is equal or newer to the given.
#define CONFIGURATION_VERSION_GE(MAJOR, MINOR, PATCH) \
  ((CONFIGURATION_VERSION_MAJOR > MAJOR) ||           \
   (CONFIGURATION_VERSION_MAJOR ==                    \
    MAJOR&&(CONFIGURATION_VERSION_MINOR > MINOR || (CONFIGURATION_VERSION_MINOR == MINOR&& CONFIGURATION_VERSION_PATCH >= PATCH))))

/// True if the current version is older than the given one.
#define CONFIGURATION_VERSION_LT(MAJOR, MINOR, PATCH) \
  ((CONFIGURATION_VERSION_MAJOR < MAJOR) ||           \
   (CONFIGURATION_VERSION_MAJOR ==                    \
    MAJOR&&(CONFIGURATION_VERSION_MINOR < MINOR || (CONFIGURATION_VERSION_MINOR == MINOR&& CONFIGURATION_VERSION_PATCH < PATCH))))

/// True if the current version is older or equal to the given.
#define CONFIGURATION_VERSION_LE(MAJOR, MINOR, PATCH) \
  ((CONFIGURATION_VERSION_MAJOR < MAJOR) ||           \
   (CONFIGURATION_VERSION_MAJOR ==                    \
    MAJOR&&(CONFIGURATION_VERSION_MINOR < MINOR || (CONFIGURATION_VERSION_MINOR == MINOR&& CONFIGURATION_VERSION_PATCH <= PATCH))))

/// Information about the current Configuration version.
class Version {
public:
  /// @return the current major version of ProjB.
  static int getMajor()
  {
    return CONFIGURATION_VERSION_MAJOR;
  }

  /// @return the current minor version of ProjB.
  static int getMinor()
  {
    return CONFIGURATION_VERSION_MINOR;
  }

  /// @return the current patch level of ProjB.
  static int getPatch()
  {
    return CONFIGURATION_VERSION_PATCH;
  }

  /// @return the current ProjB version (MM.mm.pp).
  static std::string getString()
  {
    std::ostringstream version;
    version << CONFIGURATION_VERSION_MAJOR << '.' << CONFIGURATION_VERSION_MINOR << '.' << CONFIGURATION_VERSION_PATCH;
    return version.str();
  }

  /// @return the VCS revision.
  static std::string getRevision()
  {
    return CONFIGURATION_VCS_REVISION;
  }

  /// @return the current ProjB version plus the VCS revision (MM.mm.pp.rev).
  static std::string getRevString()
  {
    std::ostringstream version;
    version << getString() << '.' << CONFIGURATION_VCS_REVISION;
    return version.str();
  }
};
}
}
}

#endif // CONFIGURATION_CONFIGURATION_VERSION_H
