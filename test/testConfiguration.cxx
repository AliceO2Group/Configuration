#include <Configuration.h>
#include <stdio.h>
#include <string>

int main () {

  configFile cfg;
  try {
    cfg.load("example.cfg"); 
  
    int v1;
    float v2;
    std::string v3;

    cfg.getValue("section 1.keyStr1",v3);
    printf("section 1.keyStr1 = %s\n",v3.c_str());
  
    cfg.getValue("section 1.keyInt1",v1);
    printf("section 1.keyInt1 = %d\n",v1);

    cfg.getValue("section 1.keyFloat1",v2);
    printf("section 1.keyFloat1 = %f\n",v2);

    cfg.getValue("section 2.keyFloat1",v2);
    printf("section 2.keyFloat1 = %f\n",v2);

    cfg.getValue("section 3.keyFloat1",v2);
    printf("section 2.keyFloat1 = %f\n",v2);


  }
  catch (std::string err) {
    printf("%s\n",err.c_str());
  }

  return 0;
}
