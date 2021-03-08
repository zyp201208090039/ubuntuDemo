#include <unistd.h>

int main()
{
    execlp("dmidecode", "dmidecode", "-t1", "-t2", nullptr);
    return 0;
}
