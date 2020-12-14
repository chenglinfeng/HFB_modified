/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-10 16:05:25
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-23 08:30:30
 * @Description: 
 */
#include "test.h"
#include "HFB/err.h"
#include "HFB/strategytb.h"
#include "HFB/sealtable.h"
#include <stdlib.h>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <ctime>
using namespace std;

void gen_seal_table()
{

    StrategyTable s((NODE_SIZE) / 2);

    for (size_t i = 1; i <= (NODE_SIZE) / 2; i++)
    {
        s.insert_compare_uint(OPERATE::EBIGGER, i - 1, i);
    }
    s.save_table("test.txt");

    SealTable t(s);
    t.gen_keypair();
    t.encrypto_table(s);

    t.save_encrypto_table("encrypto_table");
    t.save_secret_key("secret_key");
    t.save_public_key("public_key");
}

unsigned char node_shares[(NODE_SIZE) / 2] = {0}; //user

unsigned char packing_value_en[16457] = {0};
unsigned int packing_value_en_len = 16457;

void user_select_by_en_table()
{
    SealTable t((NODE_SIZE) / 2, (NODE_SIZE) / 2);
    unsigned int node_shares_len = (NODE_SIZE) / 2;
    t.load_encrpto_table("encrypto_table");

    unsigned int select[(NODE_SIZE) / 2];

    
    for (size_t i = 0; i < (NODE_SIZE) / 2; i++)
    {
        select[i] = i;
        t.select_uint(i, select[i]);
    }

    for (size_t i = 0; i < node_shares_len; i++)
    {
        node_shares[i] = rand() % 256;
    }

    t.compute_share(node_shares, node_shares_len);
    t.save_packing_value_string(packing_value_en, packing_value_en_len);
    t.release_packing_value();
}

int main()
{
    srand((unsigned)time(NULL));
    gen_seal_table();
    user_select_by_en_table();
    ot_one_offline();
    ot_one_do();

    for (size_t i = 0; i < (NODE_SIZE) / 2; i++)
    {
        printf("%x ", one_re_share[i]);
    }
    cout << endl;
    for (size_t i = 0; i < (NODE_SIZE) / 2; i++)
    {
        printf("%x ", one_share[i]);
    }
    cout << endl;

    ot_two_offline();
    ot_two_do();

    for (size_t i = 0; i < two_ot_size; i++)
    {
        printf("%x ", two_re_share[i]);
    }
    cout << endl;
    for (size_t i = 0; i < two_ot_size; i++)
    {
        printf("%x ", two_share[i]);
    }
    cout << endl;

    ot_three_offline();
    ot_three_do();
    cout << " three" << endl;
    for (size_t i = 0; i < three_ot_size; i++)
    {
        printf("%u ", random_num_re[i]);
    }
    cout << endl;
    for (size_t i = 0; i < three_ot_size; i++)
    {
        printf("%u ", random_num[i]);
    }
    cout << endl;

    cout << "test" << endl;

    return 0;
}
