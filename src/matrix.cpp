
#include "global.h"

/**
 * @brief Construct a new Table:: Table object
 *
 */
Matrix::Matrix()
{
    logger.log("Matrix::Matrix");
}


/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName 
 */
Matrix::Matrix(string matrixName)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file. 
 *
 * @param line 
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Matrix::findColumnSize(string firstLine)
{
    logger.log("Matrix::findColumnSize");
    // unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    int colCount=0;
    while (getline(s, word, ','))
    {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        
        colCount++;
        // this->columns.emplace_back(word);
    }
    this->columnCount = colCount;
    // cout<<"no of col = "<<colCount;
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 40000) / (sizeof(int) * this->columnCount));
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row 
 */






/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Matrix::blockify()
{
    logger.log("Matrix::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    
    int maxPagesInRow=ceil(this->columnCount/100.0);
    this->rowsPerBlockCount.assign(maxPagesInRow,vector<uint>(maxPagesInRow,0));
    this->columnPerBlockCount.assign(maxPagesInRow,vector<uint>(maxPagesInRow,0));
   
  
    map<int,vector<vector<int>>> pagesMatrices;
    int currColCount=this->columnCount;
    
  
    int rowCountInPage=0;
    int blocksRow=0;
    
    
    while (getline(fin, line))
    {
        this->rowCount++;
        
        stringstream s(line);

        
       
        int i=0;
        currColCount=this->columnCount;
        while(i<maxPagesInRow){
            int j=0;
            int columnNo=min(100,currColCount);
            vector<int> temp_row;
            while(j<columnNo){
                 if (!getline(s, word, ','))
                    return false;
                 temp_row.push_back(stoi(word));



                j++;
            }
            pagesMatrices[i].push_back(temp_row);
            currColCount-=columnNo;
            if(currColCount==0){
                break;
            }

            i++;

        }
        rowCountInPage++;
        if(rowCountInPage==100){
            string newMatrixName=this->matrixName+"_"+to_string(blocksRow);
            for( int i=0;i<maxPagesInRow;i++ ){
              
                     bufferManager.writePage(newMatrixName, i,pagesMatrices[i], pagesMatrices[i].size());
                     blocksWritten++;
                     this->rowsPerBlockCount[blocksRow][i]= pagesMatrices[i].size();
                     this->columnPerBlockCount[blocksRow][i]= pagesMatrices[i][0].size();
            }
            rowCountInPage=0;
            blocksRow++;
            
            
            pagesMatrices.clear();
        }

      
    }
    if (rowCountInPage)
    {
        string newMatrixName=this->matrixName+"_"+to_string(blocksRow);
            for( int i=0;i<maxPagesInRow;i++ ){
                     bufferManager.writePage(newMatrixName, i,pagesMatrices[i], pagesMatrices[i].size());
                     this->rowsPerBlockCount[blocksRow][i]= pagesMatrices[i].size();
                     this->columnPerBlockCount[blocksRow][i]= pagesMatrices[i][0].size();
                     blocksWritten++;
            }
            rowCountInPage=0;
            blocksRow++;
            pagesMatrices.clear();
    }
    this->pagesCountInRow=blocksRow;
    this->pageCountInColumn=blocksRow;
    logger.log("blocksRow: " + to_string(blocksRow));
    logger.log("pagesCountInRow: " + to_string(this->pagesCountInRow));
    logger.log("pageCountInColumn: " + to_string(this->pageCountInColumn));


    if (this->rowCount == 0)
        return false;
    // this->distinctValuesInColumns.clear();
    return true;
}



/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded 
 * @return false if an error occurred 
 */
bool Matrix::load()
{
    logger.log("Matrix::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->findColumnSize(line))
            if (this->blockify())
                return true;
    }
    fin.close();
    return false;
}
// void Matrix::unload(){
//     logger.log("Matrix::~unload");
//     for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
//         bufferManager.deleteFile(this->matrixName, pageCounter);
//     if (!isPermanent())
//         bufferManager.deleteFile(this->sourceFileName);
// }


void Matrix::unload(){
    logger.log("Matrix::~unload");
  
    for(int i=0;i<this->pagesCountInRow;i++){
        string filename=this->matrixName+"_"+to_string(i);
        for(int j=0;j<this->pageCountInColumn;j++){
            bufferManager.deleteFile(filename, j);
        }
    }
    //  bufferManager.deleteFile(this->sourceFileName);
}



/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Matrix::print()
{
    logger.log("Matrix::print");
    uint count = min((long long)PRINT_COUNT, this->rowCount);

    //print headings
    // this->writeRow(this->columns, cout);
    string newMatrixName=this->matrixName;

    
    

    Cursor cursor(newMatrixName,0,0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext();
         int colsize=row.size();
         vector<int> temprow;
         for(int i=0;i<min(colsize,20);i++){
            temprow.push_back(row[i]);
         }

        this->writeRow(temprow, cout);
    }
    printRowCount(count);
}

bool Matrix::isPermanent()
{
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
    return true;
    return false;
}



/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Matrix::makePermanent()
{
    logger.log("Matrix::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    this->sourceFileName=newSourceFile;
    ofstream fout(newSourceFile, ios::out);

    //print headings
    // this->writeRow(this->columns, fout);
    map<int,vector<int>> pagesInLine;

    for(int i=0;i<this->pagesCountInRow;i++){
        int numOfRowsInCurrentBlock=this->rowsPerBlockCount[i][0];
        
        

        for(int j=0;j<this->pageCountInColumn;j++){
            Cursor cursor(this->matrixName,i,j);
            vector<int> row;
            for(int k=0;k<numOfRowsInCurrentBlock;k++){
                row = cursor.getNext();
                pagesInLine[k].insert(pagesInLine[k].end(),row.begin(),row.end());
            }
            
        }
        for(auto it : pagesInLine){
            
            this->writeRow(it.second, fout);

        }
        pagesInLine.clear();
        

    }
    fout.close();
}






/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName 
 * @param toColumnName 
 */
void Matrix::renameMatrix(string oldMatrixName, string newMatrixName)
{
    logger.log("Matrix::renameMatrix");
    int numOfBlocksInRows=this->pagesCountInRow;
    int numOfBlocksInColumn=this->pageCountInColumn;
    string filePath="../data/temp/";
    for(int i=0;i<numOfBlocksInRows;i++){
        for(int j=0;j<numOfBlocksInColumn;j++){
            string oldFileName=filePath+oldMatrixName+"_"+to_string(i)+"_Page"+to_string(j);
            string newFileName=filePath+newMatrixName+"_"+to_string(i)+"_Page"+to_string(j);
            if(std::rename(oldFileName.c_str(), newFileName.c_str())){
                logger.log("Matrix::renameMatrix Error renaming the file.");
                return;
            }
        }
    }
    this->matrixName= newMatrixName;
    return ;
    
}
/**
 * @brief helper function to perform transpose of the submatrices.
*/
vector<vector<int> > Matrix::submatrixTranspose(vector<vector<int> > submatrix) {
    logger.log("Matrix::submatrixTranspose");
    int totalRows = submatrix.size();
    int totalCols = submatrix[0].size();

    if(totalRows != totalCols) {
        vector<vector<int> > submatrixTranspose(totalCols, vector<int>(totalRows));

        for(int row=0; row<totalRows; row++) {
            for(int col=0; col<totalCols; col++) {
                submatrixTranspose[col][row] = submatrix[row][col];
            }
        }

        return submatrixTranspose;
    }

    for(int row=0; row<totalRows; row++) {
        for(int col=row+1; col<totalCols; col++) {
            int temp = submatrix[row][col];
            submatrix[row][col] = submatrix[col][row];
            submatrix[col][row] = temp;
        }
    }

    return submatrix;
}


/**
 * @brief The transpose function takes the transpose of the
 * matrix IN-PLACE.
*/
void Matrix::transpose() {
    logger.log("Matrix::transpose");
    vector<vector<int> > submatrix1;
    vector<vector<int> > submatrix2;
    string matrixNameWithRow;
           bufferManager.cleanBufferManager();
    for(int pageRow=0; pageRow<this->pagesCountInRow; pageRow++) {
        for(int pageCol=pageRow; pageCol<this->pageCountInColumn; pageCol++) {
         
            if(pageRow == pageCol) {
                submatrix1 = bufferManager.readPage(this->matrixName, pageRow, pageCol);
                blocksRead++;
                submatrix1 = this->submatrixTranspose(submatrix1);
                matrixNameWithRow = this->matrixName + "_" + to_string(pageRow);
                bufferManager.writePage(matrixNameWithRow, pageCol, submatrix1, submatrix1.size());
                blocksWritten++;
                continue;
            }

            submatrix1 = bufferManager.readPage(this->matrixName, pageRow, pageCol);
            blocksRead++;
            submatrix2 = bufferManager.readPage(this->matrixName, pageCol, pageRow);
            blocksRead++;
            
            submatrix1 = this->submatrixTranspose(submatrix1);
            submatrix2 = this->submatrixTranspose(submatrix2);

            matrixNameWithRow = this->matrixName + "_" + to_string(pageRow);
            bufferManager.writePage(matrixNameWithRow, pageCol, submatrix1, submatrix1.size());
            blocksWritten++;
            matrixNameWithRow = this->matrixName + "_" + to_string(pageCol);
            bufferManager.writePage(matrixNameWithRow, pageRow, submatrix2, submatrix2.size());
            blocksWritten++;

            string submatrix1_name = "../data/temp/" + this->matrixName + "_" + to_string(pageRow) + "_Page" + to_string(pageCol);
            string submatrix2_name = "../data/temp/" + this->matrixName + "_" + to_string(pageCol) + "_Page" + to_string(pageRow);
            string temp_name = "../data/temp/temp_name";
            rename(submatrix1_name.c_str(), temp_name.c_str());
            rename(submatrix2_name.c_str(), submatrix1_name.c_str());
            rename(temp_name.c_str(), submatrix2_name.c_str());
        }
    }
}

/**
 * @brief Helper function to check if submatrix is symmetric.
 * @return true if submatrix is symmetric otherwise returns false.
*/
bool Matrix::checkSubmatrixSymmetry(vector<vector<int > > submatrix1, vector<vector<int > > submatrix2) {
    logger.log("Matrix::checkSubmatrixSymmetry");

    submatrix1 = this->submatrixTranspose(submatrix1);
    int totalRows = submatrix1.size();
    int totalCols = submatrix1[0].size();

    for(int row=0; row<totalRows; row++) {
        for(int col=0; col<totalCols; col++) {
            if(submatrix1[row][col] != submatrix2[row][col])
                return false;
        }
    }

    return true;
}

/**
 * @brief Function checks if the current matrix is symmetric or not.
 * @return true if matrix is symmetric otherwise returns false.
*/
bool Matrix::checkSymmetry() {
    logger.log("Matrix::checkSymmetry");
    vector<vector<int> > submatrix1;
    vector<vector<int> > submatrix2;

    for(int pageRow=0; pageRow<this->pagesCountInRow; pageRow++) {
        for(int pageCol=pageRow; pageCol<this->pageCountInColumn; pageCol++) {
            if(pageRow == pageCol) {
                submatrix1 = bufferManager.readPage(this->matrixName, pageRow, pageCol);
                blocksRead++;

                if(!this->checkSubmatrixSymmetry(submatrix1, submatrix1))
                    return false;

                continue;
            }

            submatrix1 = bufferManager.readPage(this->matrixName, pageRow, pageCol);
            blocksRead++;
            submatrix2 = bufferManager.readPage(this->matrixName, pageCol, pageRow);
            blocksRead++;

            if(!this->checkSubmatrixSymmetry(submatrix1, submatrix2)) 
                return false;

        }
    }
    
    return true;
}

/**
 * @brief Function to copy the current matrix. Used for COMPUTE command.
*/
Matrix* Matrix::copy(string copyMatrixName) {
    logger.log("Matrix::copy");

    // copy metadata of current matrix
    Matrix *newMatrix = new Matrix();
    newMatrix->matrixName = copyMatrixName;
    newMatrix->columnCount = this->columnCount;
    newMatrix->rowCount = this->rowCount;
    newMatrix->blockCount = this->blockCount;
    newMatrix->maxRowsPerBlock = this->maxRowsPerBlock;
    newMatrix->pagesCountInRow = this->pagesCountInRow;
    newMatrix->pageCountInColumn = this->pageCountInColumn;
    newMatrix->rowsPerBlockCount = this->rowsPerBlockCount;
    newMatrix->columnPerBlockCount = this->columnPerBlockCount;
    newMatrix->indexed = this->indexed;
    matrixCatalogue.insertMatrix(newMatrix);

    // copy the blocks of current matrix into the new copy matrix
    for(int pageRow=0; pageRow<this->pagesCountInRow; pageRow++) {
        string matrixNamewithRow = newMatrix->matrixName+"_"+to_string(pageRow);

        for(int pageCol=0; pageCol<this->pageCountInColumn; pageCol++) {
            vector<vector<int> > block = bufferManager.readPage(this->matrixName, pageRow, pageCol);
            blocksRead++;
            bufferManager.writePage(matrixNamewithRow, pageCol, block, block.size());
            blocksWritten++;
        }
    }

    return newMatrix;
}

/**
 * @brief compute Function calculates originalMatrix - currentMatrix 
 * and stores the result in the currentMatrix.
 * @param orginalMatrix object of the matrix from which to subtract the current matrix.
*/
void Matrix::compute(Matrix* originalMatrix) {
    logger.log("Matrix::compute");
    vector<vector<int> > submatrix1;
    vector<vector<int> > submatrix2;

    for(int pageRow=0; pageRow<this->pagesCountInRow; pageRow++) {
        string matrixNamewithRow = this->matrixName+"_"+to_string(pageRow);

        for(int pageCol=0; pageCol<this->pageCountInColumn; pageCol++) {
            submatrix1 = bufferManager.readPage(this->matrixName, pageRow, pageCol);
            blocksRead++;
            submatrix2 = bufferManager.readPage(originalMatrix->matrixName, pageRow, pageCol);
            blocksRead++;
            int totalRows = submatrix1.size();
            int totalCols = submatrix1[0].size();

            for(int row=0; row<totalRows; row++) {
                for(int col=0; col<totalCols; col++) {
                    submatrix1[row][col] = submatrix2[row][col] - submatrix1[row][col];
                }
            }

            bufferManager.writePage(matrixNamewithRow, pageCol, submatrix1, submatrix1.size());
            blocksWritten++;
        }
    }
}