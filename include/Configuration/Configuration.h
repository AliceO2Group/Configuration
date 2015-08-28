/// \file Configuration.h
/// \brief C++ interface to retrieve configuration parameters.
///
/// \author Sylvain Chapeland, CERN
#include <string>

class ConfigFilePrivate;

class ConfigFile {
public:
  ConfigFile();
  ~ConfigFile();

  /// Load the configuration from given path
  /// \param path  Path to configuration data.
  ///              Example: file:/configDir/example.cfg
  ///              Accepted prefix:
  ///                 file:     for a file accessible from the filesystem
  ///              Accepted suffix, to define file format (TODO: separate doc for file formats):
  ///                 .ini, .cfg    see example.cfg
  /// \exception   Throws a <std::string> exception on error.
  void load(const std::string path);


  /// Get the configuration value for given key path (by reference)
  /// \param key   Key name (possibly hierarchical)
  /// \param value Result value found (possible types: int, float, std::string), by reference (this variable is modified in case of success)
  /// \returns     Nothing
  /// \exception   Throws a <std::string> exception on error.
  template <typename T>
  void getValue(const std::string key, T &value);


  /// Get the configuration value for given key path (by result)
  /// \param key   Key name (possibly hierarchical)
  /// \returns     Result value found (possible types: int, float, std::string)
  /// \exception   Throws a <std::string> exception on error.
  template <typename T>
  T getValue(const std::string key);


private:
    ConfigFilePrivate *dPtr;
};
