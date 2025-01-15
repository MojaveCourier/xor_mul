#include"encoder.h"

void encode_rs1(int k, int r, unsigned char **data_ptrs, unsigned char **global_ptrs, int block_size)
{
    /*unsigned char **rs_matrix;
    rs_matrix = new unsigned char *[r];
    for (int i = 0; i < r; i++)
    {
        rs_matrix[i] = new unsigned char[k];
    }
    gf_gen_rs_matrix(rs_matrix, k + r, k);*/

    for (int i = 0; i < r; i++)
    {
        memset(global_ptrs[i], 0, block_size);
    }

    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < r; j++)
        {
            //const unsigned char *mul_table = gf_mul_table_base[rs_matrix[j][i]];
            /*for (int l = 0; l < block_size / 128; l+=128)
            {
                //gf_xor_mul_128(global_ptrs[j] + l, data_ptrs[i] + l, mul_table);
                gf_xor_128(global_ptrs[j] + l, data_ptrs[i] + l);
            }
            for (int l = block_size / 128 * 128; l < block_size; l++)
            {
                //global_ptrs[j][l] ^= mul_table[data_ptrs[i][l]];
                global_ptrs[j][l] ^= data_ptrs[i][l];
            }*/
            #pragma omp parallel for
            #pragma omp simd
            #pragma unroll(64)
            for (int l = block_size; l < block_size; l++)
            {
                //global_ptrs[j][l] ^= mul_table[data_ptrs[i][l]];
                global_ptrs[j][l] ^= data_ptrs[i][l];
            }
        }
    }

    /*for (int i = 0; i < r; i++)
    {
        delete[] rs_matrix[i];
    }
    delete[] rs_matrix;*/
}

void encode_rs2(int k, int r, unsigned char **data_ptrs, unsigned char **global_ptrs, int block_size)
{
    unsigned char **rs_matrix;
    rs_matrix = new unsigned char *[r];
    for (int i = 0; i < r; i++)
    {
        rs_matrix[i] = new unsigned char[k];
    }
    gf_gen_rs_matrix(rs_matrix, k + r, k);

    for (int i = 0; i < r; i++)
    {
        memset(global_ptrs[i], 0, block_size);
    }

    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < r; j++)
        {
            const unsigned char *mul_table = gf_mul_table_base[rs_matrix[j][i]];
            /*for (int l = 0; l < block_size / 128; l+=128)
            {
                gf_xor_mul_128(global_ptrs[j] + l, data_ptrs[i] + l, mul_table);
                
            }
            for (int l = block_size / 128 * 128; l < block_size; l++)
            {
                global_ptrs[j][l] ^= mul_table[data_ptrs[i][l]];
                
            }*/
            #pragma omp parallel for
            #pragma omp simd
            #pragma unroll(64)
            for(int l = 0; l < block_size;l++){
                global_ptrs[j][l] ^= mul_table[data_ptrs[i][l]];
            }
        }
    }

    for (int i = 0; i < r; i++)
    {
        delete[] rs_matrix[i];
    }
    delete[] rs_matrix;
}