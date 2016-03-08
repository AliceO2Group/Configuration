/// \file ConfigurationInterface.h
/// \brief Interface for getting and setting configuration parameters.
///
/// \author Pascal Boeschoten, CERN

#ifndef INCLUDE_CONFIGURATION_CONFIGURATIONINTERFACE_H_
#define INCLUDE_CONFIGURATION_CONFIGURATIONINTERFACE_H_

#include <string>

class ConfigurationInterface
{
  public:
    virtual ~ConfigurationInterface();

    virtual void putString(std::string path, std::string value) = 0;
    virtual void putInt(std::string path, int value);
    virtual void putFloat(std::string path, double value);

    virtual std::string getString(std::string path) = 0;
    virtual int getInt(std::string path);
    virtual double getFloat(std::string path);

    // Template convenience interface for put operations
    template<typename T>
    void put(std::string path, T value);

    // Template convenience interface for get operations
    template<typename T>
    T get(std::string path);
};

#endif /* INCLUDE_CONFIGURATION_CONFIGURATIONINTERFACE_H_ */
