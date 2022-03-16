#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <gmp.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>

#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

#define OPTIONS "i:o:n:vh"

void help_message(void);

int main(int argc, char **argv) {
    char *pb_file = "rsa.pub";
    FILE *infile = stdin;
    FILE *outfile = stdout;
    bool verbose = false;
    char *inputfile = NULL;
    char *outputfile = NULL;

    int opt = 0;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            inputfile = optarg;
            infile = fopen(inputfile, "rb");
            if (infile == NULL) {
                fprintf(stderr, "open input file error");
                exit(1);
            }
            break;
        case 'o':
            outputfile = optarg;
            outfile = fopen(outputfile, "wb");
            if (outfile == NULL) {
                fprintf(stderr, "open output file error");
                exit(1);
            }
            break;
        case 'n': pb_file = optarg; break;
        case 'v': verbose = true; break;
        case 'h': help_message(); break;
        default: help_message(); break;
        }
    }
    FILE *pbfile = fopen(pb_file, "rb");
    if (pbfile == NULL) {
        fprintf(stderr, "open public key file error");
        exit(2);
    }
    mpz_t n, e, s;
    mpz_inits(n, e, s, NULL);
    char username[UCHAR_MAX];
    rsa_read_pub(n, e, s, username, pbfile);
    if (verbose) {
        printf("user = %s\n", username);
        gmp_printf("s (%d bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%d bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
    }
    // verify signature
    mpz_t sign_m;
    mpz_init(sign_m);
    mpz_set_str(sign_m, username, 62);
    if (!rsa_verify(sign_m, s, e, n)) {
        fprintf(stderr, "can not verify signature\n");
        exit(3);
    }
    // encrypt input file
    rsa_encrypt_file(infile, outfile, n, e);

    mpz_clears(sign_m, s, e, n, NULL);
    fclose(pbfile);
    fclose(infile);
    fclose(outfile);

    return 0;
}

void help_message(void) {
    puts("SYNOPSIS");
    puts("   Encrypts data using RSA encryption.");
    puts("   Encrypted data is decrypted by the decrypt program.\n");
    puts("USAGE");
    puts("   ./encrypt [-hv] [-i infile] [-o outfile] -n pubkey -d privkey\n");
    puts("OPTIONS");
    puts("   -h              Display program help and usage.");
    puts("   -v              Display verbose program output.");
    puts("   -i infile       Input file of data to encrypt (default: stdin)");
    puts("   -o outfile      Output file for encrypted data (default: stdout).");
    puts("   -n pbfile       Public key file (default: rsa.pub).");
    exit(0);
    return;
}
