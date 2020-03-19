#pragma once

#include <iostream>

namespace bsp {

class BacktrackingSP {
 public:
  BacktrackingSP();

  bool IsSAT(const void* nfc);
};

}