#include "global.h"

/**
 * @brief syntactic parsing for ORDER BY query
 * 
 * syntax:
 * <new_table> <- ORDER BY <attribute> ASC|DESC ON <table_name>
*/
bool syntacticParseORDERBY() {
    logger.log("syntacticParseORDERBY");
    int n = tokenizedQuery.size();
    if(n < 8 || tokenizedQuery[n-2] != "ON") {
        cout << "SYNTAX ERROR\n";
        return false;
    }
    parsedQuery.queryType = ORDER_BY;
    parsedQuery.sortRelationName = tokenizedQuery[n-1];
    parsedQuery.sortResultRelationName = tokenizedQuery[0];
    
    for(int i=4; i<n-2; i+=2) {
        parsedQuery.sortColumnNames.push_back(tokenizedQuery[i]);
        if(tokenizedQuery[i+1] == "ASC") {
            parsedQuery.sortingStrategies.push_back(ASC);
        } else if(tokenizedQuery[i+1] == "DESC") {
            parsedQuery.sortingStrategies.push_back(DESC);
        } else {
            cout << "SYNTAX ERROR\n";
            return false;
        }
    }

    return true;
}

bool semanticParseORDERBY() {
    logger.log("semanticParseORDERBY");

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)) {
        cout << "SEMANTIC ERROR: Relation doesn't exist\n";
        return false;
    }

    if(tableCatalogue.isTable(parsedQuery.sortResultRelationName)) {
        cout << "SEMANTIC ERROR: Result Relation already exists\n";
        return false;
    }

    for(int i=0; i<parsedQuery.sortColumnNames.size(); i++) {
        if(!tableCatalogue.isColumnFromTable(parsedQuery.sortColumnNames[i], parsedQuery.sortRelationName)) {
            cout << "SEMANTIC ERROR: Column doesn't exist in relation\n";
            return false;
        }
    }

    return true;
}

void executeORDERBY() {
    logger.log("executeORDERBY");

    sortTable(true, parsedQuery.sortResultRelationName);
}