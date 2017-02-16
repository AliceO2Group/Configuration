/// \file FileBackend.cxx
/// \brief Configuration interface for files. Port of Configuration.h & Configuration.cxx
///
/// \author Sylvain Chapeland, CERN
/// \author Pascal Boeschoten, CERN

#include "FileBackend.h"
#include <vector>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace AliceO2
{
namespace Configuration
{
namespace Backends
{

FileBackend::~FileBackend()
{
}

#define SUFFIX_FILE_INI {".ini", ".cfg"}

/// Load the configuration from given path
/// \param path  Path to configuration data.
///              Example: file:/configDir/example.cfg
///              Accepted prefix:
///                 file:     for a file accessible from the filesystem
///              Accepted suffix, to define file format (TODO: separate doc for file formats):
///                 .ini, .cfg    see example.cfg
/// \exception   Throws a <std::string> exception on error.
void loadConfigFile(const std::string& filePath, boost::property_tree::ptree& pt)
{
  if (filePath.length() == 0) { throw std::runtime_error("Invalid argument"); }

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

FileBackend::FileBackend(const std::string& filePath)
    : mFilePath(filePath)
{
  loadConfigFile(filePath, mPropertyTree);
}

void FileBackend::putString(const std::string&, const std::string&)
{
  throw std::runtime_error("FileBackend does not support putting values");
}

auto FileBackend::getString(const std::string& path) -> Optional<std::string>
{
  // To use a custom separator instead of the default '.', we need to construct the path_type object explicitly
  return mPropertyTree.get<std::string>(decltype(mPropertyTree)::path_type(path, getSeparator()));
}

void FileBackend::setPrefix(const std::string& path)
{
  mFilePath = path;
  loadConfigFile(mFilePath, mPropertyTree);
}

} // namespace Configuration
} // namespace Backends
} // namespace AliceO2
