/// \file ConfigurationInterface.h
/// \brief Interface for putting and getting configuration parameters.
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_CONFIGURATION_INCLUDE_CONFIGURATIONINTERFACE_H_
#define ALICEO2_CONFIGURATION_INCLUDE_CONFIGURATIONINTERFACE_H_

#include <string>
#include <unordered_map>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

namespace o2
{
namespace configuration
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
using KeyValueMap = std::unordered_map<std::string, std::string>;
class ConfigurationInterface
{
  public:
    virtual ~ConfigurationInterface();

    /// Puts a string into the configuration.
    /// \param path The path of the value
    /// \param value The value to put
    virtual void putString(const std::string& path, const std::string& value) = 0;

    /// Retrieves a string value from the configuration.
    /// \param path The path of the value
    /// \return The retrieved value
    virtual boost::optional<std::string> getString(const std::string& path) = 0;

    /// Template convenience interface for put operations. Redirects to the appropriate virtual method.
    /// \param T The type of the value. Supported types are "std::string", "int" and "double"
    /// \param path The path of the value
    /// \param value The value to put
    template<typename T>
    void put(const std::string& path, const T& value);

    /// Template convenience interface for get operations.
    /// \param path The path of the value
    /// \return The retrieved value. Supported types are "std::string", "int" and "double"
    /// \throw std::runtime_error when value does not exist
    template<typename T>
    T get(const std::string& path) throw(std::runtime_error);

    /// Template convenience interface for get operations with additiona parameter default value
    /// \param path The path of the value
    /// \param defaultValue default value which is assigned when requested key does not exist
    template<typename T>
    T get(const std::string& path, const T& defaultValue);

    /// Sets a 'prefix' or 'directory' for the backend.
    /// After this call, all paths given to this object will be prefixed with this.
    /// The implementation of this is very backend-dependent and it may not be a trivial call.
    /// \param prefix The prefix path
    virtual void setPrefix(const std::string& prefix) = 0;

    /// Gets key-values recursively from the given path
    /// \param path The path of the values to get
    /// \return A map containing the key-values
    virtual KeyValueMap getRecursiveMap(const std::string& path) = 0;

    /// Provides subtree from given path
    /// \param path The path to the subtree
    /// \return Subtree
    virtual boost::property_tree::ptree getRecursive(const std::string& path) = 0;
};

} // namespace configuration
} // namespace o2

#endif /* ALICEO2_CONFIGURATION_INCLUDE_CONFIGURATIONINTERFACE_H_ */
