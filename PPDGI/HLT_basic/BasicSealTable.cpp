//
// Created by anson on 2020/12/16.
//

#include <fstream>
#include "basicsealtable.h"
#include "basicstrategytable.h"
#include "HFB/err.h"

using namespace std;

extern int BIT_LEN;

void BasicSealTable::init(unsigned int features, unsigned int size){
    m_encrypto_features = features;
    m_encrypto_table_size = size;

    EncryptionParameters parms(scheme_type::BFV);
    size_t m_poly_modulus_degree = 1024;
    parms.set_poly_modulus_degree(m_poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::BFVDefault(m_poly_modulus_degree));
    parms.set_plain_modulus(256);
    m_context = SEALContext::Create(parms);

    m_packing_value.reserve(m_context, 2);
    m_encrypto_table = NULL;
    long long col = m_encrypto_features*pow(2,BIT_LEN);
    m_encrypto_table = new Ciphertext [col];
}

BasicSealTable::BasicSealTable(unsigned int features, unsigned int size){
    init(features,size);
}

BasicSealTable::BasicSealTable(BasicStrategyTable &strategy_table){
    init(strategy_table.get_features(),strategy_table.size());
}

BasicSealTable::~BasicSealTable(){

    if (m_encrypto_table != NULL){
        delete[] m_encrypto_table;
        m_encrypto_table = NULL;
    }
    m_encrypto_features = 0;
    m_encrypto_table_size = 0;
}

void BasicSealTable::gen_keypair(){
    KeyGenerator keygen(m_context);
    m_public_key = keygen.public_key();
    m_secret_key = keygen.secret_key();
}

int BasicSealTable::encrypto_table(BasicStrategyTable &strategy_table){
    int ret = SUCCESSFUL;
    Encryptor encryptor(m_context, m_public_key);
    Plaintext plain(m_encrypto_table_size);
    const unsigned char **table = strategy_table.get_table(); // table data
    try {
        for (long long j = 0; j < pow(2,BIT_LEN) * m_encrypto_features; j++)
        {
            for (unsigned int k = 0; k < m_encrypto_table_size; k++)
            {
                plain[k] = table[k][j];
            }
            encryptor.encrypt(plain, m_encrypto_table[j]);
            plain.resize(m_encrypto_table_size); //clean the plain text
        }
    }
    catch (const std::exception &e)
    {
        ret = ENCRYPTO_TABLE_FAILED;
    }

    return ret;

}

ofstream BasicSealTable::save_encrypto_table(string file_name){
    ofstream outfile;

    try
    {
        for (long long i = 0; i < pow(2,BIT_LEN) * m_encrypto_features; i++)
        {
            m_encrypto_table[i].save(outfile);

        }
    }
    catch (const std::exception &e)
    {
        cout << "SAVE_ENCRYPTO_TABLE_FAILED" << endl;
//        ret = SAVE_ENCRYPTO_TABLE_FAILED;
    }
    return outfile;
}


int BasicSealTable::load_encrpto_table(string file_name){
    int ret = SUCCESSFUL;

    ifstream in;

    in.open(file_name, ios::in);

    if (!in)
    {
        return OPEN_FILE_ERROR;
    }

    try
    {

        for (long long i = 0; i < pow(2,BIT_LEN) * m_encrypto_features; i++)
        {
            m_encrypto_table[i].load(m_context, in);

        }
    }
    catch (const std::exception &e)
    {
        ret = LOAD_ENCRYPTO_TABLE_FAILED;
    }

    in.close();

    return ret;
}

int BasicSealTable::select_uint(unsigned int index, unsigned int value){
    int ret = SUCCESSFUL;
    Evaluator evaluator(m_context);
    if (index >= m_encrypto_features)
    {
        ret = PACKING_VALUE_ERROR;
    }
    try
    {
        long long pos = value+index*pow(2,BIT_LEN);
        evaluator.add_inplace(m_packing_value,m_encrypto_table[pos]);

    }
    catch (const std::exception &e)
    {
        ret = PACKING_VALUE_ERROR;
    }

    return ret;
}

int BasicSealTable::compute_share(unsigned char share[], unsigned int &len){
    int ret = SUCCESSFUL;

    if (len > m_encrypto_table_size)
    {
        len = m_encrypto_table_size;
    }

    try
    {
        Plaintext tmp(len); //make share number to a plain text
        for (unsigned int i = 0; i < len; i++)
        {
            tmp[i] = share[i];
        }

        Evaluator evaluator(m_context);
        evaluator.sub_plain_inplace(m_packing_value, tmp); // substruct the share number
    }
    catch (const std::exception &e)
    {
        ret = COMPUTE_SHARE_ERRPR;
    }

    return ret;
}

int BasicSealTable::decrypto_packing_value(unsigned char result[], unsigned int &len){
    int ret = SUCCESSFUL;
    Plaintext decrypted;
    Decryptor decryptor(m_context, m_secret_key);
    try
    {
        decryptor.decrypt(m_packing_value, decrypted);
        if (len > decrypted.coeff_count())
        {
            len = decrypted.coeff_count();
        }

        for (unsigned int i = 0; i < len; i++)
        {
            result[i] = (unsigned char)decrypted[i];
        }
    }
    catch (const std::exception &e)
    {
        ret = DECRYPTO_PACKING_VALUE_FAILED;
    }
    return ret;
}

int BasicSealTable::save_packing_value_string(unsigned char *buf,unsigned int &len){
    int ret;
    stringstream ss;
    vector<char> v2;
    try
    {
        m_packing_value.save(ss); // save
        unsigned int i = 0;
        for (char ch; ss.get(ch); i++)
        {
            v2.push_back(ch);
        }
        if (i < len)
        {
            len = i;
            return BUFFER_TOO_SMALLER;
        }
        len = i;
        copy(v2.begin(), v2.end(), buf);
    }
    catch (const std::exception &e)
    {
        ret = SAVE_PACKING_VALUE_FAILED;
        cout << "save" << e.what() << endl;
    }

    return SUCCESSFUL;
}

int BasicSealTable::load_packing_value_string(unsigned char *buf, unsigned int len)
{
    int ret;

    try
    {
        stringstream ss1;
        for (unsigned int i = 0; i < len; i++)
        {
            ss1.put(buf[i]);
        }
        m_packing_value.load(m_context, ss1);
    }
    catch (const std::exception &e)
    {
        ret = LOAD_PACKING_VALUE_FAILED;
        cout << "load" << e.what() << endl;
    }

    return SUCCESSFUL;
}

void BasicSealTable::release_packing_value()
{
    m_packing_value.release();
}
