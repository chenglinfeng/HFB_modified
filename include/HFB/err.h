/*
 * @Author: xin.xing.lzj
 * @Date: 2019-06-27 09:21:50
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-14 14:45:57
 * @Description: error report head
 */
#include <string>
#include <iostream>

#pragma once

#define BLOCK_SIZE_TOO_BIGGER 0
#define SUCCESSFUL 1
#define INSERT_FAILED -1
#define PARAMS_INPUT_ERROR -2
#define ALLOCATE_MEMORY_FAILED -3
#define INPUT_ARRAY_LEN_TOO_SMALL_ERROR -4
#define LOAD_PUBLIC_KEY_ERROR -5
#define LOAD_SECRET_KEY_ERROR -6

#define OPEN_FILE_ERROR -7
#define SAVE_PUBLIC_KEY_ERROR -8
#define SAVE_SECRET_KEY_ERROR -9
#define ENCRYPTO_TABLE_FAILED -10

#define SAVE_ENCRYPTO_TABLE_FAILED -11
#define LOAD_ENCRYPTO_TABLE_FAILED -12

#define INPUT_VALUE_TOO_SMALL -13
#define SAVE_PACKING_VALUE_FAILED -14
#define PACKING_VALUE_ERROR -15
#define COMPUTE_SHARE_ERRPR -16
#define LOAD_PACKING_VALUE_FAILED -17
#define DECRYPTO_PACKING_VALUE_FAILED -18
#define ADD_RANDOM_NOISE_FAILED -19

#define OT_KEY_SIZE_TOO_SMALL -20

#define OUT_OF_MAX_INDEX -21

#define BUFFER_TOO_SMALLER -22

/**
 * @description: 
 *  error strcut
 */
struct ErrorStruct
{
    int err_code;             /*error code */
    std::string err_descript; /*error description */
};

class ErrorPrinter
{
private:
    static ErrorStruct errs[];

public:
    /**
     * @description: 
     *   print error description with the error code
     * @param[in]  error_code  error code
     * @return: 
     */
    static void print(int error_code);
};
