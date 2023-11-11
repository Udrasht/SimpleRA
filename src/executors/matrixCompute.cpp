#include "global.h"

/**
* @brief 
* SYNTAX: COMPUTE < matrix_name >
*/

bool syntacticParseMatrixCOMPUTE() {
    logger.log("syntacticParseMatrixCOMPUTE");
    if(tokenizedQuery.size() != 2) {
        cout << "SYNTAX ERROR\n";
        return false;
    }
    parsedQuery.queryType = COMPUTE;
    parsedQuery.computeMatrixName = tokenizedQuery[1];
    return true;
}

bool semanticParseMatrixCOMPUTE() {
    logger.log("semanticParseMatrixCOMPUTE");
    if (!matrixCatalogue.isMatrix(parsedQuery.computeMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exists\n";
        return false;
    }
    
    return true;
}

void executeMatrixCOMPUTE() {
    logger.log("executeMatrixCOMPUTE");
    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.computeMatrixName);
    blocksRead = 0;
    blocksWritten = 0;
    Matrix *resultMatrix = matrix->copy(matrix->matrixName+"_RESULT");
    resultMatrix->transpose();
    resultMatrix->compute(matrix);
    cout << "\nNumber of blocks read: " << blocksRead << "\n";
    cout << "Number of blocks written: " << blocksWritten << "\n";
    cout << "Number of blocks accessed: " << blocksRead + blocksWritten << "\n";
}
