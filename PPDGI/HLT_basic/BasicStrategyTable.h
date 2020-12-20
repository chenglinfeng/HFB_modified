//
// Created by anson on 2020/12/16.
//

#ifndef HFB_BASICSTRATEGYTABLE_H
#define HFB_BASICSTRATEGYTABLE_H

/**
 * @description:
 *     Converts signed int value to unsigned integer. it is a different from the casting into unsigned int.
 * @param[in] value The signed int value
 * @return:
 *     the converted interger value
 */
inline unsigned int int_trans_uint(int value)
{
    unsigned int tmp = value ^ 0xFFFFFFFF;
    tmp = tmp ^ 0x7FFFFFFF;
    return tmp;
}

/**
 * @description:
 * operator for strategy table
 */
enum class OPERATE : unsigned int
{
    BIGGER = 0x0,   /*bigger than >*/
    EBIGGER = 0x1,  /*no less than >=*/
    SMALLER = 0x2,  /*smaller than <*/
    ESMALLER = 0x3, /*no bigger than <=*/
    EQUAL = 0x4,    /*equal ==*/
};


class BasicStrategyTable {

protected:
    unsigned int m_block_size;
    unsigned char **m_table;
    unsigned int m_block_index;
    unsigned int m_features;

private:
    void create_vector_smaller(unsigned int value, unsigned int feature_index, bool has_equal);

public:

    inline unsigned int get_features(){
        return m_features;
    }

    inline unsigned int get_block_size()
    {
        return m_block_size;
    }

    inline const unsigned char **get_table() const
    {
        return (const unsigned char **)m_table;
    }

    inline unsigned int size() const
    {
        return m_block_index;
    }

    void init(unsigned int num_decision_nodes, unsigned int features, unsigned int block_size = 1024);

    BasicStrategyTable(unsigned int num_decision_nodes, unsigned int features);

    virtual ~BasicStrategyTable();

    virtual int insert_compare_uint(OPERATE operate, unsigned int feature_index, unsigned int value);

    int save_table(std::string file_name);


};


#endif //HFB_BASICSTRATEGYTABLE_H
