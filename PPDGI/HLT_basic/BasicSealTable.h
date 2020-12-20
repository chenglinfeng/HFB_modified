//
// Created by anson on 2020/12/16.
//

#ifndef HFB_BASICSEALTABLE_H
#define HFB_BASICSEALTABLE_H

#include "basicstrategytable.h"
#include <seal/seal.h>
#include <string>
#include <memory>

using namespace std;
using namespace seal;


class BasicSealTable {

private:
    PublicKey m_public_key;                  /*public key of seal library BFV schem*/
    SecretKey m_secret_key;                  /*secret key of seal library BFV schem*/
    std::shared_ptr<SEALContext> m_context;  /*the conetext of seal library BFV schem */
    Ciphertext *m_encrypto_table;           /*encrption table data */
    unsigned int m_encrypto_table_size;      /*strategy table current size */
    unsigned int m_encrypto_features;        /*the number of features */
    Ciphertext m_packing_value;              /*pakcing value which is selected to comptuer share. after computing share value it save the finally value as well*/

public:
    void init(unsigned int features, unsigned int size);

    BasicSealTable(unsigned int features, unsigned int size);

    BasicSealTable(BasicStrategyTable &strategy_table);

    ~BasicSealTable();

    inline auto get_context()
    {
        return m_context;
    }

    inline Ciphertext* get_encrypto_table(){
        return m_encrypto_table;
    }

    void gen_keypair();

    int encrypto_table(BasicStrategyTable &strategy_table);

    ofstream save_encrypto_table(string file_name);

    int load_encrpto_table(string file_name);

    int select_uint(unsigned int index, unsigned int value);

    int compute_share(unsigned char share[], unsigned int &len);

    int decrypto_packing_value(unsigned char result[], unsigned int &len);

    int save_packing_value_string(unsigned char *buf,unsigned int &len);
    int load_packing_value_string(unsigned char *buf,unsigned int len);

    void release_packing_value();

};


#endif //HFB_BASICSEALTABLE_H
