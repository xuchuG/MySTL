#include <stdlib.h>
#include <iostream>
#include <list>
#include <vector>
using namespace std;

union a{
	union a* p;
	char b[8];
};

class {
	int c;
};
enum {
	e
};

template<class T>
class test2 {

};

template <class T>
class test {
public:
	typedef test2<T> T1;
	typedef test2<T*> T2;
	T1 t1;
	T2 t2;
	test(T1 t):t1(t),
		t2(t){}
};




#include <deque>
int main() {
	ptrdiff_t t;
	int a[5];
	int* ptr1 = a;
	int* ptr2 = &a[4];
	t = ptr1-ptr2;
	cout << t <<endl;
	
	deque<int> d(1000);
	deque<int>::iterator d_it1 = d.begin();
	deque<int>::iterator d_it2 = d_it1 + 900;
	cout << d_it1 - d_it2 << endl;

	//test2<int> t1;
	//test<int> ttt(t1);

	const int& ii = int(10);

	cout << ii;

}