/**
 * @file susqlep.hh
 * 
 * @brief Sumatra VProcess Server System compatible SQL Engine Query Parser Header
 * 
 */

#ifndef _SUSQLEP
#define _SUSQLEP

#include "susqled.hh"

namespace SumatraSQLProcess
{
    struct GeneralQuery *SUSQLEParseQuery(string input)
    {
        for (int i = 0; i < input.size(); i++)
            if (input[i] == ',')
                input[i] = ' ';

        stringstream ss;
        ss << input;

        string token;
        string temp;

        GeneralQuery *query = new GeneralQuery;

        int flag1 = -1, flag2 = 0;
        while (ss >> token)
        {
            if (flag1 == -1 && (token.compare("select") == 0 || token.compare("SELECT") == 0))
            {
                flag1 = 0;
                continue;
            }
            else if (flag1 == 0 && (token.compare("from") == 0 || token.compare("FROM") == 0))
            {
                flag1 = 1;
                continue;
            }
            else if (flag1 == 1 && (token.compare("where") == 0 || token.compare("WHERE") == 0))
            {
                flag1 = 2;
                continue;
            }
            if (flag1 == 0)
                query->columns.push_back(token);
            else if (flag1 == 1)
                query->tables.push_back(token);
            else if (flag1 == 2)
            {
                if (token.compare("and") == 0 || token.compare("or") == 0 || token.compare("OR") == 0 || token.compare("AND") == 0)
                {
                    query->operators.push_back(token);
                    if (temp.compare("") != 0)
                    {
                        string leftop = "", rightop = "", op = "";
                        for (int i = 0; i < temp.size(); i++)
                        {
                            if (temp[i] == '<' || temp[i] == '>' || temp[i] == '=' || temp[i] == '!')
                                flag2 = 1;
                            else if (flag2 == 1 and (temp[i] != '<' and temp[i] != '>' and temp[i] != '!'))
                                flag2 = 2;
                            if (flag2 == 0 && temp[i] != ' ')
                                leftop += temp[i];
                            if (flag2 == 1 && temp[i] != ' ')
                                op += temp[i];
                            if (flag2 == 2 && temp[i] != ' ')
                                rightop += temp[i];
                        }
                        string q = leftop + " " + op + " " + rightop;

                        query->conditions.push_back(q);
                    }
                    temp = "";
                    continue;
                }
                temp += token;
                temp += " ";
            }
        }
        flag2 = 0;
        if (temp.compare("") != 0)
        {
            string leftop = "", rightop = "", op = "";

            for (int i = 0; i < temp.size(); i++)
            {
                if (temp[i] == '<' or temp[i] == '>' or temp[i] == '=' or temp[i] == '!')
                    flag2 = 1;
                if (flag2 == 1 and (temp[i] != '<' and temp[i] != '>' and temp[i] != '=' and temp[i] != '!'))
                    flag2 = 2;
                if (flag2 == 0 && temp[i] != ' ')
                    leftop += temp[i];
                if (flag2 == 1 && temp[i] != ' ')
                    op += temp[i];
                if (flag2 == 2 && temp[i] != ' ')
                    rightop += temp[i];
            }
            string q = leftop + " " + op + " " + rightop;
            query->conditions.push_back(q);
        }
        if (query->columns.size() == 0 || query->tables.size() == 0)
            error = true;
        return query;
    }

} // namespace SumatraSQLProcess
#endif