#include "global.h"
/**
 * @brief 
 * SYNTAX: RENAME column_name TO column_name FROM relation_name
 */
bool syntacticParseRENAME()
{
    logger.log("syntacticParseRENAME");
    if (tokenizedQuery.size() != 6 || tokenizedQuery[2] != "TO" || tokenizedQuery[4] != "FROM")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = RENAME;
    parsedQuery.renameFromColumnName = tokenizedQuery[1];
    parsedQuery.renameToColumnName = tokenizedQuery[3];
    parsedQuery.renameRelationName = tokenizedQuery[5];
    return true;
}



bool syntacticParseMatrixRENAME(){
    logger.log("syntacticParseMatrixRENAME");
    if (tokenizedQuery.size() != 4)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = RENAME_MATRIX;
    parsedQuery.renameRelationName = tokenizedQuery[2];
    parsedQuery.renameNewRelationName= tokenizedQuery[3];
 
    return true;
}



bool semanticParseRENAME()
{
    logger.log("semanticParseRENAME");

    if (!tableCatalogue.isTable(parsedQuery.renameRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.renameFromColumnName, parsedQuery.renameRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }

    if (tableCatalogue.isColumnFromTable(parsedQuery.renameToColumnName, parsedQuery.renameRelationName))
    {
        cout << "SEMANTIC ERROR: Column with name already exists" << endl;
        return false;
    }
    return true;
}

bool semanticParseMatrixRENAME()
{
    logger.log("semanticParseMatrixRENAME");

    if (!matrixCatalogue.isMatrix(parsedQuery.renameRelationName))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }

    if(matrixCatalogue.isMatrix( parsedQuery.renameNewRelationName)){
        cout << "SEMANTIC ERROR: Matrix with newName  already exist" << endl;
        return false;
    }

    
    return true;
}



void executeRENAME()
{
    logger.log("executeRENAME");
    Table* table = tableCatalogue.getTable(parsedQuery.renameRelationName);
    table->renameColumn(parsedQuery.renameFromColumnName, parsedQuery.renameToColumnName);
    return;
}

void executeMatrixRENAME()
{
    blocksRead=0;
    blocksWritten=0;
    logger.log("executeMatrixRENAME");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.renameRelationName);
    matrix->renameMatrix(parsedQuery.renameRelationName, parsedQuery.renameNewRelationName);

    matrixCatalogue.updateMatrixName(matrix,parsedQuery.renameRelationName,parsedQuery.renameNewRelationName);
    cout << "Number of blocks read: " << blocksRead << "\n";
    cout << "Number of blocks written: " << blocksWritten << "\n";
    cout << "Number of blocks accessed: " << blocksRead + blocksWritten << "\n";

    return;
}