#include "BMgr.h"

using namespace std;


int main()
{
    BMgr* bmgr = new BMgr("data/data.dbf");
    for (int i = 0; i < MAXPAGES; i++)
    {
        bmgr->FixNewPage();
    }
    

    
    return 0;
}