// Workaround: provide out-of-class definition for Panel_NV3041A::init_cmds.
// GCC < 9 requires this when a static constexpr array member is ODR-used
// (i.e. its address is taken at runtime, as in init_cmds[i]).
#include <lgfx/v1/panel/Panel_NV3041A.hpp>

namespace lgfx {
    namespace v1 {
        constexpr uint8_t Panel_NV3041A::init_cmds[];
    }
}
