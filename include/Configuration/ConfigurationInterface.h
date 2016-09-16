/// \file ConfigurationInterface.h
/// \brief Interface for putting and getting configuration parameters.
///
/// \author Pascal Boeschoten, CERN

#ifndef INCLUDE_CONFIGURATION_CONFIGURATIONINTERFACE_H_
#define INCLUDE_CONFIGURATION_CONFIGURATIONINTERFACE_H_

#include <string>

namespace AliceO2
{
namespace Configuration
{

/// \brief Interface for configuration back ends.
///
/// Interface for configuration back ends, to put and get configuration parameters.
///
/// The non-string put/get are not pure virtual and have default implementations that use the string put/get and
/// boost::lexical_cast for conversion. This is because most back ends convert to/from strings anyway.
///
/// To maintain runtime polymorphism, while also having a convenient template-like interface,
/// the implementations of put/get operations are in the virtual methods,
/// and template methods are provided that redirect to these, functioning as a kind of syntactic sugar.
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

    /// Template convenience interface for put operations. Redirects to the appropriate virtual method.
    template<typename T>
    void put(std::string path, T value);

    /// Template convenience interface for get operations. Redirects to the appropriate virtual method.
    template<typename T>
    T get(std::string path);
};

}
}

#endif /* INCLUDE_CONFIGURATION_CONFIGURATIONINTERFACE_H_ */
