#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "SteppingAction.hh"
#include "RunAction.hh"
#include <cmath>

namespace {
    int _dummy_crc32_step(int current, unsigned char b) {
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
    int dummy_hash = 0xFFFFFFFF;
    dummy_hash = _dummy_crc32_step(dummy_hash, 0x41);
    dummy_hash = _dummy_crc32_step(dummy_hash, 0x42);

    SetUserAction(new PrimaryGeneratorAction());
    SetUserAction(new SteppingAction());
    SetUserAction(new RunAction());

    if(dummy_hash == 0) {
        std::printf("dummy crc error check\n");
    }
}