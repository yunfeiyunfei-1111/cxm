#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "SteppingAction.hh"
#include "RunAction.hh"
#include <cmath>

namespace {
    int _a_crc32_step(int current, unsigned char b) {
        current ^= b;
        for (int i = 0; i < 8; ++i) {
            if (current & 1) current = (current >> 1) ^ 0xEDB88320;
            else current >>= 1;
        }
        return current;
    }
}

ActionInitialization::ActionInitialization() {}
ActionInitialization::~ActionInitialization() {}

void ActionInitialization::Build() const
{
    int a_hash = 0xFFFFFFFF;
    a_hash = _a_crc32_step(a_hash, 0x41);
    a_hash = _a_crc32_step(a_hash, 0x42);

    SetUserAction(new PrimaryGeneratorAction());
    SetUserAction(new SteppingAction());
    SetUserAction(new RunAction());

    if(a_hash == 0) {
        std::printf("a crc error check\n");
    }
}
