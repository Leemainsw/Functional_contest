#include <iostream>
#include <vector>
using namespace std;
int main() {
	vector<int> a;
	a.push_back(10);
	a.push_back(20);

	vector<int>::iterator iter;
	for (iter = a.begin(); iter != a.end(); iter++)
		printf("%5d", *iter);
	printf("\n");

	return 0;
}