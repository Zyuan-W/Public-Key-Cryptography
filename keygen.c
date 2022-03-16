#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <gmp.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

#define OPTIONS "b:i:n:d:s:vh"

void help_message(void);

int main(int argc, char **argv) {
    char *pb_file = "rsa.pub";
    char *pv_file = "rsa.priv";
    uint64_t nbits = 256, iters = 50;
    uint64_t seed = (int) time(NULL);
    bool verbose = false;

    int opt = 0;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'b': nbits = atoi(optarg); break;
        case 'i': iters = atoi(optarg); break;
        case 'n': pb_file = optarg; break;
        case 'd': pv_file = optarg; break;
        case 's': seed = atoi(optarg); break;
        case 'v': verbose = true; break;
        case 'h': help_message(); break;
        default: help_message(); break;
        }
    }
    FILE *pbfile = fopen(pb_file, "wb");
    if (pbfile == NULL) {
        fprintf(stderr, "opening file error");
        exit(1);
    }

    FILE *pvfile = fopen(pv_file, "wb");
    if (pvfile == NULL) {
        fprintf(stderr, "opening file error");
        exit(1);
    }

    // permissions
    int fd_pvfile = fileno(pvfile);
    fchmod(fd_pvfile, 0600);

    randstate_init(seed);
    srandom(seed);

    mpz_t p, q, n, e, d, s, sign_m;
    mpz_inits(p, q, n, e, d, s, sign_m, NULL);
    // make p, q, n, e
    rsa_make_pub(p, q, n, e, nbits, iters);
    // make d
    rsa_make_priv(d, e, p, q);
    // get username
    char *username = getenv("USER");
    mpz_set_str(sign_m, username, 62);
    // signature
    rsa_sign(s, sign_m, d, n);

    // create pbfile
    rsa_write_pub(n, e, s, username, pbfile);
    // create pvfile
    rsa_write_priv(n, d, pvfile);

    if (verbose) {
        printf("user = %s\n", username);
        gmp_printf("s (%d bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("p (%d bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_printf("q (%d bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%d bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
        gmp_printf("d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }
    fclose(pbfile);
    fclose(pvfile);
    randstate_clear();
    mpz_clears(p, q, n, e, d, s, sign_m, NULL);
    return 0;
}

void help_message(void) {
    puts("SYNOPSIS");
    puts("   Generates an RSA public/private key pair.\n");
    puts("USAGE");
    puts("   ./keygen [-hv] [-b bits] -n pbfile -d pvfile\n");
    puts("OPTIONS");
    puts("   -h              Display program help and usage.");
    puts("   -v              Display verbose program output.");
    puts("   -b bits         Minimum bits needed for public key n (default: 256).");
    puts("   -c confidence   Miller-Rabin iterations for testing primes (default: 50).");
    puts("   -n pbfile       Public key file (default: rsa.pub).");
    puts("   -d pvfile       Private key file (default: rsa.priv).");
    puts("   -s seed         Random seed for testing.");
    exit(0);
}
