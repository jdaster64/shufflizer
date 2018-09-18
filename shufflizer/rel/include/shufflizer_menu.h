#pragma once

#include <cstdint>

namespace mod::shufflizer {
    
class ShufflizerOptions;

class ShufflizerMenu {
public:
    ShufflizerMenu(ShufflizerOptions* options);
    
    // Updates player input.
    void Update();
    // Updates graphical overlay.
    void Draw();
    
private:
    // Unowned pointer to options struct.
    ShufflizerOptions* options_;
};

}