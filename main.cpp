#include "pch.h"

#include "engine.h"
#include "environment.h"
#include "matrix.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

struct Engine
{
    Commands commands = {0, nullptr};
    HMODULE dll = NULL;

    void load()
    {
        if (dll != NULL) throw std::runtime_error("Engine is already loaded.");
        dll = LoadLibraryW(L"sh-engine");
        if (dll == NULL) throw std::runtime_error("Failed to load engine DLL.");
        get_commands_t get_commands_proc = (get_commands_t)GetProcAddress(dll, "get_commands");
        if (!get_commands_proc) throw std::runtime_error("Failed to load commands from engine DLL.");
        commands = get_commands_proc();
    }
    void unload()
    {
        if (dll == NULL) throw std::runtime_error("Engine is not loaded.");
        commands = {0, nullptr};
        FreeLibrary(dll);
        dll = NULL;
    }
};

int main()
{
    Environment env;
    Engine engine;

    try
    {
        engine.load();
    }
    catch (std::exception& e)
    {
        fmt::printf("%s\n", e.what());
        return 1;
    }

    char buf[128];
    while (1)
    {
        try
        {
            fmt::printf(">");
            std::fflush(nullptr);
            if (scanf_s("%s", buf, sizeof(buf)) != 1) return 0;
            std::string_view sv(buf, strlen(buf));

            if (sv.size() == 1 && isdigit(sv[0]) != 0)
            {
                env.stack.push(sv[0] - '0');
                env.stack.display_top();
            }
            else if ((isdigit(sv[0]) != 0 || (sv[0] == '-' && sv.size() > 1)) &&
                     std::all_of(sv.begin() + 1, sv.end(), [](char ch) { return isdigit(ch) != 0 || ch == '.'; }))
            {
                double i = 0;
                auto err = sscanf_s(buf, "%lf", &i);
                if (err != 1) throw std::runtime_error("failed to sscanf.");
                env.stack.push(i);
                env.stack.display_top();
            }
            else if (sv.size() >= 1 && sv[0] == '$')
            {
                if (sv.size() == 1) throw std::runtime_error("expected variable name");
                if (sv[1] == '$')
                {
                    if (sv.size() == 2) throw std::runtime_error("expected variable name");
                    env.stack.push(sv.substr(2));
                }
                else
                {
                    env.stack.push(env.varmap.at(sv.data() + 1).clone());
                }
                env.stack.display_top();
            }
            else if (sv.size() >= 1 && sv[0] == '@')
            {
                if (sv.size() == 1) throw std::runtime_error("expected stack index");
                if (!std::all_of(sv.begin() + 1, sv.end(), [](char ch) { return isdigit(ch) != 0; }))
                    throw std::runtime_error("expected stack index");

                int i = 0;
                auto err = sscanf_s(buf + 1, "%d", &i);
                if (err != 1) throw std::runtime_error("failed to sscanf.");

                env.stack.push(env.stack.at_from_top(i).clone());
                env.stack.display_top();
            }
            else if (sv == "load-engine")
            {
                engine.load();
            }
            else if (sv == "unload-engine")
            {
                engine.unload();
            }
            else
            {
                bool found = false;
                for (size_t i = 0; i < engine.commands.size; ++i)
                {
                    auto&& command = engine.commands.begin[i];

                    if (command.name == sv)
                    {
                        (*command.function)(env);
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    fmt::printf("Input not recognized: %s. Use 'help' for command list.\n", buf);
                    std::fflush(stdin);
                }
            }
        }
        catch (const std::exception& e)
        {
            fmt::printf("%s\n", e.what());
            std::fflush(stdin);
        }
    }
}
