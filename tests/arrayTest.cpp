#include<iostream>
using namespace std;
extern "C" int* fill();

//int main()
//{
//  int* arr = fill();
//  for (int i = 0; i<10; i++)
//  {
//    cout << arr[i] << endl;
//  }
//}

int printInt(int val)
{
  cout << val << endl;
  return 1;
}
