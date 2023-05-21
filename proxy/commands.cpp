#include "commands.h"
#include "gt.hpp"
#include "proton/hash.hpp"
#include "proton/rtparam.hpp"
#include "proton/variant.hpp"
#include "server.h"
#include "utils.h"
#include <thread>
#include <limits.h>
#include "HTTPRequest.hpp"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "dialog.h"
#include "math.h"
#include <iostream>

bool events::out::variantlist(gameupdatepacket_t *packet)
{
    variantlist_t varlist{};
    varlist.serialize_from_mem(utils::get_extended(packet));
    PRINTS("varlist: %s\n", varlist.print().c_str());
    return false;
}
std::vector<std::string> split(const std::string &str, const std::string &delim)
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos)
            pos = str.length();
        std::string token = str.substr(prev, pos - prev);
        if (!token.empty())
            tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}
bool events::out::pingreply(gameupdatepacket_t *packet)
{
    // since this is a pointer we do not need to copy memory manually again
    packet->m_type = 21;
    packet->m_vec2_x = 1000.f;  // gravity
    packet->m_vec2_y = 250.f;   // move speed
    packet->m_vec_x = 64.f;     // punch range
    packet->m_vec_y = 64.f;     // build range
    packet->m_jump_amount = 0;  // for example unlim jumps set it to high which causes ban
    packet->m_player_flags = 0; // effect flags. good to have as 0 if using mod noclip, or etc.
    return false;
}

using namespace gt;

bool find_command(std::string chat, std::string name)
{
    bool found = chat.find("/" + name) == 0;
    if (found)
        gt::send_log("`6" + chat);
    return found;
}

bool itsmod(int netid, std::string growid = "")
{
    if (netid == g_server->m_world.local.netid)
        return false;
    else if (growid.find(g_server->m_world.local.name) != std::string::npos)
        return false;
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    for (auto g : g_server->m_world.players)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(5));
        std::transform(g.name.begin(), g.name.end(), g.name.begin(), [](unsigned char c)
                       { return std::tolower(c); });
        if (netid == g.netid)
            return false;
        else if (growid.find(g.name) != std::string::npos)
            return false;
    }
    gt::send_overlay("`@@Mod``or `1@Guardian`` Joined The `wWorld.");
    g_server->send(false, "action|input\n|text|/unaccess", 3);
    g_server->send(false, "action|join_request\nname|exit", 3);
    return true;
}
bool events::out::generictext(std::string packet)
{
    PRINTS("Generic text: %s\n", packet.c_str());
    auto &world = g_server->m_world;
    rtvar var = rtvar::parse(packet);
    if (!var.valid())
        return false;
    if (packet.find("fastdrop|") != -1)
    {
        try
        {
            std::string aaa = packet.substr(packet.find("rop|") + 4, packet.size());
            std::string number = aaa.c_str();
            fastdrop = stoi(number);
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");
        }
    }
    if (packet.find("fastwheel|") != -1)
    {
        try
        {
            std::string aaa = packet.substr(packet.find("eel|") + 4, packet.size());
            std::string number = aaa.c_str();
            fastwheel = stoi(number);
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");
        }
    }
    if (packet.find("autobgl|") != -1) {
        try
        {
            std::string aaa = packet.substr(packet.find("bgl|") + 4, packet.size());
            std::string number = aaa.c_str();
            autobgl = stoi(number);
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");
        }
    }
    if (packet.find("worldbanmod|") != -1)
    {
        try
        {
            std::string aaa = packet.substr(packet.find("mod|") + 4, packet.size());
            std::string number = aaa.c_str();
            worldbanjoinmod = stoi(number);
            if (worldbanjoinmod == true)
            {
                gt::send_log("`9Successfuly started checking entered for `#@Moderators `9and `#@Guardians`9, please wait...");
            }
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");
        }
    }
    if (packet.find("autotax|") != -1)
    {
        try
        {
            std::string aaa = packet.substr(packet.find("tax|") + 4, packet.size());
            std::string number = aaa.c_str();
            taxsystem = stoi(number);
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");
        }
    }
    if (packet.find("dicespeed|") != -1)
    {
        try
        {
            std::string aaa = packet.substr(packet.find("eed|") + 4, packet.size());
            std::string number = aaa.c_str();
            dicespeed = stoi(number);
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");
        }
    }
    if (packet.find("saveworld|") != -1)
    {
        try
        {
            std::string aaa = packet.substr(packet.find("rld|") + 4, packet.size());
            while (!aaa.empty() && isspace(aaa[aaa.size() - 1]))
                aaa.erase(aaa.end() - (76 - 0x4B));
            saveworld = aaa;
        }
        catch (exception a)
        {
        }
    }
    if (packet.find("taxamount|") != -1)
    {
        try
        {
            std::string aaa = packet.substr(packet.find("unt|") + 4, packet.size());
            std::string number = aaa.c_str();
            yuzde = stoi(number);
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");
        }
    }
    if (packet.find("autoacc|") != -1)
    {
        try
        {
            std::string aaa = packet.substr(packet.find("acc|") + 4, packet.size());
            std::string number = aaa.c_str();
            autoacc = stoi(number);
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");
        }
    }
    if (packet.find("showxy|") != -1)
    {
        try
        {
            std::string aaa = packet.substr(packet.find("wxy|") + 4, packet.size());
            std::string number = aaa.c_str();
            showxy = stoi(number);
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");
        }
    }
    if (packet.find("saveworld|") != -1)
    {
        try
        {
            std::string aaa = packet.substr(packet.find("rld|") + 4, packet.size());
            std::string number = aaa.c_str();
            std::string saveworld = number;
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");
        }
    }
    if (packet.find("fasttrash|") != -1)
    {
        try
        {
            std::string aaa = packet.substr(packet.find("ash|") + 4, packet.size());
            std::string number = aaa.c_str();
            fasttrash = stoi(number);
        }
        catch (exception a)
        {
            gt::send_log("`4Critical Error: `2override detected");
        }
    }
    if (wrench == true)
    {
        if (packet.find("action|wrench") != -1)
        {
            g_server->send(false, packet);
            std::string mega = packet.substr(packet.find("netid|") + 6, packet.length() - packet.find("netid|") - 1);
            std::string mini = mega.substr(0, mega.find("|"));
            if (mode.find("pull") != -1)
            {
                try
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + mini + "|\nnetID|" + mini + "|\nbuttonClicked|pull");
                    gt::send_overlay("`2Successfully `4Pulled ");
                }
                catch (exception a)
                {
                    gt::send_overlay("`4Pull Error``");
                }
            }
            if (mode.find("kick") != -1)
            {
                try
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + mini + "|\nnetID|" + mini + "|\nbuttonClicked|kick");
                    gt::send_overlay("`2Successfully `4Kicked ");
                }
                catch (exception a)
                {
                    gt::send_overlay("`4Kick Error``");
                }
            }
            if (mode.find("ban") != -1)
            {
                try
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + mini + "|\nnetID|" + mini + "|\nbuttonClicked|worldban");
                    gt::send_overlay("`2Successfully `4Banned ");
                }
                catch (exception a)
                {
                    gt::send_overlay("`4Pull Error``");
                }
            }
            return true;
        }
    }
    if (var.get(0).m_key == "action" && var.get(0).m_value == "input")
    {
        if (var.size() < 2)
            return false;
        if (var.get(1).m_values.size() < 2)
            return false;

        if (!world.connected)
            return false;

        auto chat = var.get(1).m_values[1];
        if (find_command(chat, "name "))
        {
            std::string name = "``" + chat.substr(6) + "``";
            variantlist_t va{"OnNameChanged"};
            va[1] = name;
            g_server->send(true, va, world.local.netid, -1);
            gt::send_log("name set to: " + name);
            return true;
        }
        else if (find_command(chat, "flag "))
        {
            int flag = atoi(chat.substr(6).c_str());
            variantlist_t va{"OnGuildDataChanged"};
            va[1] = 1;
            va[2] = 2;
            va[3] = flag;
            va[4] = 3;
            g_server->send(true, va, world.local.netid, -1);
            gt::send_log("flag set to item id: " + std::to_string(flag));
            return true;
        }
        else if (find_command(chat, "fd"))
        {
            fastdrop = !fastdrop;
            if (fastdrop)
                gt::send_log("`9Fast Drop`` is now `2ENABLED");
            else
                gt::send_log("`9Fast Drop`` is now `4DISABLED");
            return true;
        }
        else if (find_command(chat, "ft"))
        {
            fasttrash = !fasttrash;
            if (fasttrash)
                gt::send_log("`9Fast Trash`` is now `2ENABLED");
            else
                gt::send_log("`9Fast Trash`` is now `4DISABLED");
            return true;
        }
        else if (find_command(chat, "wrench"))
        {
            wrench = !wrench;
            if (wrench)
                gt::send_log("`9Wrench Mode`` is now `2ENABLED");
            else
                gt::send_log("`9Wrench Mode`` is now `4DISABLED");
            return true;
        }
        else if (find_command(chat, "wset "))
        {
            mode = chat.substr(5);
            gt::send_log("`9Wrench mode`` set to `2" + mode);
            return true;
        }
        else if (find_command(chat, "cdrop "))
        {
            wldrop = chat.substr(7);
            int sayi = stoi(wldrop);
            int dl = (sayi / 100);
            int wl = (sayi % 100);
            if (wl >= 1 && dl == 0)
            {
                dropwl = true;
                g_server->send(false, "action|drop\n|itemID|242");
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + to_string(wl)); // 242
            }
            else if (wl == 0 && dl >= 1)
            {
                dropdl = true;
                g_server->send(false, "action|drop\n|itemID|1796");
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + to_string(dl));
            }
            else if (wl >= 1 && dl >= 1)
            {
                dropwl = true;
                g_server->send(false, "action|drop\n|itemID|242");
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + to_string(wl));
                dropdl = true;
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                g_server->send(false, "action|drop\n|itemID|1796");
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + to_string(dl));
            }
            gt::send_log("`9Dropping `2" + to_string(dl) + "DL`9 & `2" + to_string(wl) + "WL");
            return true;
        }
        else if (find_command(chat, "warp "))
        {
            std::string name = chat.substr(5);
            gt::send_log("`9Warping ``to `2" + name);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            g_server->send(false, "action|join_request\nname|" + name, 3);
            return true;
        }
        else if (find_command(chat, "save"))
        {
            if (saveworld == "")
            {
                gt::send_overlay("`4Save World `@Is Not Changed Change in /settings");
            }
            else
            {
                gt::send_overlay("`9Going to `4Save World");
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                g_server->send(false, "action|join_request\nname|" + saveworld, 3);
                return true;
            }
        }
        else if (find_command(chat, "pall"))
        {
            std::string username = chat.substr(3);
            for (auto &player : g_server->m_world.players)
            {
                auto name_2 = player.name.substr(2);
                if (name_2.find(username))
                {
                    g_server->send(false, "action|wrench\n|netid|" + std::to_string(player.netid));
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + std::to_string(player.netid) + "|\nbuttonClicked|pull");
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    gt::send_log("Pulled");
                }
            }
            return true;
        }
        else if (find_command(chat, "kall"))
        {
            std::string username = chat.substr(3);
            for (auto &player : g_server->m_world.players)
            {
                auto name_2 = player.name.substr(2);
                if (name_2.find(username))
                {
                    g_server->send(false, "action|wrench\n|netid|" + std::to_string(player.netid));
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + std::to_string(player.netid) + "|\nbuttonClicked|kick");
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    gt::send_log("Kicked All");
                }
            }
            return true;
        }
        else if (find_command(chat, "ball"))
        {
            std::string username = chat.substr(3);
            for (auto &player : g_server->m_world.players)
            {
                auto name_2 = player.name.substr(2);
                if (name_2.find(username))
                {
                    g_server->send(false, "action|wrench\n|netid|" + std::to_string(player.netid));
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + std::to_string(player.netid) + "|\nbuttonClicked|worldban");
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    gt::send_log("World Banned All");
                }
            }
            return true;
        }
        else if (find_command(chat, "skin "))
        {
            int skin = atoi(chat.substr(6).c_str());
            variantlist_t va{"OnChangeSkin"};
            va[1] = skin;
            g_server->send(true, va, world.local.netid, -1);
            return true;
        }
        else if (find_command(chat, "pwrench "))
        {
            std::string name = chat.substr(6);
            std::string username = ".";
            for (auto &player : g_server->m_world.players)
            {
                auto name_2 = player.name.substr(2);
                std::transform(name_2.begin(), name_2.end(), name_2.begin(), ::tolower);
                g_server->send(false, "action|wrench\n|netid|" + std::to_string(player.netid));
            }
            return true;
        }
        else if (find_command(chat, "csn"))
        {
            Dialog casino;
            casino.addLabelWithIcon("Casino Options", 758, LABEL_BIG);
            casino.addSpacer(SPACER_SMALL);
            casino.addCheckbox("autotax", "Enable Auto Tax", taxsystem);
            casino.addInputBox("taxamount", "Tax Amount: %", std::to_string(yuzde), 2);
            casino.addCheckbox("fastwheel", "Fast Wheel", fastwheel);
            casino.addCheckbox("autobgl", "Auto Bgl", autobgl);
            casino.addCheckbox("showxy", "Enable Show X,Y Position", showxy);
            casino.addCheckbox("worldbanmod", "Exit world when mod joins", worldbanjoinmod);
            casino.addCheckbox("dicespeed", "Dice Speed", dicespeed);
            casino.addQuickExit();
            casino.endDialog("end", "OK", "Cancel");
            variantlist_t liste{"OnDialogRequest"};
            liste[1] = casino.finishDialog();
            g_server->send(true, liste);
            return true;
        }
        else if (find_command(chat, "settings"))
        {
            std::string paket;
            Dialog a;
            a.addLabelWithIcon("Proxy Options Page", 758, LABEL_BIG);
            a.addSpacer(SPACER_SMALL);
            a.addInputBox("saveworld", "Save World:", saveworld, 12);
            a.addCheckbox("fasttrash", "Fast Trash", fasttrash);
            a.addCheckbox("fastdrop", "Fast Drop", fastdrop);
            a.addCheckbox("wrench", "Wrench Mode", wrench);
            a.addCheckbox("worldbanmod", "Exit world when mod joins", worldbanjoinmod);
            a.endDialog("end", "Save", "Close");
            variantlist_t liste{"OnDialogRequest"};
            liste[1] = a.finishDialog();
            g_server->send(true, liste);
            return true;
        }
        else if (find_command(chat, "proxy"))
        {
            std::string paket;
            Dialog a;
            a.addLabelWithIcon("Proxy Commands", 32, LABEL_BIG);
            a.addSpacer(SPACER_SMALL);
            a.addTextBox("`9/proxy `#(shows general commands)");
            a.addTextBox("`9/settings `#(you can edit proxy options)");
            a.addTextBox("`9/csn `#(shows csn commands)");
            a.addTextBox("`9/auto `#(shows auto settings)");
            a.addTextBox("`9/wp [naem] `#(warp's the world)");
            a.addTextBox("`9/fd `#(enable fast drop)");
            a.addTextBox("`9/ft `#(enable fast trash)");
            a.addTextBox("`9/name [name] `#(sets player name {visual})");
            a.addTextBox("`9/wrench `#(open wrench mode)");
            a.addTextBox("`9/wset [pull, kick, ban] `#(sets wrench mode type)");
            a.addQuickExit();
            a.endDialog("end", "Save", "Close");
            variantlist_t liste{"OnDialogRequest"};
            liste[1] = a.finishDialog();
            g_server->send(true, liste);
            return true;
        }
        else if (find_command(chat, "auto"))
        {
            std::string paket;
            Dialog a;
            a.addLabelWithIcon("Auto Page", 32, LABEL_BIG);
            a.addSpacer(SPACER_SMALL);
            a.addCheckbox("autopullwhenjoin", "Enable Auto Pull When Join", autopullwhenjoin);
            a.addQuickExit();
            a.endDialog("end", "Save", "Close");
            variantlist_t liste{"OnDialogRequest"};
            liste[1] = a.finishDialog();
            g_server->send(true, liste);
            return true;
        }
        return false;
    }

    if (packet.find("game_version|") != -1)
    {
        rtvar var = rtvar::parse(packet);
        auto mac = utils::generate_mac();
        if (g_server->m_server == "213.179.209.168")
        {
            rtvar var1;
            using namespace httplib;
            Headers Header;
            Header.insert(std::make_pair("User-Agent", "UbiServices_SDK_2019.Release.27_PC64_unicode_static"));
            Header.insert(std::make_pair("Host", "www.growtopia1.com"));
            Client cli("https://104.125.3.135");
            cli.set_default_headers(Header);
            cli.enable_server_certificate_verification(false);
            cli.set_connection_timeout(2, 0);
            auto res = cli.Post("/growtopia/server_data.php");
            std::cout << res << std::endl;
            if (res.error() == Error::Success)
                var1 = rtvar::parse({res->body});
            g_server->meta = (var1.find("meta") ? var1.get("meta") : (g_server->meta = var1.get("meta")));
        }
        var.set("game_version", version);
        var.set("meta", g_server->meta);
        var.set("country", flag);
        packet = var.serialize();
        in_game = false;
        PRINTS("Login\n");
        g_server->send(false, packet);
        return true;
    }

    return false;
}

bool events::out::gamemessage(std::string packet)
{
    PRINTS("Game message: %s\n", packet.c_str());
    if (packet == "action|quit")
    {
        g_server->quit();
        return true;
    }

    return false;
}
bool events::out::state(gameupdatepacket_t *packet)
{
    if (!g_server->m_world.connected)
        return false;
    auto &local = g_server->m_world.local;
    if (showxy == true)
    {
        int playerx = local.pos.m_x / 32;
        int playery = local.pos.m_y / 32;
        variantlist_t va{"OnNameChanged"};
        va[1] = local.name + " `4[" + std::to_string(playerx) + "," + std::to_string(playery) + "]" + " `4[" + std::to_string(local.netid) + "]``" + " `#[" + to_string(local.userid) + "]``";
        g_server->send(true, va, local.netid, -1);
    }
    g_server->m_world.local.pos = vector2_t{packet->m_vec_x, packet->m_vec_y};
    PRINTS("local pos: %.0f %.0f\n", packet->m_vec_x, packet->m_vec_y);

    return false;
}
bool events::in::variantlist(gameupdatepacket_t *packet)
{
    variantlist_t varlist{};
    auto extended = utils::get_extended(packet);
    extended += 4; // since it casts to data size not data but too lazy to fix this
    varlist.serialize_from_mem(extended);
    PRINTC("varlist: %s\n", varlist.print().c_str());
    auto func = varlist[0].get_string();

    if (func.find("OnSuperMainStartAcceptLogon") != -1)
        in_game = true;

    switch (hs::hash32(func.c_str()))
    {
    case fnv32("OnRequestWorldSelectMenu"):
    {
        auto &world = g_server->m_world;
        world.players.clear();
        world.local = {};
        world.connected = false;
        world.name = "EXIT";
    }
    break;
    case fnv32("OnSendToServer"):
        g_server->redirect_server(varlist);
        return true;

    case fnv32("OnConsoleMessage"):
    {
        if (worldbanjoinmod == true)
        {
            if (varlist[1].get_string().find("Removed your access from all locks.") != -1)
            {
                gt::send_overlay("`$Leaving the world due to having Mod bypass on and due to having a `#mod `$in the world!");
                g_server->send(false, "action|join_request\nname|exit", 3);
            }
        }
        varlist[1] = "`4[MegaMini#9735`4]```" + varlist[1].get_string();
        g_server->send(true, varlist);
        return true;
    }
    break;
    case fnv32("OnDialogRequest"):
    {
        auto content = varlist[1].get_string();

        if (content.find("add_label_with_icon|small|Remove Your Access From World|left|242|") != -1)
        {
            if (worldbanjoinmod == true)
            {
                g_server->send(false, "action|dialog_return\ndialog_name|unaccess");
                return true;
            }
        }

        if(content.find("add_label_with_icon|big|`wTelephone``|left|3898") != -1) {
            if (autobgl == true) {
                g_server->send(false, "add_text_input|dial|Phone #|12345|5");
                g_server->send(false, "action|dialog_return\ndialog_name|phonecall\ntilex||\ntiley||\num|12345|\ndial|");
            }
        }

        if (wrench == true)
        {
            if (content.find("add_button|report_player|`wReport Player``|noflags|0|0|") != -1)
            {
                if (content.find("embed_data|netID") != -1)
                {
                    return true;
                }
            }
        }
        if (taxsystem == true)
        {
            if (content.find("Drop") != -1)
            {
                if (content.find("embed_data|itemID|") != -1)
                {
                    std::string itemid = content.substr(content.find("embed_data|itemID|") + 18, content.length() - content.find("embed_data|itemID|") - 1);
                    if (itemid.find("242") != -1)
                    {
                        std::string count = content.substr(content.find("count||") + 7, content.length() - content.find("count||") - 1);
                        int yuzdeHesap = stoi(count) % 10;
                        int wlCount = stoi(count);
                        if (yuzdeHesap == 1)
                        {
                            wlCount = wlCount - 1;
                        }
                        if (yuzdeHesap == 2)
                        {
                            wlCount = wlCount - 2;
                        }
                        if (yuzdeHesap == 3)
                        {
                            wlCount = wlCount - 3;
                        }
                        if (yuzdeHesap == 4)
                        {
                            wlCount = wlCount - 4;
                        }
                        if (yuzdeHesap == 5)
                        {
                            wlCount = wlCount + 5;
                        }
                        if (yuzdeHesap == 6)
                        {
                            wlCount = wlCount + 4;
                        }
                        if (yuzdeHesap == 7)
                        {
                            wlCount = wlCount + 3;
                        }
                        if (yuzdeHesap == 8)
                        {
                            wlCount = wlCount + 2;
                        }
                        if (yuzdeHesap == 9)
                        {
                            wlCount = wlCount + 1;
                        }
                        int sonuc = wlCount * yuzde / 100;
                        int hoster = stoi(count) - sonuc;
                        varlist[1] = "set_default_color|`o\nadd_label_with_icon|big|`wDrop World Lock``|left|242|\nadd_textbox|How many to drop?|left|\nadd_text_input|count||" + to_string(hoster) + "|5|\nembed_data|itemID|242\nend_dialog|drop_item|Cancel|OK|";
                        g_server->send(true, varlist);
                        return true;
                    }
                }
            }
        }
        if (fastdrop == true)
        {
            std::string itemid = content.substr(content.find("embed_data|itemID|") + 18, content.length() - content.find("embed_data|itemID|") - 1);
            std::string count = content.substr(content.find("count||") + 7, content.length() - content.find("count||") - 1);
            if (content.find("embed_data|itemID|") != -1)
            {
                if (content.find("Drop") != -1)
                {
                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|" + itemid + "|\ncount|" + count);
                    return true;
                }
            }
        }
        if (taxsystem == true)
        {
            std::string itemid = content.substr(content.find("embed_data|itemID|") + 18, content.length() - content.find("embed_data|itemID|") - 1);
            std::string count = content.substr(content.find("count||") + 7, content.length() - content.find("count||") - 1);
            if (content.find("embed_data|itemID|") != -1)
            {
                if (content.find("Drop") != -1)
                {
                    if (itemid.find("242") != -1)
                    {
                        return true;
                    }
                }
            }
        }
        if (fasttrash == true)
        {
            std::string itemid = content.substr(content.find("embed_data|itemID|") + 18, content.length() - content.find("embed_data|itemID|") - 1);
            std::string count = content.substr(content.find("you have ") + 9, content.length() - content.find("you have ") - 1);
            std::string s = count;
            std::string delimiter = ")";
            std::string token = s.substr(0, s.find(delimiter));
            if (content.find("embed_data|itemID|") != -1)
            {
                if (content.find("Trash") != -1)
                {
                    g_server->send(false, "action|dialog_return\ndialog_name|trash_item\nitemID|" + itemid + "|\ncount|" + token);
                    return true;
                }
            }
        }
    }
    break;
    case fnv32("OnTalkBubble"):
    {
        if (fastwheel == true)
        {
            auto wry = varlist[2].get_string();
            if (wry.find("spun the wheel and got") != -1)
            {
                if (packet->m_int_data == 1800)
                {
                    varlist[2] = "`0[`2REAL```0]```` " + varlist[2].get_string();
                    std::cout << varlist[2].get_string() << std::endl;
                    g_server->send(true, varlist);
                    return true;
                }
            }
        }
    }
    break;
    case fnv32("OnRemove"):
    {
        auto text = varlist.get(1).get_string();
        if (text.find("netID|") == 0)
        {
            auto netid = atoi(text.substr(6).c_str());

            if (netid == g_server->m_world.local.netid)
                g_server->m_world.local = {};

            auto &players = g_server->m_world.players;
            for (size_t i = 0; i < players.size(); i++)
            {
                auto &player = players[i];
                if (player.netid == netid)
                {
                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                    break;
                }
            }
        }
    }
    break;
    case fnv32("OnSpawn"):
    {
        std::string meme = varlist.get(1).get_string();
        rtvar var = rtvar::parse(meme);
        auto name = var.find("name");
        auto netid = var.find("netID");
        auto onlineid = var.find("onlineID");
        if (name && netid && onlineid)
        {
            player ply{};
            ply.mod = false;
            ply.invis = false;
            ply.name = name->m_value;
            ply.country = var.get("country");
            name->m_values[0] += " `4[" + netid->m_value + "]``";
            auto pos = var.find("posXY");
            if (pos && pos->m_values.size() >= 2)
            {
                auto x = atoi(pos->m_values[0].c_str());
                auto y = atoi(pos->m_values[1].c_str());
                ply.pos = vector2_t{float(x), float(y)};
            }
            ply.userid = var.get_int("userID");
            ply.netid = var.get_int("netID");
            if (meme.find("type|local") != -1)
            {
                var.find("mstate")->m_values[0] = "1";
                g_server->m_world.local = ply;
            }
            g_server->m_world.players.push_back(ply);
            auto str = var.serialize();
            utils::replace(str, "onlineID", "onlineID|");
            varlist[1] = str;
            PRINTC("new: %s\n", varlist.print().c_str());
            g_server->send(true, varlist, -1, -1);
            return true;
        }
    }
    break;
    case fnv32("OnNameChanged"):
        std::thread(itsmod, packet->m_player_flags, varlist[1].get_string()).detach();
        return false;
    }
    return false;
}
bool events::in::generictext(std::string packet)
{
    PRINTC("Generic text: %s\n", packet.c_str());
    return false;
}
bool events::in::gamemessage(std::string packet)
{
    PRINTC("Game message: %s\n", packet.c_str());

    return false;
}
bool events::in::sendmapdata(gameupdatepacket_t *packet)
{
    g_server->m_world = {};
    auto extended = utils::get_extended(packet);
    extended += 4;
    auto data = extended + 6;
    auto name_length = *(short *)data;

    char *name = new char[name_length + 1];
    memcpy(name, data + sizeof(short), name_length);
    char none = '\0';
    memcpy(name + name_length, &none, 1);

    g_server->m_world.name = std::string(name);
    g_server->m_world.connected = true;
    delete[] name;
    return false;
}
bool events::in::state(gameupdatepacket_t *packet)
{
    if (!g_server->m_world.connected)
        return false;
    if (packet->m_player_flags == -1)
        return false;

    auto &players = g_server->m_world.players;
    for (auto &player : players)
    {
        if (player.netid == packet->m_player_flags)
        {
            player.pos = vector2_t{packet->m_vec_x, packet->m_vec_y};
            break;
        }
    }
    return false;
}
bool events::in::tracking(std::string packet)
{
    PRINTC("Tracking packet: %s\n", packet.c_str());
    if (packet.find("eventName|102_PLAYER.AUTHENTICATION") != -1)
    {
        std::string wlbalance = packet.substr(packet.find("Worldlock_balance|") + 18, packet.length() - packet.find("Worldlock_balance|") - 1);
        if (wlbalance.find("PLAYER.") != -1)
        {
            gt::send_log("`9World Lock Balance: `#" + wlbalance);
        }
    }
    return true;
}