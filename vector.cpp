#include<iostream>
#include<initializer_list>
using namespace std;

/*
    Self-implemented simplified vector with copy/move operations 
    Reference: 
        Programming Chapter 18
        https://www.learncpp.com/cpp-tutorial/15-3-move-constructors-and-move-assignment/
*/

class Vector{

public:
    unsigned int _size;              // (Actual) _size -> the _size that the vector IS holding
    double* _elem;
    unsigned int _capacity;          // (Virtual) _capacity  -> the _size that the vector COULD hold

public:
    Vector(initializer_list<double> lst);
    Vector(const Vector& a);            // Copy constructor
    Vector& operator=(const Vector&);   // Copy assignment
   

    Vector(Vector&& a);                 // Move constructor
    Vector& operator=(Vector&& a);      // Move assignment

    double& operator[](int index);                  // Get/set by index (version for non-const objects)
    const double& operator[](int index) const;      // Get/set by index (version for const objects that CANNOT change the value at certain index)


    void reserve(int new_capacity);      // Extend the vector with more (virtual) _capacity
    void resize(int new_size);           // Extend the vector with more items(initialize to default)
    void push_back(double item);        

    unsigned int size() const{return this->_size;}
    unsigned int capacity() const{return this->_capacity;}
    void print() const;
};


Vector::Vector(initializer_list<double> lst)
    :_size{lst.size()}, _elem{new double[_size]}, _capacity{_size} {
    cout<<"Initializer list constructor called!"<<endl;
    copy(lst.begin(), lst.end(), this->_elem);
}



Vector::Vector(const Vector& a)
    :_size(a._size), _elem(new double[_size]) {
    cout<<"Copy constructor called!"<<endl;
    copy(a._elem, a._elem + a._size, _elem);
}


Vector& Vector::operator=(const Vector& a){
    cout<<"Copy assignment called!"<<endl;
    double* p = new double[a._size];
    copy(a._elem, a._elem + a._size, p);
    delete[] this->_elem;
    this->_elem = p;         // New _elem
    this->_size = a._size ;   // New _size
    return *this;
}



Vector::Vector(Vector&& a)
    :_size{a._size}, _elem{a._elem} {
    cout<<"Move constructor called!"<<endl;
    a._elem = nullptr;        // To prevent dangling pointer
    a._size = 0;
}


Vector& Vector::operator=(Vector&& a){
    cout<<"Move assignment called!"<<endl;
    delete[] this->_elem; 
    this->_elem = a._elem;         // New _elem
    cout<<a._elem<<endl;
    this->_size = a._size ;        // New _size
    a._elem = nullptr;
    a._size = 0;
    return *this;
}


double& Vector::operator[](int index) {         // Get/set by index (version for non-const objects)
    cout<<"Getter/setter version 1 called!"<<endl;
    return this->_elem[index];
}

const double& Vector::operator[](int index) const{     // Get by index (version for const objects that CANNOT change the value at certain index)
    cout<<"Getter/setter version 2 called!"<<endl;
    // _elem[index]=987;                // See https://stackoverflow.com/questions/7459867/preventing-a-const-member-function-from-changing-a-member-array
    return this->_elem[index];
}


void Vector::reserve(int new_capacity){      // Extend the vector with more (virtual) _capacity
    if(new_capacity <= _capacity) return;     // If new_capacity is smaller than current _capacity
    double* p = new double[new_capacity];
    for(int i=0; i<this->_size; ++i){
        p[i] = this->_elem[i];
    }
    delete[] _elem;
    this->_elem = p;
    this->_capacity = new_capacity;
}


void Vector::resize(int new_size){           // Extend the vector with more items (initialize to default)
    reserve(new_size);           // First, reserve enough _capacity
    for(int i=_size; i<new_size; ++i){
        this->_elem[i] = 0;      //initialize to default
    }
    this->_size = new_size;
}


void Vector::push_back(double item){
    if(0 == this->_capacity){
        reserve(8);
    }
    else if(this->_capacity == this->_size){
        reserve(2*_size);
    }

    this->_elem[_size++] = item;
}




void Vector::print() const{
    for(int i=0; i<_size; i++){
        cout<<_elem[i]<<" ";
    } cout<<endl;
}



// Testing helper function
Vector fill(double x){
    Vector res({x, x});
    cout<<"return: "<<&res<<endl;
    return res;    
}




int main(){

    /* Comment everything except the first explicit constructor and try the code below to see shallow copy behaviours
    Vector v1({1,5,3});
    Vector v2({1,2,3,4});
    cout<<&v1._size<<"  "<<v1._elem<<"  "<<&v1<<"  "<<&v1._elem<<endl;
    cout<<&v2._size<<"  "<<v2._elem<<"  "<<&v2<<"  "<<&v2._elem<<endl<<endl;
    
    v2 = v1;
    cout<<&v1._size<<"  "<<v1._elem<<"  "<<&v1<<"  "<<&v1._elem<<endl;
    cout<<&v2._size<<"  "<<v2._elem<<"  "<<&v2<<"  "<<&v2._elem<<endl;     // Shallow copy
    */

   
    cout<<"===== 1.Test copy assignment ====="<<endl;
    Vector v1({1,5,3});
    Vector v2({1,2,3,4});
    cout<<v1._elem<<endl;
    cout<<v2._elem<<endl;
    cout<<&v1<<endl;
    cout<<&v2<<endl;   
    v2.print();
    // v2.operator=(v1);
    v2 = v1;                      // Copy assignment (NOT move assignment, because v1 is not an 'rvalue')
                                  // BTW, if explicit copy assignment unprovided, the default copy constructor would perform SHALLOW copy of pointer!
                                  // (The pointer _elem is copied, but not the double array it is pointing to. This is dangerous)
    cout<<&v1<<endl;
    cout<<&v2<<endl;
    v2.print();
    cout<<v1._elem<<endl;
    cout<<v2._elem<<endl<<endl;



    cout<<"===== 2.Test move constructor ====="<<endl;
    Vector v3 = fill(3.5);       // Assign by reference   (QUESTION: why assign by reference, instead of calling move/copy constructor?)
                                 // According to: https://en.cppreference.com/w/cpp/language/move_constructor
                                 // [When the initializer is a prvalue, the move constructor call is often optimized out (until C++17) or never made (since C++17)]
    // Vector v3(fill(3.5));        // Assign by reference (same as above)
    cout<<&v3<<endl<<endl;


    cout<<"===== 3.Test move assignment ====="<<endl;
    Vector v4({1,2,3});
    cout<<v4._elem<<endl;
    v4 = fill(3.5);             // Move assignment (or Copy assignment, if move assignment doesn't exist)
                                // If move assignment DOES exist, the compiler is 'smart enough' to call move assignment, because it knows the return of fill() will disappear
    // cout<<&v4<<endl;
    cout<<v4._elem<<endl;
    v4.print();
    cout<<endl;


    cout<<"===== 4.Getter/setter tests ====="<<endl;
    double item = v1[1];        // Get
    cout<<item<<endl;
    v1[1] = 2;                  // Set
    v1.print();
    
    const Vector v5(v1);
    item = v5[1];               // Get
    cout<<item<<endl;
    // v5[1] = 555;                  // Set (should raise compile error -> that's what we want and why we need to set 2 versions of getter & setter!)
    v5.print();
    cout<<endl;

    cout<<"===== 5.Test push_back ====="<<endl;
    v1.print();
    cout<<"Capacity: "<<v1.capacity()<<",  Size: "<<v1.size()<<endl;
    
    v1.push_back(400);
    v1.print();
    cout<<"Capacity: "<<v1.capacity()<<",  Size: "<<v1.size()<<endl; 

    v1.resize(5);
    v1.print();  
    cout<<"Capacity: "<<v1.capacity()<<",  Size: "<<v1.size()<<endl; 
}

