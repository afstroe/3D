#ifndef __APP_GLOBAL_STATE_H__
#define __APP_GLOBAL_STATE_H__

#include <eyeball/utils/defines.h>

class AppGlobalState
{
public:
  enum DebugMode
  {
    DM_DISABLED = 0,
    DM_WIREFRAME = 1 << 0,
    DM_SOLID_WIRE = 1 << 1,
    DM_RENDER_BOTH_FACES = 1 << 2,
  };

  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(DebugMode, debugMode)

public:
  inline static AppGlobalState& get()
  {
    static AppGlobalState state;
    return state;
  }
};

#endif // !__APP_GLOBAL_STATE_H__
