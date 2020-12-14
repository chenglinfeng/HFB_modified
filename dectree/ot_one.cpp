/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-19 13:35:21
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-22 16:48:14
 * @Description: 
 */
#include "decision_tree_test.h"
#include "common/dectree.h"
#include "HFB/err.h"
#include "HFB/strategytb.h"
#include "HFB/sealtable.h"
#include <seal/seal.h>
#include <stdlib.h>
#include <fstream>
#include <time.h>

#include <ctime>
using namespace seal;
using namespace std;
#include <sys/time.h>
#include <morse/rot/sender.h>
#include <morse/rot/receiver.h>
#include <morse/ctrdrbg/ctrdrbg.h>
#include <morse/ctrdrbg/rdrand.h>

#include <HFB/OTOfflineUtile.h>
#include <HFB/OTOneUtil.h>

unsigned int *one_ot_key;
unsigned int *one_ot_key_set;
unsigned int one_ot_size; // all node ot for one ot
unsigned int one_ot_n = 256;
unsigned int one_ot_offline_choice = 0;

timeval tbegin, tend;

void ot_one_offline()
{

    OTOfflineUtile sender;
    OTOfflineUtile receiver;

    /*######################################*/
    //sender do
    gettimeofday(&tbegin, NULL);
    unsigned char a_half[33] = {0};
    sender.offline_sender_gen_A_half(a_half);

    /*#############################################*/
    /*############ statr  node size ot ##############*/
    /*#############################################*/
    // receiver do
    unsigned char **b_half;
    one_ot_size = num_decision_node;
    b_half = new unsigned char *[one_ot_size];
    one_ot_key = new unsigned int[one_ot_size];

    for (size_t i = 0; i < one_ot_size; i++)
    {
        b_half[i] = new unsigned char[33];

        /* gen key */
        one_ot_key[i] = receiver.offline_receiver_gen_key(b_half[i], a_half, one_ot_offline_choice, one_ot_n);
    }
    gettimeofday(&tend, NULL);
    cout << "Receiver Comp Offline: " << ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec) << "us" << endl;
    /*######################################*/

    //sender do
    one_ot_key_set = new unsigned int[one_ot_size * one_ot_n];
    gettimeofday(&tbegin, NULL);
    for (size_t i = 0; i < one_ot_size; i++)
    {
        /* gen key set*/
        sender.offline_sender_gen_key_set(one_ot_key_set + i * one_ot_n, one_ot_n, b_half[i]);
    }
    gettimeofday(&tend, NULL);
    cout << "Sender Comp Offline: " << ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec) << "us" << endl;
    /*######################################*/

    for (size_t i = 0; i < one_ot_size; i++)
    {
        delete[] b_half[i];
    }
    delete[] b_half;
}

unsigned char *one_share;    //mode has
unsigned char *one_re_share; // user has

void ot_one_do()
{
    /*init */
    one_share = new unsigned char[one_ot_size];
    memset(one_share, 0, one_ot_size);
    for (size_t i = 0; i < one_ot_size; i++)
    {
        one_share[i] = rand() % 2;
        // one_share[i] = 0;
    }

    one_re_share = new unsigned char[one_ot_size];
    memset(one_re_share, 0, one_ot_size);
    /*############## user first get offset respond to model ##############*/
    unsigned char *offset = new unsigned char[one_ot_size];
    for (size_t i = 0; i < one_ot_size; i++)
    {
        offset[i] = (node_shares[i] - (unsigned char)one_ot_offline_choice) % one_ot_n;
    }
    /*#####################################################################*/

    /*############## mode sender ###########################################*/
    /*############## get pack value  #################*/
    unsigned char *pack = new unsigned char[num_decision_node];
    memset(pack, 0, num_decision_node);
    unsigned int pakc_len = num_decision_node;
    SealTable t(0, 0, 0, 0);
    t.load_secret_key("secret_key");
    t.load_packing_value_string(packing_value_en, packing_value_en_len);

    gettimeofday(&tbegin, NULL);
    t.decrypto_packing_value(pack, pakc_len);

    /*############## encode share #################*/
    unsigned char **node_exs = new unsigned char *[one_ot_size];
    for (size_t i = 0; i < one_ot_size; i++)
    {
        node_exs[i] = new unsigned char[one_ot_n];
    }
    OTOneUtil::encode_nodes(node_exs, pack, pakc_len);

    /*##############   calculate with share #################*/
    OTOneUtil::calculate_with_shares_node_exs(node_exs, one_ot_size, one_share, one_ot_size);

    /*############## encrypto node_exs and compress #################*/
    unsigned char *compress_node_exs = new unsigned char[one_ot_size * 32];

    OTOneUtil::online_sender_en_node_exs(compress_node_exs, node_exs, one_ot_key_set, offset, one_ot_size);

    /*#####################################################################*/
    gettimeofday(&tend, NULL);
    cout << "Sender Comp Online: " << ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec) << "us" << endl;

    gettimeofday(&tbegin, NULL);
    /*############## user receiver do ###########################################*/
    OTOneUtil::online_receiver_de_node_exs(one_re_share, compress_node_exs, node_shares, one_ot_key, one_ot_size);

    gettimeofday(&tend, NULL);
    cout << "Receiver Comp Online: " << ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec) << "us" << endl;

    /*#####################################################################*/
    for (size_t i = 0; i < one_ot_size; i++)
    {
        delete[] node_exs[i];
    }

    delete[] offset;
    delete[] compress_node_exs;
    delete[] node_exs;
    delete[] pack;

    delete[] one_ot_key;
    delete[] one_ot_key_set;
}
