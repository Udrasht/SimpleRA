#include"global.h"

bool semanticParse(){
    logger.log("semanticParse");
    switch(parsedQuery.queryType){
        case CLEAR: return semanticParseCLEAR();
        case CROSS: return semanticParseCROSS();
        case DISTINCT: return semanticParseDISTINCT();
        case EXPORT: return semanticParseEXPORT();
        case EXPORT_MATRIX: return semanticParseMatrixEXPORT();
        case INDEX: return semanticParseINDEX();
        case JOIN: return semanticParseJOIN();
        case LIST: return semanticParseLIST();
        case LOAD: return semanticParseLOAD();
        case LOAD_MATRIX: return semanticParseMatrixLOAD();
        case PRINT: return semanticParsePRINT();
        case PRINT_MATRIX: return semanticParseMatrixPRINT();
        case PROJECTION: return semanticParsePROJECTION();
        case RENAME: return semanticParseRENAME();
        case RENAME_MATRIX: return semanticParseMatrixRENAME();
        case SELECTION: return semanticParseSELECTION();
        case SORT: return semanticParseSORT();
        case SOURCE: return semanticParseSOURCE();
        case TRANSPOSE_MATRIX: return semanticParseMatrixTRANSPOSE();
        case CHECKSYMMETRY: return semanticParseMatrixCHECKSYMMETRY();
        case COMPUTE: return semanticParseMatrixCOMPUTE();
        case INPLACE_SORT: return semanticParseInplaceSORT();
        case ORDER_BY: return semanticParseORDERBY();
        case GROUP_BY: return semanticParseGROUPBY();

        default: cout<<"SEMANTIC ERROR"<<endl;
    }

    return false;
}