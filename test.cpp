#include <iostream>
int main(){
    for (int i = 1; i <= 5; i++){
        static int a = i;
        std::cout << a;
    }
}