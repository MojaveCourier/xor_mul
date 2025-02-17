g++ main_test.o encoder.o \
    gf_vect_dot_prod_avx.o \
    gf_2vect_dot_prod_avx.o \
    gf_3vect_dot_prod_avx.o \
    gf_4vect_dot_prod_avx.o \
    gf_5vect_dot_prod_avx.o \
    gf_6vect_dot_prod_avx.o \
    xor_gen_avx512.o \
    -o my_program