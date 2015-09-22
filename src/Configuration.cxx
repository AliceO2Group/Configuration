/// \file Configuration.h
/// \brief C++ interface to retrieve configuration parameters.
///
/// \author Sylvain Chapeland, CERN

#include <Configuration/Configuration.h>
#include <iostream>
#include <vector>
#include <boost/algorithm/string/predicate.hpp>

using namespace std;

ConfigFilePrivate::ConfigFilePrivate()
{
}

ConfigFilePrivate::~ConfigFilePrivate()
{
}


ConfigFile::ConfigFile()
{
  dPtr = new ConfigFilePrivate();
  if (dPtr == NULL) {
    std::stringstream ss;
    ss << "new() error in " << __FILE__ << " line " << __LINE__;
    throw ss.str();
  }
}

ConfigFile::~ConfigFile()
{
  if (dPtr != NULL) {
    delete dPtr;
  }
}

#define PREFIX_FILE "file:"
#define SUFFIX_FILE_INI {".ini", ".cfg"}

void ConfigFile::load(const std::string path)
{
  if (path.length() == 0) { throw std::string("Invalid argument"); }

  //
  // open location according to prefix
  //


  // filesystem file
  if (boost::algorithm::starts_with(path.c_str(), PREFIX_FILE)) {
    const char *filename;
    filename = &path[strlen(PREFIX_FILE)];

    // TODO: filter out comments in file with boost filtering_stream

    //
    // parse file according to suffix
    //

    // INI file
    std::vector<const char*> suffix_ini SUFFIX_FILE_INI;
    for (auto suffix : suffix_ini) {
      if (boost::algorithm::ends_with(filename, suffix)) {
        try {
          boost::property_tree::ini_parser::read_ini(filename, dPtr->pt);
        }
        catch (boost::property_tree::ini_parser::ini_parser_error perr) {
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
    throw std::string("Invalid type in file name");
  } else {
    throw std::string("Invalid path prefix");
  }
}

void getValueSpecialization(void) {
  ConfigFile f;
  const std::string s("");
  int vInt;
  f.getValue(s,vInt);
  vInt=f.getValue<int>(s);
  float vFloat;
  f.getValue("",vFloat);
  vFloat=f.getValue<float>(s);
  std::string vString;
  f.getValue("",vString);
  vString=f.getValue<std::string>(s);
}


//template void ConfigFile::getValue<float>(std::string, float&);
