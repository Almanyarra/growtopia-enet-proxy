#pragma once
#include <string>

namespace gt {
    extern std::string flag;
    extern std::string version;
    extern std::string mode;
    extern std::string saveworld;
    extern std::string name;
    extern std::string wldrop;
    extern bool resolving_uid2;
    extern bool connecting;
    extern bool in_game;
    extern bool wrench;
    extern bool fasttrash;
    extern bool fastdrop;
    extern bool taxsystem;
    extern bool autoacc;
    extern bool autounacc;
    extern bool autobgl;
    extern bool autopullwhenjoin;
    extern bool worldbanjoinmod;
    extern bool showxy;
    extern bool fastwheel;
    extern bool dropwl;
    extern bool dropdl;
    extern bool visualspin;
    extern bool dicespeed;
    extern int yuzde;
    void send_log(extern std::string text);
    void send_overlay(extern std::string text);
}