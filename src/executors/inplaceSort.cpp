#include "global.h"

/**
 * @brief syntactic parsing for SORT query
 * 
 * syntax:
 * SORT <table_name> BY <column_name1, column_name2,..., column_namek> IN <ASC|DESC, ASC|DESC,..., ASC|DESC>
*/
bool syntacticParseInplaceSORT() {
    logger.log("syntacticParseInplaceSORT");
    int n = tokenizedQuery.size();
    if(n < 6 || tokenizedQuery[2] != "BY") {
        cout << "SYNTAX ERROR\n";
        return false;
    }
    parsedQuery.queryType = INPLACE_SORT;
    parsedQuery.sortRelationName = tokenizedQuery[1];
    int i=3;

    while(i < n && tokenizedQuery[i] != "IN") {
        parsedQuery.sortColumnNames.push_back(tokenizedQuery[i]);
        i++;
    }

    if(i == n) {
        cout << "SYNTAX ERROR\n";
        return false;
    }

    i++;
    while(i < n) {
        if(tokenizedQuery[i] == "ASC") {
            parsedQuery.sortingStrategies.push_back(ASC);
        } else if(tokenizedQuery[i] == "DESC") {
            parsedQuery.sortingStrategies.push_back(DESC);
        } else {
            cout << "SYNTAX ERROR\n";
            return false;
        }
        i++;
    }

    if(parsedQuery.sortColumnNames.size() != parsedQuery.sortingStrategies.size()) {
        cout << "SYNTAX ERROR\n";
        return false;
    }

    return true;
}

bool semanticParseInplaceSORT() {
    logger.log("semanticParseInplaceSORT");

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)) {
        cout << "SEMANTIC ERROR: Relation doesn't exist\n";
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

bool rowComp(vector<int> &row1, vector<int> &row2) {
    for(int i=0; i<parsedQuery.sortColumnNames.size(); i++) {
        int colIdx = parsedQuery.sortColumnIndices[i];
        SortingStrategy order = parsedQuery.sortingStrategies[i];

        if(order == ASC) {
            if(row1[colIdx] < row2[colIdx]) return true;
            else if(row1[colIdx] > row2[colIdx]) return false;
        } else {
            if(row1[colIdx] > row2[colIdx]) return true;
            else if(row1[colIdx] < row2[colIdx]) return false;
        }
    }

    return false;
}

void createTempTable(string tempTableName, Table* table) {
    Table* tempTable = new Table();
    tempTable->tableName = tempTableName;
    tempTable->columns = table->columns;
    tempTable->distinctValuesPerColumnCount = table->distinctValuesPerColumnCount;
    tempTable->columnCount = table->columnCount;
    tempTable->rowCount = table->rowCount;
    tempTable->blockCount = table->blockCount;
    tempTable->maxRowsPerBlock = table->maxRowsPerBlock;
    tempTable->rowsPerBlockCount = table->rowsPerBlockCount;
    tempTable->indexed = table->indexed;
    tempTable->indexedColumn = table->indexedColumn;
    tempTable->indexingStrategy = table->indexingStrategy;
    tableCatalogue.insertTable(tempTable);
}

void copySortedTable(string sortedTableName, string tableName, bool isNewTable) {
    logger.log("copySortedTable");
    Table* sortedTable = tableCatalogue.getTable(sortedTableName);
    if(isNewTable) {
        createTempTable(tableName, sortedTable);
    }

    for(int pageIdx=0; pageIdx<sortedTable->blockCount; pageIdx++) {
        vector<vector<int> > rows;
        Cursor cursor(sortedTableName, pageIdx);

        for(int rowIdx=0; rowIdx<sortedTable->rowsPerBlockCount[pageIdx]; rowIdx++) {
            rows.push_back(cursor.getNext());
        }

        bufferManager.writePage(tableName, pageIdx, rows, rows.size());
    }

    tableCatalogue.deleteTable(sortedTableName);
}

void sortTable(bool isNewTable, string newTableName) {
    logger.log("sortTable");
    // Phase 1: sorting chunks of relation
    Table* table = tableCatalogue.getTable(parsedQuery.sortRelationName);

    // Maintain a vector containing the column indices in the order of their sorting priority
    for(int i=0; i<parsedQuery.sortColumnNames.size(); i++) {
        int colIdx = table->getColumnIndex(parsedQuery.sortColumnNames[i]);
        parsedQuery.sortColumnIndices.push_back(colIdx);
    }
    
    string tempTableName = table->tableName + "_temp0";
    createTempTable(tempTableName, table);

    for(int pageIdx=0; pageIdx<table->blockCount; pageIdx++) {
        vector<vector<int> > rows;

        Cursor cursor(table->tableName, pageIdx);

        for(int rowIdx=0; rowIdx<table->rowsPerBlockCount[pageIdx]; rowIdx++) {
            rows.push_back(cursor.getNext());
        }

        sort(rows.begin(), rows.end(), rowComp);

        bufferManager.writePage(tempTableName, pageIdx, rows, rows.size());
    }

    // Phase 2: merging the sorted chunks of relation
    int sortedPages = 1;
    int runNumber = 0;

    while(sortedPages < table->blockCount) {
        int filesCount = table->blockCount / sortedPages;
        if(table->blockCount % sortedPages) filesCount++;

        runNumber++;
        string tempTableNamePrev = table->tableName + "_temp" + to_string(runNumber-1);
        string tempTableNameNext = table->tableName + "_temp" + to_string(runNumber);
        createTempTable(tempTableNameNext, table);
        int newPageIdx = 0;

        int fileIdx = 0;
        for(; fileIdx<filesCount-1; fileIdx+=2) {
            int startPageIdxFile1 = fileIdx * sortedPages;
            int startPageIdxFile2 = (fileIdx+1) * sortedPages;
            int endPageIdxFile1 = min(startPageIdxFile1+sortedPages, (int)table->blockCount);
            int endPageIdxFile2 = min(startPageIdxFile2+sortedPages, (int)table->blockCount);
            
            Cursor cursorFile1(tempTableNamePrev, startPageIdxFile1);
            Cursor cursorFile2(tempTableNamePrev, startPageIdxFile2);
            vector<vector<int> > rows;
            vector<int> rowFile1 = cursorFile1.getNext();
            vector<int> rowFile2 = cursorFile2.getNext();

            while((cursorFile1.pageIndex < endPageIdxFile1 && !rowFile1.empty()) && 
            (cursorFile2.pageIndex < endPageIdxFile2 && !rowFile2.empty())) {
                if(rowComp(rowFile1, rowFile2)) {
                    rows.push_back(rowFile1);
                    rowFile1 = cursorFile1.getNext();
                } else {
                    rows.push_back(rowFile2);
                    rowFile2 = cursorFile2.getNext();
                }

                if(rows.size() == table->maxRowsPerBlock) {
                    bufferManager.writePage(tempTableNameNext, newPageIdx, rows, rows.size());
                    newPageIdx++;
                    rows.clear();
                }
            }

            while(cursorFile1.pageIndex < endPageIdxFile1 && !rowFile1.empty()) {
                rows.push_back(rowFile1);
                rowFile1 = cursorFile1.getNext();

                if(rows.size() == table->maxRowsPerBlock) {
                    bufferManager.writePage(tempTableNameNext, newPageIdx, rows, rows.size());
                    newPageIdx++;
                    rows.clear();
                }
            }

            while(cursorFile2.pageIndex < endPageIdxFile2 && !rowFile2.empty()) {
                rows.push_back(rowFile2);
                rowFile2 = cursorFile2.getNext();

                if(rows.size() == table->maxRowsPerBlock) {
                    bufferManager.writePage(tempTableNameNext, newPageIdx, rows, rows.size());
                    newPageIdx++;
                    rows.clear();
                }
            }

            if(!rows.empty()) {
                bufferManager.writePage(tempTableNameNext, newPageIdx, rows, rows.size());
                newPageIdx++;
                rows.clear();
            }
        }

        if(fileIdx < filesCount) {
            int startPageIdx = fileIdx * sortedPages;
            int endPageIdx = min(startPageIdx+sortedPages, (int)table->blockCount);

            for(int pageIdx = startPageIdx; pageIdx < endPageIdx; pageIdx++) {
                string oldPageName = "../data/temp/" + tempTableNamePrev + "_Page" + to_string(pageIdx);
                string newPageName = "../data/temp/" + tempTableNameNext + "_Page" + to_string(pageIdx);
                rename(oldPageName.c_str(), newPageName.c_str());
            }
            bufferManager.cleanBufferManager();
        }

        tableCatalogue.deleteTable(tempTableNamePrev);

        sortedPages *= 2;
    }

    // Copy the sorted temp table back to the original table
    tempTableName = table->tableName + "_temp" + to_string(runNumber);
    copySortedTable(tempTableName, newTableName, isNewTable);
}

void executeInplaceSORT() {
    logger.log("executeInplaceSORT");

    sortTable(false, parsedQuery.sortRelationName);
}