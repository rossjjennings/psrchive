#ifndef MATRIX_H
#define MATRIX_H

template<class T>
class Matrix
{
public:
	//Resizes the matrix to the specifications
	//(SHOULD IT ONLY RESIZE SMALLER OR CAN IT RESIZE TO A LARGER SIZE?, IF IT CAN DOES IT FILL IT WITH ZEROES?) AM I ASSUMING ALL MATRICES WILL BE SQUARE?
	virtual void resize( int nRow,  int nCol) = 0;
	virtual T getElement(int i, int j)const = 0;
	virtual void setElement(int i, int j, T value) = 0;
	
	virtual int getSize()const = 0; //Assumes Matrix is Square

	virtual void setZero() = 0; //Sets all elements in the matrix to zero
	virtual void setIdentity(); //Sets the matrix to the identity matrix
	virtual void invert() = 0; //Inverts the matrix

	virtual Matrix<T>* operator*(const Matrix<T>* RHS) = 0;	//Matrix Multiplication
	virtual Matrix<T>* operator+(const Matrix<T>* RHS) = 0; //Matrix Addition
	virtual Matrix<T>* operator-(const Matrix<T>* RHS) = 0; //Matrix Subtraction
	virtual void operator=(const Matrix<T>* RHS); //Makes the elements in the matrix the same as the one passed, probably really slow performance

	virtual ~Matrix(){}; //ERROR ????

};


template<typename T>
void Matrix<T>::setIdentity()
{
	setZero();

	for(int i = 0; i < this->getSize(); i++)
	{
		setElement(i, i, 1);
	}
}

template<typename T>
void Matrix<T>::operator=(const Matrix<T>* RHS)
{
	//Resize if neccessary
	if(this->getSize() != RHS->getSize() )
		this->resize(RHS->getSize(), RHS->getSize() );

	//Set the elements
	for(int i = 0; i < RHS->getSize(); i++)
	{
		for(int j = 0; j < RHS->getSize(); j++)
		{
			this->setElement(i, j, RHS->getElement(i, j));
		}
	}
}




#endif
