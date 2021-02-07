#ifndef _SPROCPL
#define _SPROCPL

#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <fstream>

#include "sprvlgs.hh"

enum class PRedirectionInstruction
{
    /*
    Redirection instructions means 'set another spesific process with these prevs'
        Example Instructions:

            SETPL 3
            SETPV 1
            CALL
    */

    CALL,  // Call
    SETPV, // Set Privilage < Enumeration member >
    SETPL  // Set Process < PID >
};

std::unordered_map<std::string, PRedirectionInstruction> InstructionMap{
    {"CALL", PRedirectionInstruction::CALL},
    {"SETPV", PRedirectionInstruction::SETPV},
    {"SETPL", PRedirectionInstruction::SETPL}};

class ProcessRedirection : public VProcess
{
    std::vector<std::pair<PRedirectionInstruction, std::string>> instructions;
    std::shared_ptr<VProcess> source;
    std::shared_ptr<VProcess> destination;

    ProcessRedirection(const std::string &ascode, std::shared_ptr<VProcess> source) : source(std::move(source))
    {
        ParseInstructions(ascode);
    }

    void ParseInstructions(const std::string &as)
    {
        std::stringstream fetch;
        for (int idx = 0, ch = as[idx]; idx < as.length(); idx++)
        {
            if (isalnum(ch))
                fetch << ch;

            auto t = InstructionMap.find(fetch.str());
            if (t != InstructionMap.end())
            {
                fetch.clear();
                for (int sidx = idx; as[sidx] != '\n'; sidx++)
                    fetch << as[sidx];
                instructions.push_back({t->second, fetch.str()});
            }
        }
    }

    void Run()
    {
        // 1) Find VPID value in process list which one set in SETPL instruction

        std::shared_ptr<VProcess> dest(
            std::find_if(GlobalProcessDispatcher.VProcessList.begin(),
                         GlobalProcessDispatcher.VProcessList.end(),
                         [this](const std::shared_ptr<VProcess> &p) {
                             return p->GetVPID() == std::stoi(
                                                        std::find_if(this->instructions.begin(), this->instructions.end(),
                                                                     [](const std::pair<PRedirectionInstruction, std::string>
                                                                            &element) { return element.first == PRedirectionInstruction::SETPL; })
                                                            ->second);
                         })
                ->get());

        // 2) Set Privilages of this new process

        dest->SetPrivilage((ProcessPrivilages)std::stoi(std::find_if(this->instructions.begin(), this->instructions.end(),
                                                                     [](const std::pair<PRedirectionInstruction, std::string>
                                                                            &element) { return element.first == PRedirectionInstruction::SETPV; })
                                                            ->second));
        dest->Run();
    }
};

class GlobalSasmLoader
{
    /*
    Eg. Process Redirection Assembly
        .route 1
            SETPL 1
            SETPV 1
            CALL
        .route 2
            SETPL 2
            SETPV 1
            CALL
    */

    std::vector<std::pair<int, std::string>> GlobalRedirectionMap;

public:
    GlobalSasmLoader(std::ifstream file) { ReadAsm(file); }

    void ReadAsm(std::ifstream &fs)
    {
        bool block = false;
        int routeNumber = 0;

        std::stringstream routefetch;
        while (!fs.eof())
        {
            char ch = fs.get();
            routefetch << ch;

            if (routefetch.str().substr(0, 6).c_str() == ".route")
            {
                routeNumber = std::stoi(routefetch.str().substr(0, 6));
                routefetch.clear();

                while (routefetch.seekg(-1, std::ios::end).get() != '.' && !fs.eof())
                {
                    routefetch << fs.get();
                }
                GlobalRedirectionMap.push_back({routeNumber, routefetch.str()});
            }
        }
    }
};

#endif