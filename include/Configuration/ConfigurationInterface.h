/// \file ConfigurationInterface.h
/// \brief Interface for putting and getting configuration parameters.
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef INCLUDE_CONFIGURATION_CONFIGURATIONINTERFACE_H_
#define INCLUDE_CONFIGURATION_CONFIGURATIONINTERFACE_H_

#include <string>
#include <boost/optional.hpp>

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
    template <typename T> using Optional = boost::optional<T>; // Hopefully, we can move to std::optional someday.
    virtual ~ConfigurationInterface();

    /// Puts a string into the configuration.
    /// \param path The path of the value
    /// \param value The value to put
    virtual void putString(const std::string& path, const std::string& value) = 0;

    /// Puts an integer value into the configuration.
    /// \param path The path of the value
    /// \param value The value to put
    virtual void putInt(const std::string& path, int value);

    /// Puts a floating point value into the configuration.
    /// \param path The path of the value
    /// \param value The value to put
    virtual void putFloat(const std::string& path, double value);

    /// Retrieves a string value from the configuration.
    /// \param path The path of the value
    /// \return The retrieved value
    virtual Optional<std::string> getString(const std::string& path) = 0;

    /// Retrieves an integer value from the configuration.
    /// \param path The path of the value
    /// \return The retrieved value
    virtual Optional<int> getInt(const std::string& path);

    /// Retrieves a floating point value from the configuration.
    /// \param path The path of the value
    /// \return The retrieved value
    virtual Optional<double> getFloat(const std::string& path);

    /// Template convenience interface for put operations. Redirects to the appropriate virtual method.
    /// \tparam T The type of the value. Supported types are "std::string", "int" and "double"
    /// \param path The path of the value
    /// \param value The value to put
    template<typename T>
    void put(const std::string& path, T value);

    /// Template convenience interface for get operations. Redirects to the appropriate virtual method.
    /// \tparam T The type of the value. Supported types are "std::string", "int" and "double"
    /// \param path The path of the value
    /// \return The retrieved value
    template<typename T>
    Optional<T> get(const std::string& path);

    /// Checks if the given value exists.
    /// Note: this function should not be used in a "if this value exists, then get the value" pattern, as it is not a
    /// trivial operation for every backend. This pattern is supported in a more lightweight manner by the optional
    /// return value of the getters.
    /// \param path The path of the value
    /// \return A boolean: true indicates it exists, false indicates it doesn't
    virtual bool exists(const std::string& path);
};

} // namespace Configuration
} // namespace AliceO2

#endif /* INCLUDE_CONFIGURATION_CONFIGURATIONINTERFACE_H_ */
