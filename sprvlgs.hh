/**
 * @file sprvlgs.hh
 * 
 * @brief Main Privileges & Virtual Processes System Management Code
 * 
 */
#ifndef _SPRVLGS
#define _SPRVLGS
#include <iostream>
#include <variant>
#include <string>
#include <vector>
#include <memory>
#include <cstdlib>
#include "susqlem.hh"

// For Tests
#define URI "?q='abc'#div"
void stdout_test(const std::string &s) { std::cout << "Standart Output : " << s << std::endl; }
std::string stdin_test() { return std::string(URI); }
void stderr_test(const std::string &s) { std::cout << "ERR" << s << std::endl; }

enum class ProcessPrivilages
{
    OnlyReturn,           // Only return anything in this process. Do not change or write anything.
    OnlyChange,           // Change spesific value in this process. Do not write any data anywhere.
    ReturnOrChange,       // Change or return any value from this process.
    Any = ReturnOrChange, // Standart type : 'Any' kind does not allow you to write anywhere.
    All,                  // Super privilage , you can make everything.
    NoPrivileges          // Describes null value.
};

class StdVProcessConnection
{
public:
    // Virtual STDOUT In Sumatra : Describes General Output (Browser)
    // Virtual STDIN In Sumatra  : Describes URI Parameters
    // Virtual STDERR In Sumatra : Describes Console Log In Server

    void STDOUT(const std::string &s) { stdout_test(s); }
    void STDERR(const std::string &s) { stderr_test(s); }
    std::string STDIN() { return stdin_test(); }
};

/**
 * "Virtual Process" base class. Will be inherite from pages , dbase/any command executions etc.
 * 
 * Note : VProcess have StdVProcessConnection to virtualize i/o accession. R/W of any file
 * is not a standard i/o because they are processes.
 */
class VProcess
{
private:
    ProcessPrivilages privilage;

    // Virtual Process ID
    int VPID = -1;

public:
    StdVProcessConnection standardControls;

    ProcessPrivilages GetPrivilage() { return privilage; }
    void SetPrivilage(ProcessPrivilages p) { privilage = p; }
    int GetVPID() { return VPID; }
    void SetVPID(int vpid) { VPID = (VPID != -1) ? vpid : -1; }

    virtual void Run() {}
    virtual void Kill() {}
};

/**
 * Virtual Process Dispatcher is pointer lists of all processes. (Not queue)
 */
struct VProcessDispatcher
{
    std::vector<std::shared_ptr<VProcess>> VProcessList;

    VProcessDispatcher(std::shared_ptr<VProcess> vpc)
    {
        vpc->SetVPID(VProcessList.size() + 1);
        VProcessList.push_back(std::move(vpc));
    }
} GlobalProcessDispatcher;

/**
 * Allows call renderer to generate HTML or directly shows HTML to stdout. 
 * If you have values in ? or #, you will be able to send this process via stdin.
 *  
 */
class PageProcess : public VProcess
{
private:
    std::string directoryPathLoc;
    std::string renderedFileLoc;

public:
    PageProcess(const std::string &directory, const std::string &fileName) : directoryPathLoc(directory),
                                                                             renderedFileLoc(fileName) {}
    void Run()
    {
        std::ifstream f(renderedFileLoc);
        std::stringstream buffer;
        buffer << f.rdbuf();
        this->standardControls.STDOUT(buffer.str());
    }
};

/**
 *  ExternalProcess SQL :
 *      Run commands that want to read spesific data , without change any value anywhere in db.
 *      Run commands that want to write spesific location, without get another value in db.
 */
class SQLProcess : public VProcess
{
private:
    std::string cmd;
    SumatraSQLProcess::GeneralDatabase *db;
    SumatraSQLProcess::GeneralQuery *query;

public:
    SQLProcess(const std::string &command) : cmd(command) {}

    void Run() { RunSUSQLE(cmd.c_str()); }
    void Kill()
    {
        delete db;
        delete query;
    }

    // Create SQL VProcess with virtual privileges.
    bool RunSUSQLE(const char *str)
    {
        db = SumatraSQLProcess::GetMetadata("SUSQLE_md.dat");
        Populate(db);

        query = SumatraSQLProcess::SUSQLEParseQuery(str);
        query = SumatraSQLProcess::FillFuncOperations(query);
        query = SumatraSQLProcess::RevErrors(query, db);

        if (SumatraSQLProcess::error)
            cout << "<sumatra/sqlengine> [Error] : Executing Error." << endl;
        else
        {
            if (SumatraSQLProcess::FunctionOperationHandler(query->functions[0], db, query))
                return 0;
            SumatraSQLProcess::GeneralMiner *miner = SumatraSQLProcess::prepare_mining(query, db);
            SumatraSQLProcess::Mine(miner, db, query);

            SumatraSQLProcess::DistHandler(miner, query);
            SumatraSQLProcess::Select(miner, query);
        }
        return true;
    }
};
#endif