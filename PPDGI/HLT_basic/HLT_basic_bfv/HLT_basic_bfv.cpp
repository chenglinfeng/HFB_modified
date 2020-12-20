//
// Created by anson on 2020/11/30.
//
#include <iostream>
#include <HFB/OTOfflineUtile.h>
#include <HFB/OTOneUtil.h>
#include "network.hpp"
#include "HLT_basic_bfv.h"
#include "../../dectree/common/dectree.h"
#include "BasicStrategyTable.h"
#include "BasicSealTable.h"
using namespace std;

#define DT 1 //0 for wine", 1 for iris, 2 for breast cancer, 3 for digits, 4 for diabetes, 5 for linnerud, 6 for boston



void send_arr(unsigned char* arr, unsigned int arr_len, tcp::iostream &conn){


//    conn << arr_len << '\n';
    cout << "----------start send------------" << endl;
    cout << "sender arr_len: " << arr_len << endl;
//    unsigned char tmp0 = '0';
//    conn << tmp0;
//    cout << "send tmp=" << tmp0 << endl;
    conn.write(reinterpret_cast<const char *>(arr), arr_len);
//    for(int i = 0; i < arr_len; i++){
//        conn << arr[i] << '\n';
//    }
    conn.flush();
    cout << "all send" << endl;
    for(int i = 0; i < arr_len; i++){
        cout<< hex << (unsigned int)arr[i];
    }
    cout << endl;
    unsigned char tmp = '3';
    conn << tmp;
    cout << "send tmp=" << tmp << endl;
//    conn.flush();
    cout <<"----------send finished------------" << endl;
}

void recv_arr(unsigned char *arr, unsigned int arr_len, tcp::iostream &conn){


//    conn >> arr_len;
    cout <<"----------start receive------------" << endl;
    cout << "receiver arr_len: " << arr_len << endl;
//    unsigned char tmp0;
//    conn >> tmp0;
//    cout << "receive tmp0=" << tmp0 << endl;

    conn.read(reinterpret_cast<char *>(arr), arr_len);
//    for(int i = 0; i < arr_len; i++){
//        conn >> arr[i];
//    }
    cout<<"all receive"<<endl;
    for(int i = 0; i < arr_len; i++){
        cout<< hex << (unsigned int)arr[i];
    }
    cout << endl;
    unsigned char tmp;
    conn >> tmp;
    cout << "receive tmp=" << tmp << endl;
    cout <<"----------receive finished------------" << endl;
}

BasicSealTable gen_seal_table(DecTree tree, string dectree_filename){
    BasicStrategyTable s(tree.num_dec_nodes, tree.num_attributes);
    for (size_t i = 0; i < tree.num_dec_nodes; i++)
    {
        s.insert_compare_uint(OPERATE::BIGGER, tree.attributes[i], tree.thresholds[i]);
    }
    BasicSealTable t(s);
    t.gen_keypair();
    t.encrypto_table(s);
    return t;
}


void play_server(tcp::iostream &conn){

    timeval tbegin, tend;
    long int t_offline = 0;
    long int t_online = 0;


    const string filename[7] = {
            "/home/anson/Desktop/GBDT_Predict/HFB/dectree/UCI_dectrees/wine",
            "/home/anson/Desktop/GBDT_Predict/HFB/dectree/UCI_dectrees/iris",
            "/home/anson/Desktop/GBDT_Predict/HFB/dectree/UCI_dectrees/breast",
            "/home/anson/Desktop/GBDT_Predict/HFB/dectree/UCI_dectrees/digits",
            "/home/anson/Desktop/GBDT_Predict/HFB/dectree/UCI_dectrees/diabetes",
            "/home/anson/Desktop/GBDT_Predict/HFB/dectree/UCI_dectrees/linnerud",
            "/home/anson/Desktop/GBDT_Predict/HFB/dectree/UCI_dectrees/boston"
    };
    uint32_t i = DT;

    DecTree tree;
    tree.read_from_file(filename[i]);

    tree.depthFullPad(); //full padding for testing


    /*######################################*/
    // one ot offline
    gettimeofday(&tbegin, NULL);
    BasicSealTable t = gen_seal_table(tree,filename[i]);
    gettimeofday(&tend,NULL);
    t_offline += ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec);
    cout << "Comp Offline(gen seal table): " << dec << ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec) << "us" << endl;


//    unsigned int m_encrypto_table_branch = 8;
//    unsigned int m_encrypto_table_items = 16;
//
//    for (unsigned int i = 0; i < m_encrypto_table_branch; i++)
//    {
//        for (unsigned int j = 0; j < m_encrypto_table_items * tree.num_attributes; j++)
//        {
//            t.get_encrypto_table()[i][j].save(conn);
//        }
//    }
//    conn << t.get_public_key() << '\n';

    t.save_encrypto_table("encrypto_table_"+filename[i]);
//    t.save_secret_key("secret_key_"+filename[i]);
//    t.save_public_key("public_key_"+filename[i]);

    conn << filename[i] << '\n';

    conn << tree.num_attributes << '\n';
    conn << tree.num_dec_nodes << '\n';

//    gettimeofday(&tbegin,NULL);
//    unsigned char* a_half;
//    unsigned int a_half_size = 33;
//    a_half = new unsigned char[a_half_size];
//    OTOfflineUtile sender;
//    unsigned int one_ot_size = tree.num_dec_nodes;
//    sender.offline_sender_gen_A_half(a_half);
//    gettimeofday(&tend,NULL);
//    t_offline += ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec);
//    cout << "Comp Offline(gen a half): " << dec << ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec) << "us" << endl;
//
//
//    cout << "server to send" << endl;
//    for(int i = 0; i < a_half_size; i++){
//        cout<< hex << (unsigned int)a_half[i];
//    }
//    cout << endl;
//    conn << a_half_size;
//    send_arr(a_half, a_half_size, conn);
//
//    unsigned char **b_half;
//    b_half = new unsigned char *[one_ot_size];
//
//    unsigned int one_ot_n = 256;
//
//    unsigned int b_half_size;
//    conn >> b_half_size;
//    cout << "b_half_size: " << b_half_size << endl;
//    unsigned char *b_half_array = new unsigned char[b_half_size*one_ot_size];
//    recv_arr(b_half_array,b_half_size*one_ot_size,conn);
//    for(int i=0;i<one_ot_size;i++){
//        memcpy(b_half[i],b_half_array+i*b_half_size,b_half_size);
//    }
////    for(int i = 0; i < one_ot_size; i++){
////        cout << "b_half_size: " << b_half_size << endl;
////        b_half[i] = new unsigned char[b_half_size];
////        recv_arr(b_half[i], b_half_size, conn);
////
////    }
//    cout << "b_half all received" << endl;
//
//    gettimeofday(&tbegin,NULL);
//
////    conn >> one_ot_n;
////    cout << "one_ot_n:" << one_ot_n << endl;
//    unsigned int *one_ot_key_set;
//    one_ot_key_set = new unsigned int[one_ot_size * one_ot_n];
//
//    for (size_t i = 0; i < one_ot_size; i++)
//    {
//        /* gen key set*/
//        sender.offline_sender_gen_key_set(one_ot_key_set + i * one_ot_n, one_ot_n, b_half[i]);
//    }
//
//    gettimeofday(&tend,NULL);
//    t_offline += ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec);
//    cout << "Comp Offline(gen key set): " << dec << ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec) << "us" << endl;
//
//    for (size_t i = 0; i < one_ot_size; i++)
//    {
//        delete[] b_half[i];
//    }
//    delete[] b_half;


//
//    int tmp = 1;
//    conn << tmp;
//    cout << filename[i] << endl;

//    ofstream out = t.encrypto_table_to_stream();
//    conn << out.rdbuf();
//    out.flush();

    unsigned int packing_value_en_len;
    unsigned char* packing_value_en;

    conn >> packing_value_en_len;
    packing_value_en = new unsigned char[packing_value_en_len];
    recv_arr(packing_value_en, packing_value_en_len, conn);


//    /*######################################*/
//    // one ot online
//
//    unsigned char *offset;
//    unsigned int offset_size;
//    conn >> offset_size;
//    cout << "offset_size: " << offset_size << endl;
//    offset = new unsigned char[offset_size];
//    recv_arr(offset, offset_size, conn);
//
//    unsigned char *one_share;    //mode has
//
//    /*init */
//    gettimeofday(&tbegin,NULL);
//
//    one_share = new unsigned char[one_ot_size];
//    memset(one_share, 0, one_ot_size);
//    for (size_t i = 0; i < one_ot_size; i++)
//    {
//        one_share[i] = rand() % 2;
//        // one_share[i] = 0;
//    }

//    t.load_secret_key("secret_key");
//    t.load_packing_value_string(packing_value_en, packing_value_en_len);

    gettimeofday(&tbegin,NULL);
    unsigned char *pack = new unsigned char[tree.num_dec_nodes];
    unsigned int pakc_len = tree.num_dec_nodes;
    memset(pack, 0, pakc_len);
    t.decrypto_packing_value(pack, pakc_len);
    gettimeofday(&tend,NULL);
    t_online += ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec);

//    /*############## encode share #################*/
//    unsigned char **node_exs = new unsigned char *[one_ot_size];
//    for (size_t i = 0; i < one_ot_size; i++)
//    {
//        node_exs[i] = new unsigned char[one_ot_n];
//    }
//    OTOneUtil::encode_nodes(node_exs, pack, pakc_len);
//
//    /*##############   calculate with share #################*/
//    OTOneUtil::calculate_with_shares_node_exs(node_exs, one_ot_size, one_share, one_ot_size);
//
//    /*############## encrypto node_exs and compress #################*/
//    unsigned char *compress_node_exs = new unsigned char[one_ot_size * 32];
//
//    OTOneUtil::online_sender_en_node_exs(compress_node_exs, node_exs, one_ot_key_set, offset, one_ot_size);
//
//    gettimeofday(&tend,NULL);
//    t_online += ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec);
//
//    /*#####################################################################*/
//    conn << one_ot_size*32;
//    send_arr(compress_node_exs, one_ot_size * 32, conn);
//
//    gettimeofday(&tend, NULL);
    cout << "Comp Offline: " << dec << t_offline << "us" << endl;
    cout << "Comp Online: " << dec << t_online << "us" << endl;

//    /*#####################################################################*/
//    for (size_t i = 0; i < one_ot_size; i++)
//    {
//        delete[] node_exs[i];
//    }
//
//    delete[] offset;
//    delete[] compress_node_exs;
//    delete[] node_exs;
//    delete[] pack;
//    delete[] one_ot_key_set;
//
//    for (size_t i = 0; i < one_ot_size; i++)
//    {
//        printf("%x ", one_share[i]);
//    }
//    cout << endl;

}
void play_client(tcp::iostream &conn){
    timeval tbegin, tend;
    long int t_offline = 0;
    long int t_online = 0;

    string dectree_filename;
    unsigned int features;
    unsigned int size;

    conn >> dectree_filename;
    conn >> features;
    conn >> size;
    cout << "num_decision_node: " << size << endl;

//    /*######################################*/
//    // one ot offline
//    OTOfflineUtile receiver;
//
//    unsigned char* a_half;
//    unsigned int a_half_length;
//
//    cout << "a_half_length: " << a_half_length << endl;
//    conn >> a_half_length;
//    cout << "a_half_length: " << a_half_length << endl;
//    a_half = new unsigned char[a_half_length];
//    recv_arr(a_half, a_half_length,conn);
//    cout << "a_half_length: " << a_half_length << endl;
//
//
//    unsigned int *one_ot_key;
//    unsigned int one_ot_size = size;
//    unsigned int one_ot_n = 256;
//    unsigned int one_ot_offline_choice = 0;
//    unsigned int b_half_size = a_half_length;
//    unsigned char **b_half;
//    b_half = new unsigned char *[one_ot_size];
//
//
//    gettimeofday(&tbegin, NULL);
//    one_ot_key = new unsigned int[one_ot_size];
//
//    for (size_t i = 0; i < one_ot_size; i++)
//    {
//        b_half[i] = new unsigned char[33];
//
//        /* gen key */
//        one_ot_key[i] = receiver.offline_receiver_gen_key(b_half[i], a_half, one_ot_offline_choice, one_ot_n);
//    }
//    gettimeofday(&tend, NULL);
//    t_offline += ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec);
//    cout << "Comp Offline(gen receiver key): " << dec << ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec) << "us" << endl;
//
//    // conn << b_half_size << '\n';
//    cout << "b_half_size: " << b_half_size << endl;
//    conn << b_half_size;
//
//    unsigned char *b_half_array = new unsigned char[b_half_size*one_ot_size];
//    for(int i=0;i<one_ot_size;i++){
//        memcpy(b_half_array+i*b_half_size, b_half[i], b_half_size);
//    }
//    send_arr(b_half_array, b_half_size*one_ot_size, conn);
////    for(int i = 0; i < one_ot_size; i++){
////        send_arr(b_half[i], b_half_size, conn);
////    }
//    cout << "b_half all send" << endl;
//
////    conn << one_ot_n << '\n';
////    cout << "one_ot_n: " << one_ot_n << endl;
//
//    for (size_t i = 0; i < one_ot_size; i++)
//    {
//        delete[] b_half[i];
//    }
//    delete[] b_half;


    /*######################################*/
    // one ot online

    BasicSealTable t(features, size);
    unsigned int node_shares_len = size;
    t.load_encrpto_table("encrypto_table"+dectree_filename);
//    ifstream in;
//    conn >> in.rdbuf();
//    t.stream_to_encrypto_table(in,t);

    gettimeofday(&tbegin, NULL);

    unsigned int select[features];

    for (size_t i = 0; i < features; i++)
    {
        select[i] = i;
        t.select_uint(i, select[i]);
    }
    unsigned  char* node_shares = (unsigned char*)malloc(size * sizeof(unsigned char));

    for (size_t i = 0; i < node_shares_len; i++)
    {
        node_shares[i] = rand() % 256;
    }

    unsigned char* packing_value_en;
    unsigned int packing_value_en_len = 16457;
    packing_value_en = new unsigned char[packing_value_en_len];

    t.compute_share(node_shares, node_shares_len);
    t.save_packing_value_string(packing_value_en, packing_value_en_len);
    t.release_packing_value();

    gettimeofday(&tend, NULL);
    t_online += ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec);
    cout << "Comp Online(compute sum and convert to share): " << dec << ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec) << "us" << endl;

    conn << packing_value_en_len;
    send_arr(packing_value_en, packing_value_en_len, conn);




////    gettimeofday(&tbegin,NULL);
//
//    unsigned char *one_re_share; // user has
//
//    one_re_share = new unsigned char[one_ot_size];
//    memset(one_re_share, 0, one_ot_size);
//    /*############## user first get offset respond to model ##############*/
//    unsigned char *offset = new unsigned char[one_ot_size];
//    for (size_t i = 0; i < one_ot_size; i++)
//    {
//        offset[i] = (node_shares[i] - (unsigned char)one_ot_offline_choice) % one_ot_n;
//    }
//    /*#####################################################################*/
////    gettimeofday(&tend,NULL);
////    t_online += ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec);
////    cout << "Comp Online(gen offset): " << dec << ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec) << "us" << endl;
//
//
//    cout << "one_ot_size: " << one_ot_size << endl;
//    conn << one_ot_size;
//    send_arr(offset, one_ot_size, conn);
//
//
//    unsigned char *compress_node_exs;
//    unsigned int compress_node_exs_size;
//    conn >> compress_node_exs_size;
//
//    //gettimeofday(&tbegin,NULL);
//    compress_node_exs = new unsigned char[compress_node_exs_size];
//    recv_arr(compress_node_exs, compress_node_exs_size, conn);
//
//    /*############## user receiver do ###########################################*/
//    OTOneUtil::online_receiver_de_node_exs(one_re_share, compress_node_exs, node_shares, one_ot_key, one_ot_size);
//
//    gettimeofday(&tend, NULL);
//    t_online += ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec);
//    //cout << "Comp Online(get OTSC share): " << dec << ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec) << "us" << endl;

    cout << "Comp Offline: " << dec << t_offline << "us" << endl;
    cout << "Comp Online: " << dec << t_online << "us" << endl;

//    delete[] offset;
//    delete[] compress_node_exs;
//    delete[] one_ot_key;
//
//    for (size_t i = 0; i < one_ot_size; i++)
//    {
//        printf("%x ", one_re_share[i]);
//    }
//    cout << endl;

}
int main(int argc, char *argv[]) {
    long r = 0; //0 for server, 1 for client
    if (argc > 1)
        r = std::stol(argv[1]);

    switch(r) {
        case 0:
            std::cout << "waiting for client..." << std::endl;
            run_server(play_server);
            break;
        case 1:
            std::cout << "connect to server..." << std::endl;
            run_client(play_client);
            break;
    }
    return 0;
}