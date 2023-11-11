#include "global.h"
/**
 * @brief
 * SYNTAX: R <- JOIN relation_name1, relation_name2 ON column_name1 bin_op column_name2
 */
bool syntacticParseJOIN()
{

    logger.log("syntacticParseJOIN");
    if (tokenizedQuery.size() != 9 || tokenizedQuery[5] != "ON")
    {
        cout << "SYNTAC ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = JOIN;
    parsedQuery.joinResultRelationName = tokenizedQuery[0];
    parsedQuery.joinFirstRelationName = tokenizedQuery[3];
    parsedQuery.joinSecondRelationName = tokenizedQuery[4];
    parsedQuery.joinFirstColumnName = tokenizedQuery[6];
    parsedQuery.joinSecondColumnName = tokenizedQuery[8];

    string binaryOperator = tokenizedQuery[7];
    if (binaryOperator == "<")
        parsedQuery.joinBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.joinBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.joinBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.joinBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.joinBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.joinBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseJOIN()
{
    logger.log("semanticParseJOIN");

    if (tableCatalogue.isTable(parsedQuery.joinResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.joinFirstRelationName) || !tableCatalogue.isTable(parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.joinFirstColumnName, parsedQuery.joinFirstRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.joinSecondColumnName, parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

vector<int> joinRows(vector<int> row1, vector<int> row2)
{
    logger.log("joinRows");
    row1.insert(row1.end(), row2.begin(), row2.end());
    return row1;
}

void CreateJoinTable(Table *table1, Table *table2, string joinTableName, int joinBlockCount, int rowsInJoinTable, int maxRowInJoinedPage, vector<uint> rowsPerBlockCount)
{
    Table *joinTable = new Table();
    joinTable->tableName = joinTableName;
    joinTable->columns = table1->columns;
    joinTable->columns.insert(joinTable->columns.end(), table2->columns.begin(), table2->columns.end());
    joinTable->columnCount = joinTable->columns.size();

    joinTable->rowsPerBlockCount = rowsPerBlockCount;
    joinTable->blockCount = joinBlockCount;
    joinTable->rowCount = rowsInJoinTable;
    joinTable->maxRowsPerBlock = maxRowInJoinedPage;
    tableCatalogue.insertTable(joinTable);
}

void joinWithEqual(Table *table1, Table *table2, string joinTableName)
{

    logger.log("joinWithEqual");
    int colIdxTable1 = table1->getColumnIndex(parsedQuery.joinFirstColumnName);
    int colIdxTable2 = table2->getColumnIndex(parsedQuery.joinSecondColumnName);

    Cursor cursor1(table1->tableName, 0);
    vector<int> rowFrom1 = cursor1.getNext();
    Cursor cursor2(table2->tableName, 0);
    Cursor cursor2temp(table2->tableName, 0);
    vector<int> rowFrom2 = cursor2.getNext();

    // vector<int> tempRowFrom2 = cursor2.getNext();
    vector<vector<int>> joinedRows;
    int colCount = table1->columnCount + table2->columnCount;
    int maxRowInJoinedPage = 1000 / (colCount * 4);

    vector<uint> rowsPerBlockCount;
    int rowsInJoinTable = 0;
    int joinBlockCount = 0;
    int matchFeildFrom1;
    int matchFeildFrom2;
    int prevMatchedValue = -1;

    while (rowFrom1.size() > 0)
    {

        matchFeildFrom1 = rowFrom1[colIdxTable1];
        if (matchFeildFrom1 == prevMatchedValue)
        {
            // cout << cursor2.pageIndex << " " << cursor2.pagePointer << " " << cursor2temp.pageIndex << " " << cursor2temp.pagePointer << endl;
            if(cursor2.pageIndex!=cursor2temp.pageIndex){
              Cursor cursor3(table2->tableName, cursor2temp.pageIndex);
              cursor2=cursor3;
            }
            
            cursor2.tableName = cursor2temp.tableName;
            cursor2.pageIndex = cursor2temp.pageIndex;
            cursor2.pagePointer = cursor2temp.pagePointer;
            
            // cursor2=cursor2temp;
            // cout << cursor2.pageIndex << " " << cursor2.pagePointer << " " << cursor2temp.pageIndex << " " << cursor2temp.pagePointer << endl;

            rowFrom2.clear();
            rowFrom2 = cursor2.getNext();
            // cout << cursor2.pageIndex << " " << cursor2.pagePointer << " " << cursor2temp.pageIndex << " " << cursor2temp.pagePointer << " " << rowFrom2[colIdxTable2] << " " << matchFeildFrom1 << endl;
        }
        else
        {
            if (rowFrom2.size() == 0)
            {
                break;
            }

            cursor2temp.tableName = cursor2.tableName;
            if (cursor2.pagePointer > 0)
            {

                cursor2temp.pageIndex = cursor2.pageIndex;
                cursor2temp.pagePointer = cursor2.pagePointer - 1;
                // cout << cursor2.pageIndex << " " << cursor2.pagePointer << " " << cursor2temp.pageIndex << " " << cursor2temp.pagePointer << endl;
            }
            else if (cursor2.pagePointer == 0)
            {

                cursor2temp.pagePointer = table2->rowsPerBlockCount[cursor2.pageIndex - 1] - 1; //  change
                cursor2temp.pageIndex = cursor2.pageIndex - 1;
                // cout << cursor2.pageIndex << " " << cursor2.pagePointer << " " << cursor2temp.pageIndex << " " << cursor2temp.pagePointer << endl;
            }
        }

        matchFeildFrom2 = rowFrom2[colIdxTable2];

        if (matchFeildFrom1 < matchFeildFrom2)
        {
            rowFrom1 = cursor1.getNext();
        }
        else if (matchFeildFrom1 > matchFeildFrom2)
        {
            rowFrom2 = cursor2.getNext();
        }
        else
        {
           

            prevMatchedValue = matchFeildFrom2;
            while (rowFrom2.size() > 0 && matchFeildFrom2 == matchFeildFrom1)
            {

                vector<int> result = joinRows(rowFrom1, rowFrom2);
                joinedRows.push_back(result);
                if (joinedRows.size() == maxRowInJoinedPage)
                {
                    bufferManager.writePage(joinTableName, joinBlockCount, joinedRows, joinedRows.size());
                    rowsPerBlockCount.push_back(joinedRows.size());
                    rowsInJoinTable += joinedRows.size();
                    joinBlockCount++;
                    joinedRows.clear();
                }

                rowFrom2 = cursor2.getNext();
                if (rowFrom2.size() > 0)
                {
                    matchFeildFrom2 = rowFrom2[colIdxTable2];
                }
            }

            rowFrom1 = cursor1.getNext();
        }
    }

    if (joinedRows.size() > 0)
    {
        bufferManager.writePage(joinTableName, joinBlockCount, joinedRows, joinedRows.size());
        rowsPerBlockCount.push_back(joinedRows.size());
        rowsInJoinTable += joinedRows.size();
        joinBlockCount++;
        joinedRows.clear();
    }
    if (joinedRows.size() == 0 && joinBlockCount == 0)
    {
        joinedRows.push_back({-1});
        bufferManager.writePage(joinTableName, joinBlockCount, joinedRows, joinedRows.size());
        rowsPerBlockCount.push_back(joinedRows.size());
        rowsInJoinTable += joinedRows.size();
        joinBlockCount++;
        joinedRows.clear();
    }

    CreateJoinTable(table1, table2, joinTableName, joinBlockCount, rowsInJoinTable, maxRowInJoinedPage, rowsPerBlockCount);

}

void joinWithNotEqual(Table *table1, Table *table2, string joinTableName)
{
    logger.log("joinWithNotEqual");
    int colIdxTable1 = table1->getColumnIndex(parsedQuery.joinFirstColumnName);
    int colIdxTable2 = table2->getColumnIndex(parsedQuery.joinSecondColumnName);

    Cursor cursor1(table1->tableName, 0);
    vector<int> rowFrom1 = cursor1.getNext();
    vector<vector<int>> joinedRows;
    int colCount = table1->columnCount + table2->columnCount;
    int maxRowInJoinedPage = 1000 / (colCount * 4);
    int joinBlockCount = 0;

    vector<uint> rowsPerBlockCount;
    int rowsInJoinTable = 0;
    while (rowFrom1.size() > 0)
    {

        int matchFeild = rowFrom1[colIdxTable1];

        Cursor cursor2(table2->tableName, 0);
        vector<int> rowOfSecondFile;
        rowOfSecondFile = cursor2.getNext();
        int valOfsecondrow = rowOfSecondFile[colIdxTable2];
        while (rowOfSecondFile.size() > 0 && valOfsecondrow < matchFeild)
        {
            vector<int> result = joinRows(rowFrom1, rowOfSecondFile);
            joinedRows.push_back(result);
            if (joinedRows.size() == maxRowInJoinedPage)
            {
                bufferManager.writePage(joinTableName, joinBlockCount, joinedRows, joinedRows.size());
                rowsPerBlockCount.push_back(joinedRows.size());
                rowsInJoinTable += joinedRows.size();
                joinBlockCount++;
                joinedRows.clear();
            }
            rowOfSecondFile = cursor2.getNext();
            if (rowOfSecondFile.size() < 1)
            {
                break;
            }
            valOfsecondrow = rowOfSecondFile[colIdxTable2];
        }
        while (rowOfSecondFile.size() > 0 && valOfsecondrow == matchFeild)
        {
            rowOfSecondFile = cursor2.getNext();
            if (rowOfSecondFile.size() < 1)
            {
                break;
            }
            valOfsecondrow = rowOfSecondFile[colIdxTable2];
        }

        while (rowOfSecondFile.size() > 0 && valOfsecondrow > matchFeild)
        {
            vector<int> result = joinRows(rowFrom1, rowOfSecondFile);
            joinedRows.push_back(result);
            if (joinedRows.size() == maxRowInJoinedPage)
            {
                bufferManager.writePage(joinTableName, joinBlockCount, joinedRows, joinedRows.size());
                rowsPerBlockCount.push_back(joinedRows.size());
                rowsInJoinTable += joinedRows.size();
                joinBlockCount++;
                joinedRows.clear();
            }
            rowOfSecondFile = cursor2.getNext();
            if (rowOfSecondFile.size() < 1)
            {
                break;
            }
            valOfsecondrow = rowOfSecondFile[colIdxTable2];
        }

        rowFrom1 = cursor1.getNext();
    }
    if (joinedRows.size() > 0)
    {
        bufferManager.writePage(joinTableName, joinBlockCount, joinedRows, joinedRows.size());
        rowsPerBlockCount.push_back(joinedRows.size());
        rowsInJoinTable += joinedRows.size();
        joinBlockCount++;
        joinedRows.clear();
    }
    if (joinedRows.size() == 0 && joinBlockCount == 0)
    {
        joinedRows.push_back({-1});
        bufferManager.writePage(joinTableName, joinBlockCount, joinedRows, joinedRows.size());
        rowsPerBlockCount.push_back(joinedRows.size());
        rowsInJoinTable += joinedRows.size();
        joinBlockCount++;
        joinedRows.clear();
    }

    CreateJoinTable(table1, table2, joinTableName, joinBlockCount, rowsInJoinTable, maxRowInJoinedPage, rowsPerBlockCount);
}



void joinWithGreaterThanEqualTo(Table *table1, Table *table2, string joinTableName, bool equalToConsider)
{
    logger.log("joinWithGreaterThanEqualTo");

    int colIdxTable1 = table1->getColumnIndex(parsedQuery.joinFirstColumnName);
    int colIdxTable2 = table2->getColumnIndex(parsedQuery.joinSecondColumnName);

    Cursor cursor1(table1->tableName, 0);
    vector<int> rowFrom1 = cursor1.getNext();
    vector<vector<int>> joinedRows;
    int colCount = table1->columnCount + table2->columnCount;
    int maxRowInJoinedPage = 1000 / (colCount * 4);
    int joinBlockCount = 0;

    vector<uint> rowsPerBlockCount;
    int rowsInJoinTable = 0;
    while (rowFrom1.size() > 0)
    {

        int matchFeild = rowFrom1[colIdxTable1];

        Cursor cursor2(table2->tableName, 0);
        vector<int> rowOfSecondFile;

        int rowInBlock = table2->rowsPerBlockCount[0];
        int itr = 0;
        int valOfsecondrow;
        rowOfSecondFile = cursor2.getNext();
        if (equalToConsider)
        {
            while (rowOfSecondFile.size() > 0)
            {
                valOfsecondrow = rowOfSecondFile[colIdxTable2];

                if (valOfsecondrow <= matchFeild)
                {
                    vector<int> result = joinRows(rowFrom1, rowOfSecondFile);
                    joinedRows.push_back(result);
                    if (joinedRows.size() == maxRowInJoinedPage)
                    {
                        bufferManager.writePage(joinTableName, joinBlockCount, joinedRows, joinedRows.size());
                        rowsPerBlockCount.push_back(joinedRows.size());
                        rowsInJoinTable += joinedRows.size();
                        joinBlockCount++;
                        joinedRows.clear();
                    }
                    rowOfSecondFile = cursor2.getNext();
                }
                else
                {
                    break;
                }
            }
        }
        else
        {

            while (rowOfSecondFile.size() > 0)
            {
                valOfsecondrow = rowOfSecondFile[colIdxTable2];

                if (valOfsecondrow < matchFeild)
                {
                    vector<int> result = joinRows(rowFrom1, rowOfSecondFile);
                    joinedRows.push_back(result);
                    if (joinedRows.size() == maxRowInJoinedPage)
                    {
                        bufferManager.writePage(joinTableName, joinBlockCount, joinedRows, joinedRows.size());
                        rowsPerBlockCount.push_back(joinedRows.size());
                        rowsInJoinTable += joinedRows.size();
                        joinBlockCount++;
                        joinedRows.clear();
                    }
                    rowOfSecondFile = cursor2.getNext();
                }
                else
                {
                    break;
                }
            }
        }

        rowFrom1 = cursor1.getNext();
    }
    if (joinedRows.size() > 0)
    {
        bufferManager.writePage(joinTableName, joinBlockCount, joinedRows, joinedRows.size());
        rowsPerBlockCount.push_back(joinedRows.size());
        rowsInJoinTable += joinedRows.size();
        joinBlockCount++;
        joinedRows.clear();
    }
    if (joinedRows.size() == 0 && joinBlockCount == 0)
    {
        joinedRows.push_back({-1});
        bufferManager.writePage(joinTableName, joinBlockCount, joinedRows, joinedRows.size());
        rowsPerBlockCount.push_back(joinedRows.size());
        rowsInJoinTable += joinedRows.size();
        joinBlockCount++;
        joinedRows.clear();
    }

    CreateJoinTable(table1, table2, joinTableName, joinBlockCount, rowsInJoinTable, maxRowInJoinedPage, rowsPerBlockCount);
}



void joinWithLessThan(Table *table1, Table *table2, string joinTableName)
{
    logger.log("joinWithLessThan");

    int colIdxTable1 = table1->getColumnIndex(parsedQuery.joinFirstColumnName);
    int colIdxTable2 = table2->getColumnIndex(parsedQuery.joinSecondColumnName);

    Cursor cursor1(table1->tableName, 0);
    vector<int> rowFrom1 = cursor1.getNext();
    Cursor cursor2(table2->tableName, 0);
    Cursor cursor2temp(table2->tableName, 0);
    vector<int> rowFrom2 = cursor2.getNext();

    // vector<int> tempRowFrom2 = cursor2.getNext();
    vector<vector<int>> joinedRows;
    int colCount = table1->columnCount + table2->columnCount;
    int maxRowInJoinedPage = 1000 / (colCount * 4);

    vector<uint> rowsPerBlockCount;
    int rowsInJoinTable = 0;
    int joinBlockCount = 0;
    int matchFeildFrom1;
    int matchFeildFrom2;
    int prevMatchedValue = -1;
    while (rowFrom1.size() > 0 && rowFrom2.size() > 0)
    {
        matchFeildFrom1 = rowFrom1[colIdxTable1];

        matchFeildFrom2 = rowFrom2[colIdxTable2];
        if (matchFeildFrom1 >= matchFeildFrom2)
        {
            rowFrom2 = cursor2.getNext();
        }
        else
        {
            cursor2temp.tableName = cursor2.tableName;
            if (cursor2.pagePointer > 0)
            {
                cursor2temp.pageIndex = cursor2.pageIndex;
                cursor2temp.pagePointer = cursor2.pagePointer - 1;
            }
            else if (cursor2.pagePointer == 0)
            {
                cursor2temp.pagePointer = table2->rowsPerBlockCount[cursor2.pageIndex - 1] - 1;
                cursor2temp.pageIndex = cursor2.pageIndex - 1;
            }

            while (rowFrom2.size() > 0)
            {

                vector<int> result = joinRows(rowFrom1, rowFrom2);
                joinedRows.push_back(result);
                if (joinedRows.size() == maxRowInJoinedPage)
                {
                    bufferManager.writePage(joinTableName, joinBlockCount, joinedRows, joinedRows.size());
                    rowsPerBlockCount.push_back(joinedRows.size());
                    rowsInJoinTable += joinedRows.size();
                    joinBlockCount++;
                    joinedRows.clear();
                }

                rowFrom2 = cursor2.getNext();
            }
            if(cursor2.pageIndex!=cursor2temp.pageIndex){
              Cursor cursor3(table2->tableName, cursor2temp.pageIndex);
              cursor2=cursor3;
            }
            
            cursor2.tableName = cursor2temp.tableName;
            cursor2.pageIndex = cursor2temp.pageIndex;
            cursor2.pagePointer = cursor2temp.pagePointer;
            rowFrom2 = cursor2.getNext();

            rowFrom1 = cursor1.getNext();
        }
    }
    if (joinedRows.size() > 0)
    {
        bufferManager.writePage(joinTableName, joinBlockCount, joinedRows, joinedRows.size());
        rowsPerBlockCount.push_back(joinedRows.size());
        rowsInJoinTable += joinedRows.size();
        joinBlockCount++;
        joinedRows.clear();
    }
    if (joinedRows.size() == 0 && joinBlockCount == 0)
    {
        joinedRows.push_back({-1});
        bufferManager.writePage(joinTableName, joinBlockCount, joinedRows, joinedRows.size());
        rowsPerBlockCount.push_back(joinedRows.size());
        rowsInJoinTable += joinedRows.size();
        joinBlockCount++;
        joinedRows.clear();
    }
    CreateJoinTable(table1, table2, joinTableName, joinBlockCount, rowsInJoinTable, maxRowInJoinedPage, rowsPerBlockCount);

}

void joinWithLessThanEqualTo(Table *table1, Table *table2, string joinTableName)
{
    logger.log("joinWithLessThanEqualTo");

    int colIdxTable1 = table1->getColumnIndex(parsedQuery.joinFirstColumnName);
    int colIdxTable2 = table2->getColumnIndex(parsedQuery.joinSecondColumnName);

    Cursor cursor1(table1->tableName, 0);
    vector<int> rowFrom1 = cursor1.getNext();
    Cursor cursor2(table2->tableName, 0);
    Cursor cursor2temp(table2->tableName, 0);
    vector<int> rowFrom2 = cursor2.getNext();

    // vector<int> tempRowFrom2 = cursor2.getNext();
    vector<vector<int>> joinedRows;
    int colCount = table1->columnCount + table2->columnCount;
    int maxRowInJoinedPage = 1000 / (colCount * 4);

    vector<uint> rowsPerBlockCount;
    int rowsInJoinTable = 0;
    int joinBlockCount = 0;
    int matchFeildFrom1;
    int matchFeildFrom2;
    int prevMatchedValue = -1;
    while (rowFrom1.size() > 0 && rowFrom2.size() > 0)
    {
        matchFeildFrom1 = rowFrom1[colIdxTable1];

        matchFeildFrom2 = rowFrom2[colIdxTable2];
        if (matchFeildFrom2 < matchFeildFrom1)
        {
            rowFrom2 = cursor2.getNext();
        }
        else
        {
            cursor2temp.tableName = cursor2.tableName;
            if (cursor2.pagePointer > 0)
            {
                cursor2temp.pageIndex = cursor2.pageIndex;
                cursor2temp.pagePointer = cursor2.pagePointer - 1;
            }
            else if (cursor2.pagePointer == 0)
            {
                cursor2temp.pagePointer = table2->rowsPerBlockCount[cursor2.pageIndex - 1] - 1;
                cursor2temp.pageIndex = cursor2.pageIndex - 1;
            }

            while (rowFrom2.size() > 0)
            {

                vector<int> result = joinRows(rowFrom1, rowFrom2);
                joinedRows.push_back(result);
                if (joinedRows.size() == maxRowInJoinedPage)
                {
                    bufferManager.writePage(joinTableName, joinBlockCount, joinedRows, joinedRows.size());
                    rowsPerBlockCount.push_back(joinedRows.size());
                    rowsInJoinTable += joinedRows.size();
                    joinBlockCount++;
                    joinedRows.clear();
                }

                rowFrom2 = cursor2.getNext();
            }
            if(cursor2.pageIndex!=cursor2temp.pageIndex){
              Cursor cursor3(table2->tableName, cursor2temp.pageIndex);
              cursor2=cursor3;
            }
            
            cursor2.tableName = cursor2temp.tableName;
            cursor2.pageIndex = cursor2temp.pageIndex;
            cursor2.pagePointer = cursor2temp.pagePointer;
            rowFrom2 = cursor2.getNext();

            rowFrom1 = cursor1.getNext();
        }
    }
    if (joinedRows.size() > 0)
    {
        bufferManager.writePage(joinTableName, joinBlockCount, joinedRows, joinedRows.size());
        rowsPerBlockCount.push_back(joinedRows.size());
        rowsInJoinTable += joinedRows.size();
        joinBlockCount++;
        joinedRows.clear();
    }

    if (joinedRows.size() == 0 && joinBlockCount == 0)
    {
        joinedRows.push_back({-1});
        bufferManager.writePage(joinTableName, joinBlockCount, joinedRows, joinedRows.size());
        rowsPerBlockCount.push_back(joinedRows.size());
        rowsInJoinTable += joinedRows.size();
        joinBlockCount++;
        joinedRows.clear();
    }
    CreateJoinTable(table1, table2, joinTableName, joinBlockCount, rowsInJoinTable, maxRowInJoinedPage, rowsPerBlockCount);

}

void executeJOIN()
{

    logger.log("executeJOIN");
    parsedQuery.queryType = ORDER_BY;
    parsedQuery.sortRelationName = tokenizedQuery[3];
    parsedQuery.sortResultRelationName = tokenizedQuery[0] + "_A";
    parsedQuery.sortColumnNames.push_back(tokenizedQuery[6]);
    parsedQuery.sortingStrategies.push_back(ASC);

    executeORDERBY();
    parsedQuery.clear();

    parsedQuery.queryType = ORDER_BY;
    parsedQuery.sortRelationName = tokenizedQuery[4];
    parsedQuery.sortResultRelationName = tokenizedQuery[0] + "_B";

    parsedQuery.sortColumnNames.push_back(tokenizedQuery[8]);

    parsedQuery.sortingStrategies.push_back(ASC);
    executeORDERBY();

    parsedQuery.clear();

    logger.log("executeJOIN");
    bool temp = syntacticParseJOIN();

    parsedQuery.joinFirstRelationName = tokenizedQuery[0] + "_A";
    parsedQuery.joinSecondRelationName = tokenizedQuery[0] + "_B";

    Table *table1 = tableCatalogue.getTable(parsedQuery.joinFirstRelationName);
    Table *table2 = tableCatalogue.getTable(parsedQuery.joinSecondRelationName);
   
    string joinTableName = parsedQuery.joinResultRelationName;
    if (parsedQuery.joinBinaryOperator == EQUAL)
    {
        joinWithEqual(table1, table2, joinTableName);
    }
    else if (parsedQuery.joinBinaryOperator == NOT_EQUAL)
    {
        joinWithNotEqual(table1, table2, joinTableName);
    }

    else if (parsedQuery.joinBinaryOperator == LESS_THAN)
    {

        joinWithLessThan(table1, table2, joinTableName);
    }
    else if (parsedQuery.joinBinaryOperator == LEQ)

    {
        joinWithLessThanEqualTo(table1, table2, joinTableName);
    }
    else if (parsedQuery.joinBinaryOperator == GREATER_THAN)
    {
        bool isEqualTo = false;
        joinWithGreaterThanEqualTo(table1, table2, joinTableName, isEqualTo);
    }
    else if (parsedQuery.joinBinaryOperator == GEQ)
    {
        bool isEqualTo = true;
        joinWithGreaterThanEqualTo(table1, table2, joinTableName, isEqualTo);
    }

    tableCatalogue.deleteTable(parsedQuery.joinFirstRelationName);
    tableCatalogue.deleteTable(parsedQuery.joinSecondRelationName);


    return;
}