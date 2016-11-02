#pragma once

#include <fstream>
#include <vector>
#include <string>

using namespace std;

const double eps = 1e-5;
const int val_limit = 20; //для ГСЧ: от -val/2 до val/2

template <typename T>
class Grid
{
protected:
	T *a;
	int m, n;
	// Construct grid and fill with random numbers
	Grid(int rows_n = 0, int columns_n = 0, bool set_null = false);
	// Load grid from text file. File format:
	// <rows> <columns> <rows*columns numbers>
	Grid(ifstream &file);
	Grid(T *_a, int _m, int _n): a(_a), m(_m), n(_n) {if(!m||!n) a=nullptr; };
public:
	// Move assignment operator for temporary objects
	Grid<T> & operator = (Grid<T> &&x);
	// Assignment operator. Makes a deep copy of an object
	Grid<T> & operator = (const Grid<T> &x);
	// Fill the grid with value
	Grid<T> & operator = (T value);
	// Write to text file. Format:
	// <rows> <columns> <rows*columns numbers>
	void write(ofstream &out) const;
	// Copy constructor. Allocates memory and copies all elements
	Grid(const Grid<T> &other);
	// Move constructor from c++11 standard
	Grid(Grid<T> &&x): a(x.a), m(x.m), n(x.n) { x.a = nullptr; }
	// Destructor, yeah
	~Grid();
	// Prints the grid row by row
	void print() const;
	// Returns a reference to the element (row, col)
	T &operator() (int row, int col);
	// For constant matrices
	T operator() (int row, int col) const;
	//Getters
	int get_m() const { return m; }
	int get_n() const { return n; }
};

template <typename T>
class Vector: public Grid<T>
{
	// Deprecated. Use operator []
	T &operator() (int row, int col);
	T operator() (int row, int col) const;
public:
	// Vector is a grid with cols = 1
	Vector(int _m = 0): Grid<T>(_m, 1) {}
	T &operator[] (int row) { return Grid<T>::operator ()(row, 0); };
	T operator[] (int row) const {return Grid<T>::operator ()(row, 0); };
	int size() {return Grid<T>::m; };
};

template <typename T>
class Matrix: public Grid<T>
{
	string mode = "ijk";
	int bl_size = 32;
public:
	// Matrix is a grid with operations
	Matrix(int _m = 0, int _n = 0, bool set_null = false): Grid<T>(_m, _n, set_null) {}
	Matrix(ifstream &file): Grid<T>(file) {};
	// Search for max non-diagonal element in symmetric matrix
	vector<int> find_max() const;
	// Multiply matrix by a rotation matrix from right hand side
	void rotate_right(const Matrix & rotation, int row, int col);
	// Same, but from left hand side
	void rotate_left(const Matrix & rotation, int row, int col);
	// Takes the right upper corner of a matrix and reflects it
	Matrix<T> & symmetrize();
	void set_mode(const string _mode, int _bl_size);
	Matrix<T> operator * (const Matrix<T> & b) const;
	Matrix<T> block_mult(const Matrix<T> & b) const;
	Matrix(T *_a, int _m, int _n): Grid<T>(_a, _m, _n) {};
};
#include "matrix.hpp"
