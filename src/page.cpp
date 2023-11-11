#include "global.h"
/**
 * @brief Construct a new Page object. Never used as part of the code
 *
 */
Page::Page()
{
    this->pageName = "";
    this->tableName = "";
    this->pageIndex = -1;
    this->rowCount = 0;
    this->columnCount = 0;
    this->rows.clear();
}

/**
 * @brief Construct a new Page:: Page object given the table name and page
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<tablename>_Page<pageindex>". For example, If the Page being loaded is of
 * table "R" and the pageIndex is 2 then the file name is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tableName 
 * @param pageIndex 
 */
Page::Page(string tableName, int pageIndex)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->tableName + "_Page" + to_string(pageIndex);
    Table table = *tableCatalogue.getTable(tableName);
    this->columnCount = table.columnCount;
    uint maxRowCount = table.maxRowsPerBlock;
    vector<int> row(columnCount, 0);
    this->rows.assign(maxRowCount, row);

    ifstream fin(pageName, ios::in);
    this->rowCount = table.rowsPerBlockCount[pageIndex];
    int number;
    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
        {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
}

Page::Page(string matrixName,int rowNumberOfPage , int columnNumberOfPage)
{
    logger.log("Page::Matrix page");
    // this->tableName = tableName;
    // this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + matrixName+"_"+to_string(rowNumberOfPage) + "_Page" + to_string(columnNumberOfPage);
    Matrix matrix = *matrixCatalogue.getMatrix(matrixName);

    this->rowCount = matrix.rowsPerBlockCount[rowNumberOfPage][columnNumberOfPage];
    this->columnCount = matrix.columnPerBlockCount[rowNumberOfPage][columnNumberOfPage];

    // uint maxRowCount = table.maxRowsPerBlock;
    vector<int> row(columnCount, 0);
    this->rows.assign(this->rowCount, row);

    ifstream fin(pageName, ios::in);
    
    int number;
    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount ; columnCounter++)
        {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
}

/**
 * @brief Get row from page indexed by rowIndex
 * 
 * @param rowIndex 
 * @return vector<int> 
 */
vector<int> Page::getRow(int rowIndex)
{
    logger.log("Page::getRow");
    vector<int> result;
    result.clear();
    // cout<<this->rowCount<<"hi"<<rowIndex<<endl;
    if (rowIndex >= this->rowCount)
        return result;
    
    return this->rows[rowIndex];
}

Page::Page(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->rows = rows;
    this->rowCount = rowCount;
    this->columnCount = rows[0].size();
    this->pageName = "../data/temp/"+this->tableName + "_Page" + to_string(pageIndex);
}

/**
 * @brief writes current page contents to file.
 * 
 */
void Page::writePage()
{
    logger.log("Page::writePage");
    ofstream fout(this->pageName, ios::trunc);
    int a=this->rowCount;

    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
 
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << this->rows[rowCounter][columnCounter];
        }
        fout << endl;
        
    }
    fout.close();
}


/**
 * @brief reads file to the current page.
 * 
 */
vector<vector<int> > Page::readPage(string matrixName, int rowIndex, int colIndex) {
    logger.log("Page::readPage");
    this->pageName = "../data/temp/" + matrixName + "_" + to_string(rowIndex) + "_Page" + to_string(colIndex);
    ifstream fin(this->pageName);
    Matrix *matrix = matrixCatalogue.getMatrix(matrixName);
    this->rowCount = matrix->rowsPerBlockCount[rowIndex][colIndex];
    this->columnCount = matrix->columnPerBlockCount[rowIndex][colIndex];
    this->rows = vector<vector<int> >(this->rowCount, vector<int>(this->columnCount));

    for(int i=0; i<this->rowCount; i++) {
        for(int j=0; j<this->columnCount; j++) {
            fin >> this->rows[i][j];
        }
    }
    fin.close();

    return this->rows;
}