
#include "HFB/err.h"
using namespace std;

ErrorStruct ErrorPrinter::errs[] = {{BLOCK_SIZE_TOO_BIGGER, "block size is too bigger"},
                                    {SUCCESSFUL, "successful"},
                                    {INSERT_FAILED, "insert failed"},
                                    {PARAMS_INPUT_ERROR, "input parameter error"},
                                    {ALLOCATE_MEMORY_FAILED, "allocate memory failed"},
                                    {INPUT_ARRAY_LEN_TOO_SMALL_ERROR, "input array is too small"},
                                    {LOAD_PUBLIC_KEY_ERROR, "load public key error"},
                                    {LOAD_SECRET_KEY_ERROR, "load secret key error"},
                                    {OPEN_FILE_ERROR, "open file error"},
                                    {SAVE_PUBLIC_KEY_ERROR, "save public key error"},
                                    {SAVE_SECRET_KEY_ERROR, "save secret key error"},
                                    {ENCRYPTO_TABLE_FAILED, "encrypto table failed"},
                                    {SAVE_ENCRYPTO_TABLE_FAILED, "save encrypto table failed"},
                                    {LOAD_ENCRYPTO_TABLE_FAILED, "load encrypto table failed"},
                                    {INPUT_VALUE_TOO_SMALL, "input value too small"},
                                    {SAVE_PACKING_VALUE_FAILED, "save packing value failed"},
                                    {PACKING_VALUE_ERROR, "packing value error"},
                                    {COMPUTE_SHARE_ERRPR, "compute value error"},
                                    {LOAD_PACKING_VALUE_FAILED, "load packing value failed"},
                                    {DECRYPTO_PACKING_VALUE_FAILED, "decrypto packing value failed"},
                                    {ADD_RANDOM_NOISE_FAILED, "add random noise failed"},
                                    {0XFFFF, ""}};

void ErrorPrinter::print(int error_code)
{
    int i = 0;
    while (errs[i].err_code != 0XFFFF)
    {
        if (error_code == errs[i].err_code)
        {
            cout << errs[i].err_descript << endl;
            return;
        }
        i++;
    }
}
