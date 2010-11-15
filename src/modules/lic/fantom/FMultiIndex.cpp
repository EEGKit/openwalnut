//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// these lines where created by a python script, do not edit
#include "_OWmodule_lic_PCH.h"

#include "FMultiIndex.h"

std::ostream& operator<<(std::ostream& os, const FMultiIndex& mi)
{
  os << "[ ";
  for(unsigned int i=0; i< mi.order(); ++i)
  {
    os << mi[i] << " ";
  }
  os << "]";
  return os;
}

