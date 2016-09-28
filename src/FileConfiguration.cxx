/// \file FileConfiguration.cxx
/// \brief Configuration interface for files. Port of Configuration.h & Configuration.cxx
///
/// \author Sylvain Chapeland, CERN
/// \author Pascal Boeschoten, CERN

#include "FileConfiguration.h"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace AliceO2
{
namespace Configuration
{

FileConfiguration::~FileConfiguration()
{
}

//#define PREFIX_FILE "file:"
#define SUFFIX_FILE_INI {".ini", ".cfg"}

/// Load the configuration from given path
/// \param path  Path to configuration data.
///              Example: file:/configDir/example.cfg
///              Accepted prefix:
///                 file:     for a file accessible from the filesystem
///              Accepted suffix, to define file format (TODO: separate doc for file formats):
///                 .ini, .cfg    see example.cfg
/// \exception   Throws a <std::string> exception on error.
void loadConfigFile(const std::string filePath, boost::property_tree::ptree& pt)
{
  if (filePath.length() == 0) { throw std::string("Invalid argument"); }

  //
  // open location according to prefix
  //

  // TODO: filter out comments in file with boost filtering_stream

  //
  // parse file according to suffix
  //

  // INI file
  std::vector<const char*> suffix_ini SUFFIX_FILE_INI;
  for (auto suffix : suffix_ini) {
    if (boost::algorithm::ends_with(filePath, suffix)) {
      try {
        boost::property_tree::ini_parser::read_ini(filePath, pt);
      }
      catch (const boost::property_tree::ini_parser::ini_parser_error& perr) {
        std::stringstream ss;
        if (perr.line()) {
          ss << perr.message() << " in " << perr.filename() << " line " << perr.line();
        } else {
          ss << perr.message() << " " << perr.filename();
        }
        throw ss.str();
      }
      return;
    }
  }
  throw std::runtime_error("Invalid type in file name");
}

FileConfiguration::FileConfiguration(std::string filePath)
    : filePath(filePath)
{
  loadConfigFile(filePath, pt);
}

void FileConfiguration::putString(const std::string&, const std::string&)
{
  throw std::runtime_error("FileConfiguration does not support putting values");
}

auto FileConfiguration::getString(const std::string& path) -> Optional<std::string>
{
  // To use a '/' instead of the default '.' as separator, we need to construct the path object explicitly
  //return pt.get<std::string>(decltype(pt)::path_type(path, '/'));
  return pt.get<std::string>(path);
}

} // namespace Configuration
} // namespace AliceO2
