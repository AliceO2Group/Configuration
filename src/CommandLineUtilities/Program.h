/// \file Program.h
/// \brief Definition of the Program class.
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_FLPPROTOTYPE_CONFIGURATION_PROGRAM_H_
#define ALICEO2_FLPPROTOTYPE_CONFIGURATION_PROGRAM_H_

#include <atomic>
#include <boost/program_options.hpp>

namespace o2
{
namespace configuration
{

/// Helper class for making a command-line program. It handles:
/// - Creation of the options_descripotion object
/// - Creation of the variables_map object
/// - Help message
/// - Exceptions & error messages
class Program
{
  public:
    Program();
    virtual ~Program();

    /// Execute the program using the given arguments
    int execute(int argc, char** argv);

  protected:
    /// Should output be verbose
    bool isVerbose() const
    {
      return mVerbose;
    }

    struct Description
    {
        std::string name; ///< Name of the utility
        std::string description; ///< Short description
        std::string usage; ///< Usage example
    };

  private:
    /// Get the description of the program
    /// \return A tuple containing name, description and example usage of the program
    virtual Description getDescription() = 0;

    /// Add the program's options
    virtual void addOptions(boost::program_options::options_description& optionsDescription) = 0;

    /// The main function of the program
    virtual void run(const boost::program_options::variables_map& variablesMap) = 0;

    void printHelp(const boost::program_options::options_description& optionsDescription);
    bool mVerbose;
};

} // namespace configuration
} // namespace o2

#endif // PROGRAM
