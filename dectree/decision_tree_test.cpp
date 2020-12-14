//
// Created by anson on 2020/11/30.
//

#include "common/dectree.h"
#include "HFB/strategytb.h"
#include "HFB/sealtable.h"
#include "decision_tree_test.h"
using namespace std;

void gen_seal_table(DecTree tree, string dectree_filename)
{

    StrategyTable s(tree.num_dec_nodes, tree.num_attributes);
    for (size_t i = 0; i < tree.num_dec_nodes; i++)
    {
        s.insert_compare_uint(OPERATE::BIGGER, tree.attributes[i], tree.thresholds[i]);
    }
    s.save_table(dectree_filename+".txt");

    SealTable t(s);
    t.gen_keypair();
    t.encrypto_table(s);

    t.save_encrypto_table("encrypto_table_"+dectree_filename);
    t.save_secret_key("secret_key_"+dectree_filename);
    t.save_public_key("public_key_"+dectree_filename);
}
//const int node_size = 31;

unsigned int num_decision_node;
unsigned  char* node_shares;
//unsigned char node_shares[node_size] = {0}; //user

unsigned char packing_value_en[16457] = {0};
unsigned int packing_value_en_len = 16457;

void user_select_by_en_table(DecTree tree, string dectree_filename)
{
    SealTable t(tree.num_attributes, tree.num_dec_nodes);
    unsigned int node_shares_len = tree.num_dec_nodes;
    t.load_encrpto_table("encrypto_table"+dectree_filename);

    unsigned int select[tree.num_attributes];


    for (size_t i = 0; i < tree.num_attributes; i++)
    {
        select[i] = i;
        t.select_uint(i, select[i]);
    }
    node_shares = (unsigned char*)malloc(tree.num_dec_nodes * sizeof(unsigned char));

    for (size_t i = 0; i < node_shares_len; i++)
    {
        node_shares[i] = rand() % 256;
    }

    t.compute_share(node_shares, node_shares_len);
    t.save_packing_value_string(packing_value_en, packing_value_en_len);
    t.release_packing_value();
}


int main(int argc, char** argv) {
    string dectree_rootdir = "/home/anson/Desktop/GBDT_Predict/HFB/dectree/UCI_dectrees/";
    string dectree_filename = "wine";
    DecTree tree;
    num_decision_node = tree.num_dec_nodes;
    tree.read_from_file(dectree_rootdir + dectree_filename);
    tree.depthFullPad();
    // generate seal table
    gen_seal_table(tree, dectree_filename);

    //user_select_by_encrypted table
    user_select_by_en_table(tree,dectree_filename);

    ot_one_offline();
    ot_one_do();

    for (size_t i = 0; i < tree.num_dec_nodes; i++)
    {
        printf("%x\t", one_re_share[i]);
    }
    cout << endl;
    for (size_t i = 0; i < tree.num_dec_nodes; i++)
    {
        printf("%x\t", one_share[i]);
    }
    cout << endl;

    ot_two_offline();
    ot_two_do();

    for (size_t i = 0; i < two_ot_size; i++)
    {
        printf("%x\t", two_re_share[i]);
    }
    cout << endl;
    for (size_t i = 0; i < two_ot_size; i++)
    {
        printf("%x\t", two_share[i]);
    }
    cout << endl;

    ot_three_offline();
    ot_three_do();
    cout << " three" << endl;
    for (size_t i = 0; i < three_ot_size; i++)
    {
        printf("%u\t", random_num_re[i]);
    }
    cout << endl;
    for (size_t i = 0; i < three_ot_size; i++)
    {
        printf("%u\t", random_num[i]);
    }
    cout << endl;

    cout << "test" << endl;

    return 0;
}