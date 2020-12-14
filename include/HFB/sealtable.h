/*
 * @Author: xin.xing.lzj
 * @Date: 2019-06-30 12:34:41
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-23 08:29:49
 * @Description: 
 */

#pragma once
#include "HFB/strategytb.h"
#include <seal/seal.h>
#include <string>
#include <memory>

using namespace seal;

/**
 * @description: 
 *    encrypto strategy table into homomorphic encryption table
 */
class SealTable
{

private:
    PublicKey m_public_key;                  /*public key of seal library BFV schem*/
    SecretKey m_secret_key;                  /*secret key of seal library BFV schem*/
    std::shared_ptr<SEALContext> m_context;  /*the conetext of seal library BFV schem */
    Ciphertext **m_encrypto_table;           /*encrption table data */
    unsigned int m_encrypto_table_branch;    /*the branch of encrption table data*/
    unsigned int m_encrypto_table_items;     /*the size of branch of encrption table data*/
    unsigned int m_encrypto_table_size;      /*strategy table current size */
    unsigned int m_encrypto_features;        /*the number of features */
    unsigned int m_encrypto_table_log_items; /*the logarithmic of size of each branch, for example, m_encrypto_table_items is 16, so m_log_items is 4*/
    Ciphertext m_packing_value;              /*pakcing value which is selected to comptuer share. after computing share value it save the finally value as well*/


public:
    SealTable() = default;
    /**
     * @description: 
     *     The constructor that uses the BFV encryption scheme
     * @param[in] features      @see m_features
     * @param[in] size          @see m_encrypto_table_size
     * @param[in] branch        @see m_encrypto_table_branch
     * @param[in] items         @see m_encrypto_table_items
     * @return: 
     */
    SealTable(unsigned int features, unsigned int size, unsigned int branch, unsigned int items);

    /**
     * @description: 
     *     The constructor that uses the BFV encryption scheme
     * @param[in] features      @see m_features
     * @param[in] size          @see m_encrypto_table_size
     * @return: 
     */
    SealTable(unsigned int features, unsigned int size);

    /**
     * @description: 
     *     Initialize function that uses the BFV encryption scheme
     * @param[in] features      @see m_features
     * @param[in] size          @see m_encrypto_table_size
     * @param[in] branch        @see m_encrypto_table_branch default is 8
     * @param[in] items         @see m_encrypto_table_items  default is 16
     * @return: 
     */
    void init(unsigned int features, unsigned int size, unsigned int branch = 8, unsigned int items = 16);
    /**
     * @description:
     *   Using the BFV encryption scheme
     *   There are three encryption parameters that are necessary to set:
     *    - poly_modulus_degree (degree of polynomial modulus), here is 1024;
     *    - coeff_modulus ([ciphertext] coefficient modulus), here 27 bit-length; 
     *    - plain_modulus (plaintext modulus; only for the BFV scheme), here is 256.
     * 
     *@param[in] strategy_table strategy table context
     */
    SealTable(StrategyTable &strategy_table);

    /**
     * @description: 
     *      clean the momery of allocation 
     */
    ~SealTable();

    /**
     * @description: 
     *    get the context of seal
     * @return: 
     *    the context of seal
     */
    inline auto get_context()
    {
        return m_context;
    }
    /**
    * @description:
    *    get the context of encrypto table
    * @return:
    *    the context of encrypto table
    */
    inline Ciphertext** get_encrypto_table(){
        return m_encrypto_table;
    }

    /**
     * @description: 
     *    get the public key 
     * @return: 
     *    public key 
     */
    inline PublicKey get_public_key()
    {
        return m_public_key;
    }

    /**
     * @description:
     *    get the secret key 
     * @return: 
     *    secret key
     */
    inline SecretKey get_secret_key()
    {
        return m_secret_key;
    }

    /**
     * @description:
     *    set public key  
     * @param[in] pk public key  
     * @return: 
     */
    inline void set_public_key(PublicKey &pk)
    {
        m_public_key = pk;
    }

    /**
     * @description:
     *    set secret key  
     * @param[in] se secret key  
     * @return: 
     */
    inline void set_secret_key(SecretKey &se)
    {
        m_secret_key = se;
    }

    /**
     * @description: 
     *    generate key pair of BFV schem
     * @return: 
     */
    void gen_keypair();

    /**
     * @description: 
     *    save the public key into a file
     * @param[in] file_name  the name of file that saves the public key
     * @return: 
     *   1 is successful otherwise is failed
     */
    int save_public_key(string file_name);

    /**
     * @description: 
     *    save the secret key into a file
     * @param[in] file_name  the name of file that saves the secret key
     * @return: 
     *   1 is successful otherwise is failed
     */
    int save_secret_key(string file_name);

    /**
     * @description: 
     *    load the public key with a file
     * @param[in] file_name  the name of file that loads the public key
     * @return: 
     *   1 is successful otherwise is failed
     */
    int load_public_key(string file_name);

    /**
     * @description: 
     *    load the secret key with a file
     * @param[in] file_name  the name of file that loads the secret key
     * @return: 
     *   1 is successful otherwise is failed
     */
    int load_secret_key(string file_name);

    /**
     * @description: 
     *     encrypto strategy table into homomorphic encryption table
     * @param[in] strategy_table strategy table context
     * @param[in] save_file_name  the name of file that will save the homomorphic encryption table
     * @return: 
     *     1 is successful otherwise is failed
     */
    int encrypto_table(StrategyTable &strategy_table);

    ofstream encrypto_table_to_stream();
    void stream_to_encrypto_table(ifstream &in, SealTable &t);
    /**
     * @description: 
     *     save the homomorphic encryption table into a file
     * @param[in] file_name  the name of file that will save the homomorphic encryption table
     * @return: 
     *       1 is successful otherwise is failed
     */
    int save_encrypto_table(string file_name);

    /**
     * @description: 
     *     load the homomorphic encryption table with a file
     * @param[in] file_name  the name of the file that loads the homomorphic encryption table
     * @return: 
     *       1 is successful otherwise is failed
     */
    int load_encrpto_table(string file_name);

    /**
     * @description: 
     *      select the vector of homomorphic encryption table with the int value, and then packing the vector in packing value
     * @param[in] value the value which is a decision value
     * @param[in] index the index of feature 
     * @return: 
     *      1 is successful otherwise is failed
     */
    int select(unsigned int index, int value);

    /**
     * @description: 
     *      select the vector of homomorphic encryption table with the float value, and then packing the vector in packing value
     * @param[in] value the value which is a decision value
     * @param[in] index the index of feature
     * @return: 
     *      1 is successful otherwise is failed
     */
    int select(unsigned int index, float value);

    /**
     * @description: 
     *      select the vector of homomorphic encryption table with the unsigned int value, and then packing the vector in packing value
     * @param[in] value the value which is a decision value
     * @param[in] index the index of feature
     * @return: 
     *      1 is successful otherwise is failed
     */
    int select_uint(unsigned int index, unsigned int value);

    /**
     * @description: 
     *      after the selecting and packing options, here will need substruct a share random number, and save the result in packing value.
     * @param[in] share the share number array
     * @param[in] len the length of array (the same as size of node)
     * @return: 
     *      1 is successful otherwise is failed
     */
    int compute_share(unsigned char share[], unsigned int &len);

    /**
     * @description: 
     *    save the packing_valye into a file
     * @param[in] file_name  the name of file that saves the packing value
     * @return: 
     *   1 is successful otherwise is failed
     */
    int save_packing_value(string file_name);

    /**
     * @description: 
     *      clear the value in packing value array
     * @return: 
     */
    void clean_packing_value();

    /**
     * @description: 
     *    load the packing_valye with a file
     * @param[in] file_name  the name of file that saves the packing value
     * @return: 
     *   1 is successful otherwise is failed
     */
    int load_packing_value(string file_name);

    /**
     * @description: 
     *    decrypto packing value and store the value into result array
     *    before call the function, you have to set or load the secret key
     * @param[in] result save the decrypto pakcing value
     * @param[in] len the length of result array
     * @return: 
     *    1 is successful otherwise is failed
     */
    int decrypto_packing_value(unsigned char result[], unsigned int &len);

    /**
     * @description: 
     *     homomorphic add zero plaintext to increase the noise. Before you call this function,
     * you must set the public key @see load_public_key()
     * @return: 
     */
    int add_random_noise();


    int save_packing_value_string(unsigned char *buf,unsigned int &len);
    int load_packing_value_string(unsigned char *buf,unsigned int len);

    void release_packing_value();

};