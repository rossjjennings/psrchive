#ifndef GSLMATRIX_H
#define GSLMATRIX_H

#include <gsl/gsl_matrix_double.h> //Science Library header file
#include <gsl/gsl_linalg.h>
#include "Matrix.h"

template<typename T>
class GSLMatrix : public Matrix<T>
{
private:
	gsl_matrix *_matrix; //Pointer to the gsl_matrix struct
	void setMatrix(gsl_matrix* newMatrix);

public:
	GSLMatrix(int nRow, int nCol);
	~GSLMatrix();

	void resize(int nRow, int nCol); //If the matrix has elements in it should it keep those elements or is it ok if they are lost?
	
	T getElement(int i, int j) const;
	void setElement(int i, int j, T value);
	gsl_matrix* getMatrix() const;
	
	int getSize() const;
	void setZero();
	void invert();

	Matrix<T>* operator*(const Matrix<T>* RHS); 
	Matrix<T>* operator+(const Matrix<T>* RHS); 
	Matrix<T>* operator-(const Matrix<T>* RHS);
	void operator=(const GSLMatrix<T>* RHS);

};


template<typename T>
GSLMatrix<T>::GSLMatrix(int nRow, int nCol)
{
	_matrix = gsl_matrix_alloc(nRow, nCol);
}


template<typename T>
GSLMatrix<T>::~GSLMatrix()
{
	if(_matrix != NULL)
		gsl_matrix_free(_matrix);
}


template<typename T>
void GSLMatrix<T>::setMatrix(gsl_matrix* newMatrix)
{
	//Free the old matrix, if there is one
	if(_matrix != NULL)
		gsl_matrix_free(_matrix);

	_matrix = newMatrix;
}


template<typename T>
void GSLMatrix<T>::resize(int nRow, int nCol)
{
	if(_matrix != NULL)
		gsl_matrix_free(_matrix);

	_matrix = NULL;

	_matrix = gsl_matrix_alloc(nRow, nCol);
}


template<typename T>
T GSLMatrix<T>::getElement(int i, int j) const
{
	return gsl_matrix_get(_matrix, i, j);
}


template<typename T>
void GSLMatrix<T>::setElement(int i, int j, T value)
{
	gsl_matrix_set(_matrix, i, j, value);
}


template<typename T>
void GSLMatrix<T>::invert()
{
	int sigNum;
	
	//gsl_matrix for the output
	gsl_matrix* outputMatrix = gsl_matrix_alloc(_matrix->size1, _matrix->size1 );

	//Allocate permutation matrix
	gsl_permutation* permutationM = gsl_permutation_alloc(_matrix->size1);

	//get the LU decomposition of the matrix
	gsl_linalg_LU_decomp((gsl_matrix*) _matrix, permutationM, &sigNum);

	//invert the matrix
	gsl_linalg_LU_invert(_matrix, permutationM, outputMatrix);

	this->setMatrix(outputMatrix);
	
	gsl_permutation_free(permutationM);
}



template<typename T>
gsl_matrix* GSLMatrix<T>::getMatrix() const
{
	return _matrix;
}


template<typename T>
int GSLMatrix<T>::getSize() const
{
	return _matrix->size1;
}

template<typename T>
void GSLMatrix<T>::setZero()
{
	gsl_matrix_set_zero (_matrix);
}


template<typename T>
Matrix<T>* GSLMatrix<T>::operator*(const Matrix<T>* RHS)
{
  //Check if its the same size
  if(_matrix->size1 != RHS->getSize() || _matrix->size1 != _matrix->size1)
    return NULL;

  //Check if its the same type (GSL Matrix)
  if(dynamic_cast<const GSLMatrix<T>*> (RHS) == NULL)
    throw Error (InvalidParam, "GSLMatrix<T>::operator*",
		 "RHS is not a GSLMatrix");

  //Create the new result matrix
  GSLMatrix<T> *result = new GSLMatrix<T>(_matrix->size1, _matrix->size1);
  result->setZero();

  //For each row
  for(int row = 0; row < _matrix->size1; row++)
  {
    //for each element in that row
    for(int element = 0; element < _matrix->size2; element++)
    {
      //calculate the value for the current element
      for(int i = 0; i < _matrix->size1; i++)
      {
	result->setElement(row, element, (result->getElement(row, element) ) + this->getElement(row, i) * RHS->getElement(i,element));
      }
    }
  }
  
  return result;
}


template<typename T>
Matrix<T>* GSLMatrix<T>::operator+(const Matrix<T>* RHS)
{
  //Check if its the same size
  if(_matrix->size1 != RHS->getSize() || _matrix->size1 != _matrix->size1)
    return NULL;

  //Check if its the same type (GSL Matrix)
  if(dynamic_cast<const GSLMatrix<T>*> (RHS) == NULL)
    throw Error (InvalidParam, "GSLMatrix<T>::operator*",
		 "RHS is not a GSLMatrix");

	GSLMatrix<T> *result = new GSLMatrix<T>(_matrix->size1, _matrix->size1); //Create the new result matrix
	result->setZero();

	//For each row
	for(int row = 0; row < _matrix->size1; row++)
	{
		//for each element in that row
		for(int element = 0; element < _matrix->size1; element++)
		{
			result->setElement(row, element, this->getElement(row, element) + RHS->getElement(row, element) );
		}
	}

	return result;
}


template<typename T>
Matrix<T>* GSLMatrix<T>::operator-(const Matrix<T>* RHS)
{
	if(_matrix->size1 != RHS->getSize() || _matrix->size1 != _matrix->size1) //Check if its the same size
			return NULL;

  //Check if its the same type (GSL Matrix)
  if(dynamic_cast<const GSLMatrix<T>*> (RHS) == NULL)
    throw Error (InvalidParam, "GSLMatrix<T>::operator*",
		 "RHS is not a GSLMatrix");

	GSLMatrix<T> *result = new GSLMatrix<T>(RHS->getSize(), RHS->getSize()); //Create the new result matrix
	result->setZero();

	//For each row
	for(int row = 0; row < _matrix->size1; row++)
	{
		//for each element in that row
		for(int element = 0; element < _matrix->size1; element++)
		{
			result->setElement(row, element, this->getElement(row, element) - RHS->getElement(row, element) );
		}
	}

	return result;
}


template<typename T>
void GSLMatrix<T>::operator=(const GSLMatrix<T>* RHS)
{
	if(_matrix->size1 != RHS->getSize() )
	{
		gsl_matrix_free(_matrix);
		gsl_matrix_alloc(RHS->getSize(), RHS->getSize());
	}

	gsl_matrix_memcpy(_matrix, RHS->getMatrix());
}


#endif
