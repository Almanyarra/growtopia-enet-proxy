#include "gt.hpp"
#include "packet.h"
#include "server.h"
#include "utils.h"
variantlist_t overlay{ "OnTextOverlay" };

std::string gt::flag = "tr";
std::string gt::version = "4.26";
std::string gt::mode = "pull";
std::string gt::saveworld = "";
std::string gt::name = "";
std::string gt::wldrop = "";
bool gt::resolving_uid2;
bool gt::connecting = false;
bool gt::in_game = false;
bool gt::wrench = false;
bool gt::fasttrash = false;
bool gt::fastdrop = false;
bool gt::taxsystem = false;
bool gt::autoacc = false;
bool gt::autounacc = false;
bool gt::autobgl = false;
bool gt::autopullwhenjoin = false;
bool gt::worldbanjoinmod = false;
bool gt::showxy = false;
bool gt::fastwheel = false;
bool gt::dropwl = false;
bool gt::dropdl = false;
bool gt::visualspin = false;
bool gt::dicespeed = false;
int gt::yuzde = 10;

void gt::send_log(std::string text) {
    g_server->send(true, "action|log\nmsg|" + text, NET_MESSAGE_GAME_MESSAGE);
}

void gt::send_overlay(std::string text) {
    overlay[1] = text;
    g_server->send(true, overlay);
}