// brd_input.h - keyboard and mouse input

#ifndef __BRD_INPUT
#include "brd_shared.h"
#include "brd_console.h"
#define __BRD_INPUT

// SDL_NUM_SCANCODES - scancode array size

// BRD_Keyboard - contains a keyboard key array. Can load binds from file, change binds,
//                change key state (incl. through [] operator), return record contents if the bind is pressed

/*
bool BRD_IsKeyPressed(int scancode){
    bool res = keys[scancode];
    if (res == true) keys[scancode] = false;
    return res;
}
*/
struct BRD_KeyboardKey{
    //const SDL_Scancode scancode; // No need in that - scancode is already defined by the array index
    CON_Record *cmd = NULL;
    bool pressed = false;
};

class BRD_Keyboard{
private:
    struct BRD_KeyboardKey keys[SDL_NUM_SCANCODES];
public:
    BRD_Keyboard(){};
    // TODO: Binds loading from file
    //BRD_Keyboard(std::string& filepath);
    struct BRD_KeyboardKey& operator[](int scancode){return keys[scancode];};
    void Toggle(int scancode){keys[scancode].pressed = !keys[scancode].pressed;}
    void ChangeState(int scancode, bool new_state){keys[scancode].pressed = new_state;}
    CON_Record* GetRecord(int scancode){return keys[scancode].cmd;}
};

extern BRD_Keyboard keys[SDL_NUM_SCANCODES];

#endif // __BRD_INPUT
