//
// Created by anson on 2020/12/16.
//

#include "HLT_basic_elgamal.h"

#include "BasicStrategyTable.h"
#include "BasicElgamalTable.h"
#include "network.hpp"
#include "../../dectree/common/dectree.h"

#define PROT 0 //0 for HHH, 1 for HH(G), 2 for (GG)/(HG)H where the parts in brackets are executed outside of this code before/after
#define DT 0 //0 for wine", 1 for iris, 2 for breast cancer, 3 for digits, 4 for diabetes, 5 for linnerud, 6 for boston

using namespace std;

cybozu::RandomGenerator rg;

const uint32_t bitlen = 64;
uint32_t ElGamalBits = 514;
uint32_t Buflen = ElGamalBits / 8 + 1; //size of one ciphertext to send via network. Paillier uses n bits == n/8 bytes


void SysInit()
{
    const mcl::EcParam& para = mcl::ecparam::secp256k1;
    Zn::init(para.n);
    Fp::init(para.p);
    Ec::init(para.a, para.b);
}

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

BasicStrategyTable gen_strategy_table(DecTree tree, string dectree_filename){
//    StrategyTable s(7, 7);
//    for (size_t i = 0; i < 7; i++)
//    {
//        s.insert_compare_uint_binary(OPERATE::ESMALLER, i, i+1);
//    }
//    s.save_table("test.txt");
    BasicStrategyTable s(tree.num_dec_nodes, tree.num_attributes);
    for (size_t i = 0; i < tree.num_dec_nodes; i++)
    {
        s.insert_compare_uint(OPERATE::ESMALLER, tree.attributes[i], tree.thresholds[i]);
    }
    s.save_table("test.txt");

    return s;
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


    const mcl::EcParam& para = mcl::ecparam::secp256k1;
    const Fp x0(para.gx);
    const Fp y0(para.gy);
    const Ec P(x0, y0);

    Elgamal::PrivateKey prv;
    prv.init(P, para.bitSize, rg);
    Elgamal::PublicKey pub = prv.getPublicKey();

    BasicStrategyTable s = gen_strategy_table(tree,filename[i]);
    BasicElgamalTable t = BasicElgamalTable(s);
    t.set_secret_key(prv);
    t.set_public_key(pub);
    t.encrypto_table(s);

    t.save_encrypto_table("encrypto_table_"+filename[i]);

    conn << filename[i] << '\n';
    conn << tree.num_attributes  << '\n';
    conn << tree.num_dec_nodes  << '\n';

    conn << pub;

    Elgamal::CipherText* packing_value_en = new Elgamal::CipherText [tree.num_dec_nodes];

    for(int i = 0; i < tree.num_dec_nodes; i++){
        packing_value_en[i] = t.get_packing_value()[i];
        conn >> packing_value_en[i];
    }

    gettimeofday(&tbegin,NULL);
    Zn *pack = new Zn[tree.num_dec_nodes];
    for(int i =0; i < tree.num_dec_nodes; i++){
        prv.dec(pack[i],packing_value_en[i],100000);
    }
    gettimeofday(&tend,NULL);
    t_online += ((tend.tv_sec-tbegin.tv_sec)*1000000 + tend.tv_usec - tbegin.tv_usec);

    cout << "Comp Offline: " << dec << t_offline << "us" << endl;
    cout << "Comp Online: " << dec << t_online << "us" << endl;

}


void play_client(tcp::iostream &conn){

    string dectree_filename;
    uint32_t feature;
    uint32_t size;

    conn >> dectree_filename;
    conn >> feature;
    conn >> size;

    Elgamal::PublicKey pub;
    conn >> pub;

    BasicElgamalTable t(feature, size);
    t.load_encrpto_table("encrypto_table_"+dectree_filename);

    unsigned int select[feature];

    for(int i=0;i < feature; i++){
        select[i] = i;
        t.select_uint(i, select[i]);
    }

    unsigned int* node_shares = new unsigned int[size];


    for(int i=0;i<size;i++){
        node_shares[i] = rand() % 2;
    }
    t.compute_share(node_shares);

    Elgamal::CipherText* packing_value_en = new Elgamal::CipherText [size];

    for(int i = 0; i < size; i++){
        packing_value_en[i] = t.get_packing_value()[i];
        conn << packing_value_en[i];
    }

}


int main(int argc, char *argv[]) {
    long r = 0; //0 for server, 1 for client
    if (argc > 1)
        r = std::stol(argv[1]);
    SysInit();
    std::srand(std::time(0));

    switch (r) {
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