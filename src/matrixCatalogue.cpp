#include "global.h"

bool MatrixCatalogue::isMatrix(string matrixName)
{
    logger.log("MatrixCatalogue::isMatrix"); 
    if (this->matrixes.count(matrixName))
        return true;
    return false;
}
MatrixCatalogue::~MatrixCatalogue(){
    logger.log("MatrixCatalogue::~MatrixCatalogue"); 
    for(auto matrix: this->matrixes){
        matrix.second->unload();
        delete matrix.second;
    }
}
void MatrixCatalogue::insertMatrix(Matrix* matrix)
{
    logger.log("MatrixCatalogue::insertTable"); 
    this->matrixes[matrix->matrixName] = matrix;
}
Matrix* MatrixCatalogue::getMatrix(string matrixName)
{
    logger.log("MatrixCatalogue::getMatrix"); 
    Matrix *matrix = this->matrixes[matrixName];
    return matrix;
}
void MatrixCatalogue::updateMatrixName(Matrix* matrix,string oldName,string newName)
{
    logger.log("MatrixCatalogue::updateMatrixName"); 
    this->matrixes.erase(oldName);
     this->matrixes[newName]=matrix;
    
    return;
}

