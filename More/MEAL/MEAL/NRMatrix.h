#ifndef VECTOR_MATRIX_H
#define VECTOR_MATRIX_H

#include "Matrix.h"
#include "GaussJordan.h"
#include <vector>

template<class T>
class VectorMatrix : public Matrix<T>
{

private:
	std::vector<std::vector<T> > _matrix;

	void setMatrix(const std::vector<std::vector<T> > *matrix);

public:

	VectorMatrix(std::vector<std::vector<T> > matrix);
	VectorMatrix(size_t nRows = 0, size_t nCols = 0);
	~VectorMatrix();


	void resize( size_t nRow,  size_t nCol);
	T getElement(int i, int j) const;
	void setElement( int i,  int j, T value);
	void invert();

	std::vector<std::vector<T> > getMatrix();
	size_t getSize() const;
	std::vector<std::vector<T> > getMatrix() const;

	void setZero();

	Matrix<T>* operator*(const Matrix<T>* RHS);
	Matrix<T>* operator+(const Matrix<T>* RHS);
	Matrix<T>* operator-(const Matrix<T>* RHS);
	void operator=(const VectorMatrix<T>* RHS);

};


template<typename T>
VectorMatrix<T>::VectorMatrix(std::vector<std::vector<T> > matrix) : Matrix<T>()
{
	_matrix = matrix;
}


template<typename T>
VectorMatrix<T>::VectorMatrix(size_t nRows, size_t nCols) : Matrix<T>()
{
	for(size_t i = 0; i < nRows; i++)
	{
		std::vector<T> temp;
		_matrix.push_back(temp);

		for(size_t j = 0; j < nCols; j++)
		{
			_matrix[i].push_back(0);
		}
	}
}

template<typename T>
VectorMatrix<T>::~VectorMatrix()
{
}



template<typename T>
void VectorMatrix<T>::resize(size_t nRow, size_t nCol)
{
	//Determine if it needs to get smaller or bigger
	bool bigger = false;

	//tests to see if rows need to get bigger
	if(nRow > _matrix.size() )
	{
		bigger = true;
	}

	//tests to see if cols need to get bigger
	for(size_t i = 0; i < _matrix.size(); i++)
	{
		if(_matrix[i].size() < nCol)
		{
			bigger = true;
			break;
		}
	}

	//Make the matrix bigger
	if(bigger)
	{
		//Make it bigger
		//cout << "Bigger" << endl;

		//Add extra cols to existing rows if needed
		for(size_t i = 0; i < _matrix.size(); i++)
		{
			if(_matrix[i].size() < nCol)
			{
				while(_matrix[i].size() != nCol)
					_matrix[i].push_back(0);
			}
		}

		//Add extra rows if needed
		if(nRow > _matrix.size())
		{
			size_t amount = nRow - _matrix.size();
			for(size_t i = 0; i < amount; i++)
			{
				size_t currentNum = 0;
				std::vector<T> vec;

				//Pad the new row with 0's
				while(currentNum != nCol)
				{
					vec.push_back(0);
					currentNum++;
				}
				_matrix.push_back(vec);
			}
		}
	}
	//else make it smaller
	else
	{
		//cout << "Smaller" << endl;

	  typename std::vector<std::vector<T> >::iterator start;
	  typename std::vector<T>::iterator colStart;

		//delete unwanted rows
		if(nRow <_matrix.size())
		{
			start = _matrix.begin() + nRow;
			_matrix.erase(start, _matrix.end() );
		}

		//delete unwanted column elements in the remaining rows
		for(size_t i = 0; i < _matrix.size(); i++)
		{
			if(_matrix[i].size() > nCol)
			{
				colStart = _matrix[i].begin() + nCol;
				_matrix[i].erase(colStart, _matrix[i].end() );
			}
		}
	}
}


template<typename T>
void VectorMatrix<T>::invert()
{
        std::vector<std::vector<T> > dummy;
	MEAL::GaussJordan(_matrix, dummy);
}

template<typename T>
T VectorMatrix<T>::getElement(int i, int j) const
{
	return _matrix[i][j];
}


template<typename T>
void VectorMatrix<T>::setElement(int i, int j, T value)
{
	_matrix[i][j] = value;
}


template<typename T>
std::vector<std::vector<T> > VectorMatrix<T>::getMatrix() const
{
	return _matrix;
}

template<typename T>
std::vector<std::vector<T> > VectorMatrix<T>::getMatrix()
{
	return _matrix;
}

template<typename T>
void VectorMatrix<T>::setMatrix(const std::vector<std::vector<T> >* matrix)
{
	_matrix = matrix;
}


template<typename T>
void VectorMatrix<T>::setZero()
{
		for(size_t i = 0; i < _matrix.size(); i++)
		{
			for(size_t j = 0; j < _matrix.size(); j++)
			{
				_matrix[i][j] = 0;
			}
		}
}

template<typename T>
size_t VectorMatrix<T>::getSize() const
{
	return _matrix.size();
}


template<typename T>
Matrix<T>* VectorMatrix<T>::operator*(const Matrix<T>* RHS)
{
	if(_matrix.size() != RHS->getSize() || _matrix.size() != RHS->getSize())
		return NULL;

	VectorMatrix<T> *result = new VectorMatrix<T>(); //Create the new result matrix
	result->resize(_matrix.size(), _matrix.size()); //Resize the matrix

	//For each row
	for(size_t row = 0; row < _matrix.size(); row++)
	{
		//for each element in that row
		for(size_t element = 0; element < _matrix[row].size(); element++)
		{
			//calculate the value for the current element
			for(size_t i = 0; i < _matrix.size(); i++)
			{
				result->setElement(row, element, (result->getElement(row, element) ) + _matrix[row][i] * RHS->getElement(i,element));
			}
		}
	}

	return result;
}



template<typename T>
Matrix<T>* VectorMatrix<T>::operator+(const Matrix<T>* RHS)
{
	if(_matrix.size() != RHS->getSize() || _matrix.size() != RHS->getSize())
		return NULL;

	VectorMatrix<T> *result = new VectorMatrix<T>(); //Create the new result matrix
	result->resize(_matrix.size(), _matrix.size()); //Resize the result matrix

		//For each row
	for(size_t row = 0; row < _matrix.size(); row++)
	{
		//for each element in that row
		for(size_t element = 0; element < _matrix[row].size(); element++)
		{
			result->setElement(row, element, _matrix[row][element] + RHS->getElement(row, element) );
		}
	}

	return result;
}


template<typename T>
Matrix<T>* VectorMatrix<T>::operator-(const Matrix<T>* RHS)
{
	if(_matrix.size() != RHS->getSize() || _matrix.size() != RHS->getSize())
		return NULL;

	VectorMatrix<T> *result = new VectorMatrix<T>(); //Create the new result matrix
	result->resize(_matrix.size(), _matrix.size()); //Resize the result matrix

		//For each row
	for(size_t row = 0; row < _matrix.size(); row++)
	{
		//for each element in that row
		for(size_t element = 0; element < _matrix[row].size(); element++)
		{
			result->setElement(row, element, _matrix[row][element] - RHS->getElement(row, element) );
		}
	}

	return result;
}


template<typename T>
void VectorMatrix<T>::operator=(const VectorMatrix<T>* RHS)
{
	if(_matrix.size() != RHS->getSize())
		this->resize(RHS->getSize(), RHS->getSize() );

	for(size_t i = 0; i < _matrix.size(); i++)
	{
		for(size_t j = 0; j < _matrix.size(); j++)
		{
			setElement(i, j, RHS->getElement(i, j) );
		}
	}
}



#endif
