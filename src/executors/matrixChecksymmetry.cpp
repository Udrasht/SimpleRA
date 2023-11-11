#include "global.h"

/**
* @brief 
* SYNTAX: CHECKSYMMETRY < matrix_name >
*/

bool syntacticParseMatrixCHECKSYMMETRY() {
    logger.log("syntacticParseMatrixCHECKSYMMETRY");
    if(tokenizedQuery.size() != 2) {
        cout << "SYNTAX ERROR\n";
        return false;
    }
    parsedQuery.queryType = CHECKSYMMETRY;
    parsedQuery.symmetryMatrixName = tokenizedQuery[1];
    return true;
}

bool semanticParseMatrixCHECKSYMMETRY() {
    logger.log("semanticParseMatrixCHECKSYMMETRY");
    if (!matrixCatalogue.isMatrix(parsedQuery.symmetryMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exists\n";
        return false;
    }
    
    return true;
}

void executeMatrixCHECKSYMMETRY() {
    logger.log("executeMatrixCHECKSYMMETRY");
    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.symmetryMatrixName);
    blocksRead = 0;
    blocksWritten = 0;
    if(matrix->checkSymmetry()) {
        cout << "TRUE\n";
    } else {
        cout << "FALSE\n";
    }
    cout << "\nNumber of blocks read: " << blocksRead << "\n";
    cout << "Number of blocks written: " << blocksWritten << "\n";
    cout << "Number of blocks accessed: " << blocksRead + blocksWritten << "\n";
}
