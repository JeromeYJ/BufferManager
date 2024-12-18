#include "BMgr.h"

using namespace std;


int main()
{
    // 创建数据库文件
    BMgr* bmgr = new BMgr("data/data.dbf");
    for (int i = 0; i < MAXPAGES; i++)
        bmgr->FixNewPage();
    
    FILE* request_file = fopen("data/data-5w-50w-zipf.txt", "r");
    int page_id, op;
    clock_t start = clock();
    for (int i = 0; i < REQUESTNUM; i++)
    {
        fscanf(request_file, "%d,%d", &op, &page_id);
        bmgr->FixPage(page_id, op);
        bmgr->UnfixPage(page_id);
    }

    printf("hit number: %lld\n", bmgr->GetNumHit());
    printf("hit rate: %.2lf%%\n", bmgr->GetNumHit() * 100.0 / REQUESTNUM );
    printf("io number: %lld\n", bmgr->GetNumDiskIO());
    printf("time taken: %.2fs\n", (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}