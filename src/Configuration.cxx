#include <Configuration.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <iostream>
using namespace std;

class configFilePrivate {
  public:
  configFilePrivate();
  ~configFilePrivate();
  
  friend class configFile;
  protected:
    boost::property_tree::ptree pt;    
    
};

configFilePrivate::configFilePrivate(){
}

configFilePrivate::~configFilePrivate(){
}


configFile::configFile(){
  dPtr=new configFilePrivate();
  if (dPtr==NULL) {
    std::stringstream ss;
    ss << "new() error in " << __FILE__ << " line " << __LINE__;
    throw ss.str();
  }
}

configFile::~configFile(){
  if (dPtr!=NULL) {
    delete dPtr;
  }
}

void configFile::load(const char *path) {
  try {
    boost::property_tree::ini_parser::read_ini(path, dPtr->pt);
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
}
  
void configFile::getValue(const char *key,std::string &value){
  value=dPtr->pt.get<std::string>(key);
}

void configFile::getValue(const char *key, int &value){
 value=dPtr->pt.get<int>(key);
}

void configFile::getValue(const char *key, float &value){
  try {
    value=dPtr->pt.get<float>(key);
 }
 catch(const boost::property_tree::ptree_error &e)
    {
        throw std::string(e.what());
    }
  }
