// test main function for primary exe
#include "myprojectheader.h"
#include "lib.h"

int main(void)
{
	std::cout << "hello world\n";
	
	MyStruct s;
	s.print();
	lib::print();

	return 0;
}
