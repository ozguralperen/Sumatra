/**
 * @file susqled.hh
 * 
 * @brief Sumatra VProcess Server System compatible SQL Engine Usage Declarations File
 * 
 */

#ifndef _SUSQLED
#define _SUSQLED

#include <bits/stdc++.h>
using namespace std;

namespace SumatraSQLProcess
{
    typedef map<string, map<string, vector<int>>> Database;
    typedef map<string, map<int, string>> DatabaseReference;

    bool error;

    struct GeneralDatabase
    {
        Database data;
        DatabaseReference meta;
    };

    struct GeneralQuery
    {
        vector<string> columns;
        vector<string> tables;
        vector<string> conditions;
        vector<string> operators;
        vector<string> functions;
    };

    struct GeneralMiner
    {
        int n; 
        Database data;
        map<string, int> m;  
        vector<string> table; 
        vector<int> size;
        vector<vector<string>> column;
        vector<int> pointer;
        vector<int> maybe;
    };

    vector<vector<int>> join_table;
    set<string> to_remove;

} // namespace SumatraSQLProcess
#endif