#include <string>

class configFilePrivate;

class configFile {
public:
  configFile();
  ~configFile();

  void load(const char *path);
  
  void getValue(const char *key, std::string &value); 
  void getValue(const char *key, int &value);
  void getValue(const char *key, float &value);

private:
  configFilePrivate *dPtr;
};

