#include <iostream>
#include <save.h>
#include <thread>

using namespace std;

int main()
{
    save save;
    std::thread Son(&save::son,&save);
    std::thread Father(&save::father,&save);
    Son.join();
    Father.join();

    getchar();

    return 0;
}
