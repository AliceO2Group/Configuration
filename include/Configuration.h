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
  void load(const char *path);


  /// Get  the configuration from given path
  /// \param key   key name (possibly hierarchical)
  /// \param value Result (std::string) value found, by reference (this variable is modified in case of success)
  /// \exception   Throws a <std::string> exception on error.
  void getValue(const char *key, std::string &value); 
  
  /// Get  the configuration from given path
  /// \param key   key name (possibly hierarchical)
  /// \param value Result (int) value found, by reference (this variable is modified in case of success)
  /// \exception   Throws a <std::string> exception on error.
  void getValue(const char *key, int &value);
  
  /// Get  the configuration from given path
  /// \param key   key name (possibly hierarchical)
  /// \param value Result (float) value found, by reference (this variable is modified in case of success)
  /// \exception   Throws a <std::string> exception on error.
  void getValue(const char *key, float &value);

private:
    ConfigFilePrivate *dPtr;
};

