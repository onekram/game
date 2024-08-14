#pragma once


struct input {
    bool up;
    bool down;
    bool left;
    bool right;

    static input get_default_input();
};
