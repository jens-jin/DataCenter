#include<iostream>
#include <stdio.h>
using namespace std;
class A{
 private:
  int x;
 public:
  A(){
   x=0;
  }
  void display(){
   cout<<this<<endl;
   cout<<this->x<<endl;
   cout<<x<<endl;
   cout<<(*this).x<<endl;
  }
};
int main(void){
    A a;
    a.display();
}
