#include "rsa.h"
#include <stdlib.h>
#include "numtheory.h"
#include "randstate.h"

// generate p, q, n, public key e
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    uint64_t p_bits = (random() % (nbits / 2)) + (nbits / 4);
    uint64_t q_bits = nbits - p_bits;
    make_prime(p, p_bits, iters);
    make_prime(q, q_bits, iters);
    mpz_mul(n, p, q); // n = p * q
    mpz_t p_1, q_1, totient;
    mpz_inits(p_1, q_1, totient, NULL);
    mpz_sub_ui(p_1, p, 1);
    mpz_sub_ui(q_1, q, 1);
    mpz_mul(totient, p_1, q_1); // totient = (p-1) * (q-1)
    mpz_t g;
    mpz_init(g);
    do {
        mpz_urandomb(e, state, nbits);
        gcd(g, e, totient);
    } while (mpz_cmp_ui(g, 1) != 0);
    mpz_clears(p_1, q_1, g, totient, NULL);
    return;
}

// write public key to pbfile
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n", n);
    gmp_fprintf(pbfile, "%Zx\n", e);
    gmp_fprintf(pbfile, "%Zx\n", s);
    fprintf(pbfile, "%s\n", username);
    return;
}

// read public key from pbfile
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx", n);
    gmp_fscanf(pbfile, "%Zx", e);
    gmp_fscanf(pbfile, "%Zx", s);
    fscanf(pbfile, "%s", username);
    return;
}

// generate private key d
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    mpz_t totient, p_1, q_1;
    mpz_inits(totient, p_1, q_1, NULL);
    mpz_sub_ui(p_1, p, 1); // p-1
    mpz_sub_ui(q_1, q, 1); // q-1
    mpz_mul(totient, p_1, q_1); // totient = p-1 * q-1
    mod_inverse(d, e, totient); // d = e mod totient

    mpz_clears(totient, p_1, q_1, NULL);
    return;
}

// write private key to pvfile
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n", n);
    gmp_fprintf(pvfile, "%Zx\n", d);
    return;
}

// read private key from pvfile
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx", n);
    gmp_fscanf(pvfile, "%Zx", d);
    return;
}

// encrypt m to c
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
    return;
}

// encrypt file
void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    mpz_t m, c, k, log, log_1;
    mpz_inits(m, c, k, log, log_1, NULL);
    long exp = 0;
    mpz_get_d_2exp(&exp, n);
    mpz_set_si(log, exp);
    mpz_sub_ui(log_1, log, 1);
    mpz_fdiv_q_ui(k, log_1, 8); // floor (log2(n)-1) / 8
    uint8_t *buf = (uint8_t *) malloc(mpz_get_ui(k));
    buf[0] = 0xFF;
    uint32_t j = 0;
    while (!feof(infile)) {
        j = fread(buf + 1, 1, mpz_get_ui(k) - 1, infile);
        mpz_import(m, j + 1, 1, sizeof(buf[0]), 1, 0, buf);
        rsa_encrypt(c, m, e, n);
        gmp_fprintf(outfile, "%Zx\n", c);
        for (uint32_t i = 1; i < mpz_get_ui(k); i++) {
            buf[i] = 0;
        }
    }
    free(buf);
    buf = NULL;
    mpz_clears(m, c, k, log_1, log, NULL);
    return;
}

// decrypt c to m
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n);
    return;
}

// decrypt file
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    mpz_t m, c, k, log, log_1;
    mpz_inits(m, c, k, log, log_1, NULL);
    long exp = 0;
    mpz_get_d_2exp(&exp, n);
    mpz_set_si(log, exp);
    mpz_sub_ui(log_1, log, 1);
    mpz_fdiv_q_ui(k, log_1, 8); // floor (log2(n)-1) / 8
    uint8_t *buf = (uint8_t *) malloc(mpz_get_ui(k));
    buf[0] = 0xFF;
    size_t j = 0;
    while ((gmp_fscanf(infile, "%Zx\n", c)) != EOF) {
        rsa_decrypt(m, c, d, n);
        mpz_export(buf, &j, 1, 1, sizeof(buf[0]), 0, m);
        fwrite(buf + 1, 1, j - 1, outfile);
        for (uint32_t i = 1; i < mpz_get_ui(k); i++) {
            buf[i] = 0;
        }
    }
    free(buf);
    buf = NULL;
    mpz_clears(m, c, k, log, log_1, NULL);
    return;
}

// signature
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
    return;
}

// verify
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t;
    mpz_init(t);
    pow_mod(t, s, e, n);
    if (mpz_cmp(t, m) == 0) {
        mpz_clear(t);
        return true;
    } else {
        mpz_clear(t);
        return false;
    }
}
