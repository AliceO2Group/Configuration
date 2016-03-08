/*
 * ConfigurationInterface.cxx
 *
 *  Created on: Mar 8, 2016
 *      Author: pboescho
 */

#include <Configuration/ConfigurationInterface.h>
#include <boost/lexical_cast.hpp>

ConfigurationInterface::~ConfigurationInterface()
{
}

// Default implementations of non-string puts/gets, that use putString() and
// getString() + a lexical_cast

void ConfigurationInterface::putInt(std::string path, int value)
{
  putString(path, boost::lexical_cast<std::string>(value));
}

void ConfigurationInterface::putFloat(std::string path, double value)
{
  putString(path, boost::lexical_cast<std::string>(value));
}

int ConfigurationInterface::getInt(std::string path)
{
  return boost::lexical_cast<int>(getString(path));
}

double ConfigurationInterface::getFloat(std::string path)
{
  return boost::lexical_cast<double>(getString(path));
}

// Template convenience interface methods

template<> void ConfigurationInterface::put(std::string path, std::string value)
{
  putString(path, value);
}

template<> void ConfigurationInterface::put(std::string path, int value)
{
  putInt(path, value);
}

template<> void ConfigurationInterface::put(std::string path, double value)
{
  putFloat(path, value);
}

template<> std::string ConfigurationInterface::get(std::string path)
{
  return getString(path);
}

template<> int ConfigurationInterface::get(std::string path)
{
  return getInt(path);
}

template<> double ConfigurationInterface::get(std::string path)
{
  return getFloat(path);
}

