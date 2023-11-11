#include "global.h"

/**
* @brief 
* SYNTAX: TRANSPOSE MATRIX < matrix_name >
*/

bool syntacticParseMatrixTRANSPOSE() {
    logger.log("syntacticParseMatrixTRANSPOSE");
    if(tokenizedQuery.size() != 3) {
        cout << "SYNTAX ERROR\n";
        return false;
    }
    parsedQuery.queryType = TRANSPOSE_MATRIX;
    parsedQuery.transposeMatrixName = tokenizedQuery[2];
    return true;
}

bool semanticParseMatrixTRANSPOSE() {
    logger.log("semanticParseMatrixTRANSPOSE");
    if (!matrixCatalogue.isMatrix(parsedQuery.transposeMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exists\n";
        return false;
    }
    
    return true;
}

void executeMatrixTRANSPOSE() {
    logger.log("executeMatrixTRANSPOSE");
    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.transposeMatrixName);
    blocksRead = 0;
    blocksWritten = 0;
    matrix->transpose();
    cout << "Number of blocks read: " << blocksRead << "\n";
    cout << "Number of blocks written: " << blocksWritten << "\n";
    cout << "Number of blocks accessed: " << blocksRead + blocksWritten << "\n";
}
