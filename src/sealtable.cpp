/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-11 20:19:47
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-23 08:29:57
 * @Description: 
 */
#include "HFB/sealtable.h"
#include "HFB/err.h"
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>

void SealTable::init(unsigned int features, unsigned int size, unsigned int branch, unsigned int items)
{

    m_encrypto_table_branch = branch;
    m_encrypto_table_items = items;
    m_encrypto_table_log_items = (unsigned int)(log2f32((float)(m_encrypto_table_items)));
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
    m_encrypto_table = new Ciphertext *[m_encrypto_table_branch];
    for (unsigned int i = 0; i < m_encrypto_table_branch; i++)
    {
        m_encrypto_table[i] = new Ciphertext[m_encrypto_table_items * m_encrypto_features];
    }
}

SealTable::SealTable(unsigned int features, unsigned int size, unsigned int branch, unsigned int items)
{
    init(features, size, branch, items);
}

SealTable::SealTable(unsigned int features, unsigned int size)
{
    init(features, size);
}

SealTable::SealTable(StrategyTable &strategy_table)
{
    init(strategy_table.get_features(), strategy_table.size(), strategy_table.get_branch(), strategy_table.get_items());
}

SealTable::~SealTable()
{

    if (m_encrypto_table != NULL)
    {
        for (unsigned int i = 0; i < m_encrypto_table_branch; i++)
        {
            delete[] m_encrypto_table[i];
            m_encrypto_table[i] = NULL;
        }
        delete[] m_encrypto_table;
    }

    m_encrypto_table = NULL;
    m_encrypto_table_branch = 0;
    m_encrypto_table_items = 0;
    m_encrypto_features = 0;
    m_encrypto_table_size = 0;
}

void SealTable::gen_keypair()
{
    KeyGenerator keygen(m_context);
    m_public_key = keygen.public_key();
    m_secret_key = keygen.secret_key();
}

int SealTable::save_public_key(string file_name)
{
    int ret;
    ofstream outfile;
    outfile.open(file_name, ios::out);
    if (!outfile)
    {
        return OPEN_FILE_ERROR;
    }
    try
    {
        m_public_key.save(outfile);
    }
    catch (const std::exception &e)
    {
        ret = SAVE_PUBLIC_KEY_ERROR;
    }
    outfile.close();
    return ret;
}
int SealTable::save_secret_key(string file_name)
{
    int ret;
    ofstream outfile;
    outfile.open(file_name, ios::out);
    if (!outfile)
    {
        return OPEN_FILE_ERROR;
    }
    try
    {
        m_secret_key.save(outfile);
    }
    catch (const std::exception &e)
    {
        ret = SAVE_SECRET_KEY_ERROR;
    }
    outfile.close();
    return ret;
}

int SealTable::load_public_key(string file_name)
{
    ifstream pkinfile;
    int ret = SUCCESSFUL;
    pkinfile.open(file_name, ios::in);
    if (!pkinfile)
    {
        return OPEN_FILE_ERROR;
    }

    try
    {
        m_public_key.load(m_context, pkinfile);
    }
    catch (const std::exception &e)
    {
        ret = LOAD_PUBLIC_KEY_ERROR;
    }

    pkinfile.close();

    return ret;
}

int SealTable::load_secret_key(string file_name)
{
    ifstream seinfile;
    int ret = SUCCESSFUL;
    seinfile.open(file_name, ios::in);
    if (!seinfile)
    {
        return OPEN_FILE_ERROR;
    }
    try
    {
        m_secret_key.load(m_context, seinfile);
    }
    catch (const std::exception &e)
    {
        ret = LOAD_SECRET_KEY_ERROR;
    }

    seinfile.close();
    return ret;
}

int SealTable::encrypto_table(StrategyTable &strategy_table)
{
    int ret = SUCCESSFUL;
    Encryptor encryptor(m_context, m_public_key);
    Plaintext plain(m_encrypto_table_size);
    const unsigned char ***table = strategy_table.get_table(); // table data
    try
    {
        for (unsigned int i = 0; i < m_encrypto_table_branch; i++)
        {
            for (unsigned int j = 0; j < m_encrypto_table_items * m_encrypto_features; j++)
            {
                for (unsigned int k = 0; k < m_encrypto_table_size; k++)
                {
                    plain[k] = table[i][k][j];
                }
                encryptor.encrypt(plain, m_encrypto_table[i][j]);
                plain.resize(m_encrypto_table_size); //clean the plain text
            }
        }
    }
    catch (const std::exception &e)
    {
        ret = ENCRYPTO_TABLE_FAILED;
    }

    return ret;
}

ofstream SealTable::encrypto_table_to_stream(){
    ofstream outfile;

    try
    {
        for (unsigned int i = 0; i < m_encrypto_table_branch; i++)
        {
            for (unsigned int j = 0; j < m_encrypto_table_items * m_encrypto_features; j++)
            {
                m_encrypto_table[i][j].save(outfile);
            }
        }
    }
    catch (const std::exception &e)
    {
        cout << "SAVE_ENCRYPTO_TABLE_FAILED" << endl;
//        ret = SAVE_ENCRYPTO_TABLE_FAILED;
    }
    return outfile;
}

void SealTable::stream_to_encrypto_table(ifstream &in, SealTable &t){
    try
    {

        for (unsigned int i = 0; i < m_encrypto_table_branch; i++)
        {
            for (unsigned int j = 0; j < m_encrypto_table_items * m_encrypto_features; j++)
            {
                m_encrypto_table[i][j].load(m_context, in);
            }
        }
    }
    catch (const std::exception &e)
    {
        cout << "LOAD_ENCRYPTO_TABLE_FAILED" << endl;
    }
}

int SealTable::save_encrypto_table(string save_file_name)
{
    int ret;
    ofstream outfile;
    outfile.open(save_file_name, ios::out);
    if (!outfile)
    {
        return OPEN_FILE_ERROR;
    }
    try
    {
        for (unsigned int i = 0; i < m_encrypto_table_branch; i++)
        {
            for (unsigned int j = 0; j < m_encrypto_table_items * m_encrypto_features; j++)
            {
                m_encrypto_table[i][j].save(outfile);
            }
        }
    }
    catch (const std::exception &e)
    {
        ret = SAVE_ENCRYPTO_TABLE_FAILED;
    }

    outfile.close();
    return ret;
}

int SealTable::load_encrpto_table(string file_name)
{
    int ret = SUCCESSFUL;

    ifstream in;

    in.open(file_name, ios::in);

    if (!in)
    {
        return OPEN_FILE_ERROR;
    }

    try
    {

        for (unsigned int i = 0; i < m_encrypto_table_branch; i++)
        {
            for (unsigned int j = 0; j < m_encrypto_table_items * m_encrypto_features; j++)
            {
                m_encrypto_table[i][j].load(m_context, in);
            }
        }
    }
    catch (const std::exception &e)
    {
        ret = LOAD_ENCRYPTO_TABLE_FAILED;
    }

    in.close();

    return ret;
}

int SealTable::select(unsigned int index, int value)
{
    unsigned int tmp = int_trans_uint(value);
    int ret = select_uint(index, tmp);
    return ret;
}

int SealTable::select(unsigned int index, float value)
{
    unsigned int tmp = float_trans_uint(value);
    int ret = select_uint(index, tmp);
    return ret;
}

int SealTable::select_uint(unsigned int index, unsigned int value)
{
    int ret = SUCCESSFUL;
    Evaluator evaluator(m_context);
    unsigned int shift_r = (m_encrypto_table_branch - 1) * m_encrypto_table_log_items;
    if (index >= m_encrypto_features)
    {
        ret = PACKING_VALUE_ERROR;
    }
    try
    {
        for (unsigned int i = 0; i < m_encrypto_table_branch; i++)
        {
            /*branch the data and get each item vaule to create a vector */
            unsigned char item = (unsigned char)((value << (i * m_encrypto_table_log_items)) >> shift_r);
            /* packing the vaule just use the add function */
            evaluator.add_inplace(m_packing_value, m_encrypto_table[i][item + index * m_encrypto_table_items]);
        }
    }
    catch (const std::exception &e)
    {
        ret = PACKING_VALUE_ERROR;
    }

    return ret;
}

int SealTable::save_packing_value(string file_name)
{
    int ret;
    ofstream outfile;
    outfile.open(file_name, ios::out);
    if (!outfile)
    {
        return OPEN_FILE_ERROR;
    }

    try
    {
        m_packing_value.save(outfile); // save
    }
    catch (const std::exception &e)
    {
        ret = SAVE_PACKING_VALUE_FAILED;
    }

    outfile.close();
    return ret;
}

void SealTable::clean_packing_value()
{
    for (unsigned int j = 0; j < m_encrypto_table_branch; j++)
    {
        m_packing_value.resize(m_packing_value.size());
    }
}

int SealTable::compute_share(unsigned char share[], unsigned int &len)
{
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

int SealTable::add_random_noise()
{
    int ret = SUCCESSFUL;
    try
    {
        Ciphertext tmp(m_context);
        Encryptor encryptor(m_context, m_public_key);
        encryptor.encrypt_zero(tmp);
        Evaluator evaluator(m_context);
        evaluator.add_inplace(m_packing_value, tmp);
    }
    catch (const std::exception &e)
    {
        ret = ADD_RANDOM_NOISE_FAILED;
    }

    return ret;
}

int SealTable::load_packing_value(string file_name)
{
    int ret;
    ifstream infile;
    infile.open(file_name, ios::in);
    if (!infile)
    {
        return OPEN_FILE_ERROR;
    }

    try
    {
        m_packing_value.load(m_context, infile);
    }
    catch (const std::exception &e)
    {
        ret = LOAD_PACKING_VALUE_FAILED;
    }

    infile.close();
    return ret;
}

int SealTable::decrypto_packing_value(unsigned char result[], unsigned int &len)
{
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

int SealTable::save_packing_value_string(unsigned char *buf, unsigned int &len)
{
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

int SealTable::load_packing_value_string(unsigned char *buf, unsigned int len)
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

void SealTable::release_packing_value()
{
    m_packing_value.release();
}