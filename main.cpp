#include "pch.h"

#include "cfile.h"
#include "engine.h"
#include "environment.h"
#include "matrix.h"

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

struct Interpreter
{
    void handle_command(std::string_view sv)
    {
        if (sv.size() == 1 && isdigit(sv[0]) != 0)
        {
            m_env.stack.push(sv[0] - '0');
            m_env.auto_display();
        }
        else if ((isdigit(sv[0]) != 0 || (sv[0] == '-' && sv.size() > 1)) &&
                 std::all_of(sv.begin() + 1, sv.end(), [](char ch) { return isdigit(ch) != 0 || ch == '.'; }))
        {
            double i = 0;
            auto err = sscanf_s(sv.data(), "%lf", &i);
            if (err != 1) throw std::runtime_error("failed to sscanf.");
            m_env.stack.push(i);
            m_env.auto_display();
        }
        else if (sv.size() >= 1 && sv[0] == '$')
        {
            if (sv.size() == 1) throw std::runtime_error("expected variable name");
            if (sv[1] == '$')
            {
                if (sv.size() == 2) throw std::runtime_error("expected variable name");
                m_env.stack.push(sv.substr(2), Value::symbol_tag);
            }
            else
            {
                m_env.stack.push(m_env.varmap.at(sv.data() + 1).clone());
            }
            m_env.auto_display();
        }
        else if (sv.size() >= 1 && sv[0] == '@')
        {
            if (sv.size() == 1) throw std::runtime_error("expected stack index");
            if (!std::all_of(sv.begin() + 1, sv.end(), [](char ch) { return isdigit(ch) != 0; }))
                throw std::runtime_error("expected stack index");

            int i = 0;
            auto err = sscanf_s(sv.data() + 1, "%d", &i);
            if (err != 1) throw std::runtime_error("failed to sscanf.");

            m_env.stack.push(m_env.stack.at_from_top(i).clone());
            m_env.auto_display();
        }
        else if (sv[0] == '"')
        {
            if (sv.size() < 2 || sv.back() != '"') throw std::runtime_error("unterminated string literal");
            auto str = sv;
            str.remove_prefix(1);
            str.remove_suffix(1);
            m_env.stack.push(str, Value::string_tag);
        }
        else if (sv == "load-engine")
        {
            m_engine.load();
        }
        else if (sv == "unload-engine")
        {
            m_engine.unload();
        }
        else if (sv == "load-file")
        {
            fmt::printf("Filename>");

            std::string filename = read_line();

            auto p = fs::absolute(filename);
            auto in_file = CFile::open_rb(p);
            CFileView in = in_file;

            auto old_flag = m_env.auto_display_flag;
            m_env.auto_display_flag = false;

            try
            {
                char buf[128];
                while (in.scan_string(buf, sizeof(buf)) != -1)
                {
                    handle_command({buf, strlen(buf)});
                }
            }
            catch (...)
            {
                m_env.auto_display_flag = old_flag;
                throw;
            }

            m_env.auto_display_flag = old_flag;

            fmt::printf("Loaded file \"%s\".\n", p.u8string());
        }
        else
        {
            bool found = false;
            for (size_t i = 0; i < m_engine.commands.size; ++i)
            {
                auto&& command = m_engine.commands.begin[i];

                if (command.name == sv)
                {
                    (*command.function)(m_env);
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                throw std::runtime_error(fmt::sprintf("Input not recognized: %s. Use 'help' for command list.\n", sv));
            }
        }
    }

    void load_engine() { m_engine.load(); }

private:
    Environment m_env;
    Engine m_engine;
};

int main()
{
    Interpreter interpreter;

    try
    {
        interpreter.load_engine();
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

            interpreter.handle_command(std::string_view{buf, strlen(buf)});
        }
        catch (const std::exception& e)
        {
            fmt::printf("%s\n", e.what());
            std::fflush(stdin);
        }
    }
}
