#include"encoder.h"

extern "C" void gf_vect_dot_prod_avx(int len, int vec, unsigned char *g_tbls, unsigned char **buffs, unsigned char*dests);
extern "C" void gf_2vect_dot_prod_avx(int len, int vec, unsigned char *g_tbls, unsigned char **buffs, unsigned char**dests);
extern "C" void gf_3vect_dot_prod_avx(int len, int vec, unsigned char *g_tbls, unsigned char **buffs, unsigned char**dests);
extern "C" void gf_4vect_dot_prod_avx(int len, int vec, unsigned char *g_tbls, unsigned char **buffs, unsigned char**dests);
extern "C" void gf_5vect_dot_prod_avx(int len, int vec, unsigned char *g_tbls, unsigned char **buffs, unsigned char**dests);
extern "C" void gf_6vect_dot_prod_avx(int len, int vec, unsigned char *g_tbls, unsigned char **buffs, unsigned char**dests);
extern "C" int xor_gen_avx(int vects, int len, void **array);


void encode_rs1(int k, int r, unsigned char** block_ptr, int block_size)
{
    /*for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < r; j++)
        {
            for (int l = block_size; l < block_size; l++)
            {

                global_ptrs[j][l] ^= data_ptrs[i][l];
            }
        }
    }*/
    xor_gen_avx(k + r, block_size, (void **)block_ptr);
}


void encode_rs3(int k, int r, unsigned char **data_ptrs, unsigned char **global_ptrs, int block_size)
{
    int m = k + r;
    unsigned char *encode_matrix = new unsigned char[m * k];
    gf_gen_rs_matrix(encode_matrix, m, k);
    unsigned char *g_tbls = new unsigned char[k * r * 32];
    ec_init_tables(k, r, &encode_matrix[k * k], g_tbls);
    ec_encode_data_avx(block_size, k, r, g_tbls, data_ptrs, global_ptrs);

}

void
ec_encode_data_avx(int len, int k, int rows, unsigned char *g_tbls, unsigned char **data,
                    unsigned char **coding)
{

        if (len < 32) {
                ec_encode_data_avx(len, k, rows, g_tbls, data, coding);
                return;
        }

        while (rows >= 6) {
                gf_6vect_dot_prod_avx(len, k, g_tbls, data, coding);
                g_tbls += 6 * k * 32;
                coding += 6;
                rows -= 6;
        }
        switch (rows) {
        case 5:
                gf_5vect_dot_prod_avx(len, k, g_tbls, data, coding);
                break;
        case 4:
                gf_4vect_dot_prod_avx(len, k, g_tbls, data, coding);
                break;
        case 3:
                gf_3vect_dot_prod_avx(len, k, g_tbls, data, coding);
                break;
        case 2:
                gf_2vect_dot_prod_avx(len, k, g_tbls, data, coding);
                break;
        case 1:
                gf_vect_dot_prod_avx(len, k, g_tbls, data, *coding);
                break;
        case 0:
                break;
        }
}

void
ec_init_tables(int k, int rows, unsigned char *a, unsigned char *g_tbls)
{
        int i, j;

        for (i = 0; i < rows; i++) {
                for (j = 0; j < k; j++) {
                        gf_vect_mul_init(*a++, g_tbls);
                        g_tbls += 32;
                }
        }
}

void
gf_vect_mul_init(unsigned char c, unsigned char *tbl)
{
        unsigned char c2 = (c << 1) ^ ((c & 0x80) ? 0x1d : 0);   // Mult by GF{2}
        unsigned char c4 = (c2 << 1) ^ ((c2 & 0x80) ? 0x1d : 0); // Mult by GF{2}
        unsigned char c8 = (c4 << 1) ^ ((c4 & 0x80) ? 0x1d : 0); // Mult by GF{2}

#if (__WORDSIZE == 64 || _WIN64 || __x86_64__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
        unsigned long long v1, v2, v4, v8, *t;
        unsigned long long v10, v20, v40, v80;
        unsigned char c17, c18, c20, c24;

        t = (unsigned long long *) tbl;

        v1 = c * 0x0100010001000100ull;
        v2 = c2 * 0x0101000001010000ull;
        v4 = c4 * 0x0101010100000000ull;
        v8 = c8 * 0x0101010101010101ull;

        v4 = v1 ^ v2 ^ v4;
        t[0] = v4;
        t[1] = v8 ^ v4;

        c17 = (c8 << 1) ^ ((c8 & 0x80) ? 0x1d : 0);   // Mult by GF{2}
        c18 = (c17 << 1) ^ ((c17 & 0x80) ? 0x1d : 0); // Mult by GF{2}
        c20 = (c18 << 1) ^ ((c18 & 0x80) ? 0x1d : 0); // Mult by GF{2}
        c24 = (c20 << 1) ^ ((c20 & 0x80) ? 0x1d : 0); // Mult by GF{2}

        v10 = c17 * 0x0100010001000100ull;
        v20 = c18 * 0x0101000001010000ull;
        v40 = c20 * 0x0101010100000000ull;
        v80 = c24 * 0x0101010101010101ull;

        v40 = v10 ^ v20 ^ v40;
        t[2] = v40;
        t[3] = v80 ^ v40;

#else // 32-bit or other
        unsigned char c3, c5, c6, c7, c9, c10, c11, c12, c13, c14, c15;
        unsigned char c17, c18, c19, c20, c21, c22, c23, c24, c25, c26, c27, c28, c29, c30, c31;

        c3 = c2 ^ c;
        c5 = c4 ^ c;
        c6 = c4 ^ c2;
        c7 = c4 ^ c3;

        c9 = c8 ^ c;
        c10 = c8 ^ c2;
        c11 = c8 ^ c3;
        c12 = c8 ^ c4;
        c13 = c8 ^ c5;
        c14 = c8 ^ c6;
        c15 = c8 ^ c7;

        tbl[0] = 0;
        tbl[1] = c;
        tbl[2] = c2;
        tbl[3] = c3;
        tbl[4] = c4;
        tbl[5] = c5;
        tbl[6] = c6;
        tbl[7] = c7;
        tbl[8] = c8;
        tbl[9] = c9;
        tbl[10] = c10;
        tbl[11] = c11;
        tbl[12] = c12;
        tbl[13] = c13;
        tbl[14] = c14;
        tbl[15] = c15;

        c17 = (c8 << 1) ^ ((c8 & 0x80) ? 0x1d : 0);   // Mult by GF{2}
        c18 = (c17 << 1) ^ ((c17 & 0x80) ? 0x1d : 0); // Mult by GF{2}
        c19 = c18 ^ c17;
        c20 = (c18 << 1) ^ ((c18 & 0x80) ? 0x1d : 0); // Mult by GF{2}
        c21 = c20 ^ c17;
        c22 = c20 ^ c18;
        c23 = c20 ^ c19;
        c24 = (c20 << 1) ^ ((c20 & 0x80) ? 0x1d : 0); // Mult by GF{2}
        c25 = c24 ^ c17;
        c26 = c24 ^ c18;
        c27 = c24 ^ c19;
        c28 = c24 ^ c20;
        c29 = c24 ^ c21;
        c30 = c24 ^ c22;
        c31 = c24 ^ c23;

        tbl[16] = 0;
        tbl[17] = c17;
        tbl[18] = c18;
        tbl[19] = c19;
        tbl[20] = c20;
        tbl[21] = c21;
        tbl[22] = c22;
        tbl[23] = c23;
        tbl[24] = c24;
        tbl[25] = c25;
        tbl[26] = c26;
        tbl[27] = c27;
        tbl[28] = c28;
        tbl[29] = c29;
        tbl[30] = c30;
        tbl[31] = c31;

#endif //__WORDSIZE == 64 || _WIN64 || __x86_64__
}

void
gf_gen_rs_matrix(unsigned char *a, int m, int k)
{
        int i, j;
        unsigned char p, gen = 1;

        memset(a, 0, k * m);
        for (i = 0; i < k; i++)
                a[k * i + i] = 1;

        for (i = k; i < m; i++) {
                p = 1;
                for (j = 0; j < k; j++) {
                        a[k * i + j] = p;
                        p = gf_mul(p, gen);
                }
                gen = gf_mul(gen, 2);
        }
}

unsigned char
gf_mul(unsigned char a, unsigned char b)
{
#ifndef GF_LARGE_TABLES
        int i;

        if ((a == 0) || (b == 0))
                return 0;

        return gff_base[(i = gflog_base[a] + gflog_base[b]) > 254 ? i - 255 : i];
#else
        return gf_mul_table_base[b * 256 + a];
#endif
}