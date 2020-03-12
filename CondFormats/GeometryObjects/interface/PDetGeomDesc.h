#ifndef CondFormats_PDetGeomDesc_h
#define CondFormats_PDetGeomDesc_h

#include "CondFormats/Serialization/interface/Serializable.h"

#include <vector>
#include <string>

class PDetGeomDesc {
public:
  PDetGeomDesc(){};
  ~PDetGeomDesc(){};

  struct Item {
    // Translation
    double _dx, _dy, _dz;
    // fill as you will but intent is rotation matrix A where first number is row and second number is column
    double _a11, _a12, _a13, _a21, _a22, _a23, _a31, _a32, _a33;
    std::string _name;  // save only the name, not the namespace.
    std::vector<double> _params;
    uint32_t _geographicalID;  // to be converted to DetId
    int _copy;
    float _z;
    std::string _sensorType;

    COND_SERIALIZABLE;
  };

  std::vector<Item> container_;

  COND_SERIALIZABLE;
};

#endif
