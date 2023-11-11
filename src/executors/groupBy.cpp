#include "global.h"

pair<string, string> functionColumnSeperator(string combinedString)
{
    int len = combinedString.size();
    int idx = 0;
    string functionName = "";
    string columnName = "";
    for (; idx < len; idx++)
    {
        if (combinedString[idx] == '(')
        {
            break;
        }
        functionName = functionName + combinedString[idx];
    }
    idx++;
    for (; idx < len; idx++)
    {
        if (combinedString[idx] == ')')
        {
            break;
        }
        columnName = columnName + combinedString[idx];
    }

    return {functionName, columnName};
}

bool isInteger(const std::string &str)
{
    for (char c : str)
    {
        if (!std::isdigit(c))
        {
            return false;
        }
    }
    return true;
}
pair<bool, int> conditionValueCheck(string value)
{
    int len = value.size();
    string str = value;
    

    if (isInteger(str))
    {
        // Convert the modified string to an integer
        std::istringstream iss(str);
        int number;
        if (iss >> number)
        {
            return {true, number};
        }
        else
        {

            return {false, -1};
        }
    }
    else
    {

        return {false, -1};
    }
}

bool syntacticParseGROUPBY()
{
    logger.log("syntacticParseGROUPBY");
    int n = tokenizedQuery.size();
    if (n < 13  || tokenizedQuery[n - 2] != "RETURN" || tokenizedQuery[7] != "HAVING" || tokenizedQuery[5] != "FROM")
    {
        cout << "SYNTAX ERROR\n";
        return false;
    }

    parsedQuery.queryType = GROUP_BY;
    parsedQuery.groupByResultRelationName = tokenizedQuery[0];
    parsedQuery.groubByColumnName = tokenizedQuery[4];
    parsedQuery.groubByRelationName = tokenizedQuery[6];

    string aggrigateFunctionWithColumn = tokenizedQuery[8];
    string returnFunctionWithColumn = tokenizedQuery[n-1];

    if (aggrigateFunctionWithColumn.size() < 6)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    pair<string, string> functioWithColumn = functionColumnSeperator(aggrigateFunctionWithColumn);
    parsedQuery.groupByaggrigateFunctionName = functioWithColumn.first;
    parsedQuery.groubByaggrigateColumnName = functioWithColumn.second;
    string conditionStr=tokenizedQuery[10];
    if(n==14){

            conditionStr=conditionStr+tokenizedQuery[11];
    }
    else if(n==15){

            conditionStr=conditionStr+tokenizedQuery[11]+tokenizedQuery[12];
    }
    else if(n==16){

            conditionStr=conditionStr+tokenizedQuery[11]+tokenizedQuery[12]+tokenizedQuery[13];
    }
    else if(n==17){

            conditionStr=conditionStr+tokenizedQuery[11]+tokenizedQuery[12]+tokenizedQuery[13]+tokenizedQuery[14];
    }
    // cout<<conditionStr<<"hi"<<endl;
    pair<bool, int> conditionValue = conditionValueCheck(conditionStr);
    if (!conditionValue.first)
    {
        cout << "Conditioned value is wrong!" << endl;
        return false;
    }
    parsedQuery.aggrigateFunctionConditionValue = conditionValue.second;

    if (parsedQuery.groupByaggrigateFunctionName != "MAX" && parsedQuery.groupByaggrigateFunctionName != "MIN" && parsedQuery.groupByaggrigateFunctionName != "SUM" && parsedQuery.groupByaggrigateFunctionName != "AVG" && parsedQuery.groupByaggrigateFunctionName != "COUNT")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    if (returnFunctionWithColumn.size() < 6)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    pair<string, string> functioWithColumn1 = functionColumnSeperator(returnFunctionWithColumn);
    parsedQuery.groupByReturnFunctionName = functioWithColumn1.first;
    parsedQuery.groubByReturnColumnName = functioWithColumn1.second;
    if (parsedQuery.groupByReturnFunctionName != "MAX" && parsedQuery.groupByReturnFunctionName != "MIN" && parsedQuery.groupByReturnFunctionName != "SUM" && parsedQuery.groupByReturnFunctionName != "AVG" && parsedQuery.groupByReturnFunctionName != "COUNT")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    string binaryOperator = tokenizedQuery[9];
    if (binaryOperator == "<")
        parsedQuery.groupByBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.groupByBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.groupByBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.groupByBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.groupByBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.groupByBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    return true;
}

bool semanticParseGROUPBY()
{
    logger.log("semanticParseGROUBBY");

    if (!tableCatalogue.isTable(parsedQuery.groubByRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist\n";
        return false;
    }

    if (tableCatalogue.isTable(parsedQuery.groupByResultRelationName))
    {
        cout << "SEMANTIC ERROR: Result Relation already exists\n";
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.groubByaggrigateColumnName, parsedQuery.groubByRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.groubByColumnName, parsedQuery.groubByRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.groubByReturnColumnName, parsedQuery.groubByRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }

    return true;
}

bool conditionCheck(int gropingAggregateColumnValue, int count, bool isCountUse)
{
    int value;
    if (isCountUse)
    {
        value = gropingAggregateColumnValue / count;
    }
    else
    {
        value = gropingAggregateColumnValue;
    }
    // cout << value << endl;
    int conditionCheckValue = parsedQuery.aggrigateFunctionConditionValue;
    if (parsedQuery.groupByBinaryOperator == LESS_THAN)
    {
        if (value < conditionCheckValue)
        {
            return true;
        }
    }
    else if (parsedQuery.groupByBinaryOperator == GREATER_THAN)
    {
        if (value > conditionCheckValue)
        {
            return true;
        }
    }
    else if (parsedQuery.groupByBinaryOperator == GEQ)
    {
        if (value >= conditionCheckValue)
        {
            return true;
        }
    }
    else if (parsedQuery.groupByBinaryOperator == LEQ)
    {
        if (value <= conditionCheckValue)
        {
            return true;
        }
    }
    else if (parsedQuery.groupByBinaryOperator == EQUAL)
    {
        if (value == conditionCheckValue)
        {
            return true;
        }
    }
    else if (parsedQuery.groupByBinaryOperator == NOT_EQUAL)
    {
        if (value != conditionCheckValue)
        {
            return true;
        }
    }
    return false;
}

void CreateJoinTable(string resultTableName, vector<uint> rowsPerBlockCount, int rowinTable, int pageCount)
{
    Table *resultTable = new Table();
    resultTable->tableName = resultTableName;
    resultTable->columns = {parsedQuery.groubByColumnName, parsedQuery.groupByReturnFunctionName + parsedQuery.groubByReturnColumnName};

    resultTable->columnCount = 2;

    resultTable->rowsPerBlockCount = rowsPerBlockCount;
    resultTable->blockCount = pageCount;
    resultTable->rowCount = rowinTable;
    resultTable->maxRowsPerBlock = 1000 / (2 * 4);
    tableCatalogue.insertTable(resultTable);
}

void performGROUPBY()
{
    logger.log("performGROUPBY");
    Table *table = tableCatalogue.getTable(parsedQuery.groubByRelationName);

    string resultTableName = parsedQuery.groupByResultRelationName;

    Cursor cursor(table->tableName, 0);

    vector<int> pageRow = cursor.getNext();

    int groubByColumnNameidx = table->getColumnIndex(parsedQuery.groubByColumnName);
    int groubByaggrigateColumnidx = table->getColumnIndex(parsedQuery.groubByaggrigateColumnName);

    int returnColumnidx = table->getColumnIndex(parsedQuery.groubByReturnColumnName);
    int gropingColumnValue;
    int gropingAggregateColumnValue = 0;
    int returnValueCalculate = 0;
    int maxRowInGroupTable = 1000 / (2 * 4);

    if (pageRow.size() > 0)
    {
        gropingColumnValue = pageRow[groubByColumnNameidx];
        gropingAggregateColumnValue = pageRow[groubByaggrigateColumnidx];
        returnValueCalculate = pageRow[returnColumnidx];
    }
    if (parsedQuery.groupByReturnFunctionName == "COUNT")
    {
        returnValueCalculate = 1;
    }
    if (parsedQuery.groupByaggrigateFunctionName == "COUNT")
    {
        gropingAggregateColumnValue = 1;
    }

    int count = 1;
    int countForReturn = 1;
    bool isCountUse = false;
    bool iscountForReturnuse = false;

    vector<vector<int>> pageVectors;
    int pageCount = 0;

    vector<uint> rowsPerBlockCount;
    int rowsInTable = 0;

    while (pageRow.size() > 0)
    {

        pageRow = cursor.getNext();

        while (pageRow.size() > 0 && pageRow[groubByColumnNameidx] == gropingColumnValue)
        {

            // aggrigatefunction operation performed
            if (parsedQuery.groupByaggrigateFunctionName == "MAX")
            {

                gropingAggregateColumnValue = max(gropingAggregateColumnValue, pageRow[groubByaggrigateColumnidx]);
            }
            else if (parsedQuery.groupByaggrigateFunctionName == "MIN")
            {
                gropingAggregateColumnValue = min(gropingAggregateColumnValue, pageRow[groubByaggrigateColumnidx]);
            }
            else if (parsedQuery.groupByaggrigateFunctionName == "SUM")
            {
                gropingAggregateColumnValue += pageRow[groubByaggrigateColumnidx];
            }
            else if (parsedQuery.groupByaggrigateFunctionName == "AVG")
            {
                gropingAggregateColumnValue += pageRow[groubByaggrigateColumnidx];
                count++;
                isCountUse = true;
            }
            else if (parsedQuery.groupByaggrigateFunctionName == "COUNT")
            {
                gropingAggregateColumnValue++;
            }
            // returnfunction operation performed
            if (parsedQuery.groupByReturnFunctionName == "MAX")
            {

                returnValueCalculate = max(returnValueCalculate, pageRow[returnColumnidx]);
            }
            else if (parsedQuery.groupByReturnFunctionName == "MIN")
            {
                returnValueCalculate = min(returnValueCalculate, pageRow[returnColumnidx]);
            }
            else if (parsedQuery.groupByReturnFunctionName == "SUM")
            {
                returnValueCalculate += pageRow[returnColumnidx];
            }
            else if (parsedQuery.groupByReturnFunctionName == "AVG")
            {
                returnValueCalculate += pageRow[returnColumnidx];
                countForReturn++;
                iscountForReturnuse = true;
            }
            else if (parsedQuery.groupByReturnFunctionName == "COUNT")
            {
                
                returnValueCalculate++;
                
            }
            pageRow = cursor.getNext();
        }
        bool conditionSatisfied = conditionCheck(gropingAggregateColumnValue, count, isCountUse);
        if (conditionSatisfied)
        {
            if (iscountForReturnuse)
            {
                returnValueCalculate = returnValueCalculate / countForReturn;
            }
           
            pageVectors.push_back({gropingColumnValue, returnValueCalculate});
            if (pageVectors.size() == maxRowInGroupTable)
            {
                bufferManager.writePage(resultTableName, pageCount, pageVectors, pageVectors.size());
                rowsPerBlockCount.push_back(pageVectors.size());
                rowsInTable += pageVectors.size();
                pageCount++;
                pageVectors.clear();
            }
        }
        if (pageRow.size() > 0)
        {
            gropingColumnValue = pageRow[groubByColumnNameidx];
            gropingAggregateColumnValue = pageRow[groubByaggrigateColumnidx];
            returnValueCalculate = pageRow[returnColumnidx];

            if (parsedQuery.groupByReturnFunctionName == "COUNT")
            {
                returnValueCalculate = 1;
            }
            if (parsedQuery.groupByaggrigateFunctionName == "COUNT")
            {
                gropingAggregateColumnValue = 1;
            }
            count = 1;
            countForReturn = 1;
        }
    }
    if (pageVectors.size() > 0)
    {
        bufferManager.writePage(resultTableName, pageCount, pageVectors, pageVectors.size());
        rowsPerBlockCount.push_back(pageVectors.size());
        rowsInTable += pageVectors.size();
        pageCount++;
        pageVectors.clear();
    }
    if (pageVectors.size() == 0 && pageCount == 0)
    {
        pageVectors.push_back({-1});
        bufferManager.writePage(resultTableName, pageCount, pageVectors, pageVectors.size());
        rowsPerBlockCount.push_back(pageVectors.size());
        rowsInTable += pageVectors.size();
        pageCount++;
        pageVectors.clear();
    }

    CreateJoinTable(resultTableName, rowsPerBlockCount, rowsInTable, pageCount);
}
void executeGROUPBY()
{
    logger.log("executeGROUPBY");
    parsedQuery.queryType = ORDER_BY;
    parsedQuery.sortRelationName = tokenizedQuery[6];
    parsedQuery.sortResultRelationName = tokenizedQuery[0] + "_A";
    parsedQuery.sortColumnNames.push_back(tokenizedQuery[4]);
    parsedQuery.sortingStrategies.push_back(ASC);
    executeORDERBY();
    parsedQuery.clear();
  
    logger.log("executeGROUPBY");
    bool temp = syntacticParseGROUPBY();
    parsedQuery.groubByRelationName = tokenizedQuery[0] + "_A";

    performGROUPBY();
   
    tableCatalogue.deleteTable(tokenizedQuery[0] + "_A");

    return;
}