#include "tableCatalogue.h"
// #include "matrixCatalogue.h"

using namespace std;

enum QueryType
{
    CLEAR,
    CROSS,
    DISTINCT,
    EXPORT,
    INDEX,
    JOIN,
    LIST,
    LOAD,
    PRINT,
    PROJECTION,
    RENAME,
    SELECTION,
    SORT,
    SOURCE,
    UNDETERMINED,
    LOAD_MATRIX,
    PRINT_MATRIX,
    EXPORT_MATRIX,
    RENAME_MATRIX,
    TRANSPOSE_MATRIX,
    CHECKSYMMETRY,
    COMPUTE,
    INPLACE_SORT,
    ORDER_BY,
    GROUP_BY
};

enum BinaryOperator
{
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};

enum SortingStrategy
{
    ASC,
    DESC,
    NO_SORT_CLAUSE
};

enum SelectType
{
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

class ParsedQuery
{

public:
    QueryType queryType = UNDETERMINED;

    string clearRelationName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";

    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationName = "";

    IndexingStrategy indexingStrategy = NOTHING;
    string indexColumnName = "";
    string indexRelationName = "";

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";

    string loadRelationName = "";

    string printRelationName = "";

    string projectionResultRelationName = "";
    vector<string> projectionColumnList;
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";
    string renameNewRelationName="";

    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    SortingStrategy sortingStrategy = NO_SORT_CLAUSE;
    string sortResultRelationName = "";
    string sortColumnName = "";
    string sortRelationName = "";
    vector<string> sortColumnNames;
    vector<int> sortColumnIndices;
    vector<SortingStrategy> sortingStrategies;

    string sourceFileName = "";

    string transposeMatrixName = "";
    string symmetryMatrixName = "";
    string computeMatrixName = "";
    

     string groupByResultRelationName = "";
     string groubByRelationName = "";
     string groubByColumnName = "";
     string groubByaggrigateColumnName = "";
     
     string groupByaggrigateFunctionName="";
     string groupByReturnFunctionName="";
     string groubByReturnColumnName="";
     int aggrigateFunctionConditionValue=-1;
     BinaryOperator groupByBinaryOperator = NO_BINOP_CLAUSE;



    ParsedQuery();
    void clear();
};

bool syntacticParse();
bool syntacticParseCLEAR();
bool syntacticParseCROSS();
bool syntacticParseDISTINCT();
bool syntacticParseEXPORT();
bool syntacticParseINDEX();
bool syntacticParseJOIN();
bool syntacticParseLIST();
bool syntacticParseLOAD();
bool syntacticParsePRINT();
bool syntacticParsePROJECTION();
bool syntacticParseRENAME();
bool syntacticParseMatrixRENAME();
bool syntacticParseSELECTION();
bool syntacticParseSORT();
bool syntacticParseSOURCE();
bool syntacticParseMatrixLOAD();
bool syntacticParseMatrixPRINT();
bool syntacticParseMatrixEXPORT();
bool syntacticParseMatrixTRANSPOSE();
bool syntacticParseMatrixCHECKSYMMETRY();
bool syntacticParseMatrixCOMPUTE();
bool syntacticParseInplaceSORT();
bool syntacticParseORDERBY();
bool syntacticParseGROUPBY();


bool isFileExists(string tableName);
bool isQueryFile(string fileName);
