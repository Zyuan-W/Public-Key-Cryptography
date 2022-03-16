#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <gmp.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

#define OPTIONS "i:o:n:vh"

void help_message(void);

int main(int argc, char **argv) {
    char *pv_file = "rsa.priv";
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
        case 'n': pv_file = optarg; break;
        case 'v': verbose = true; break;
        case 'h': help_message(); break;
        default: help_message(); break;
        }
    }
    FILE *pvfile = fopen(pv_file, "rb");
    if (pvfile == NULL) {
        fprintf(stderr, "open private key file error");
        exit(3);
    }

    mpz_t n, d;
    mpz_inits(n, d);
    rsa_read_priv(n, d, pvfile);

    if (verbose) {
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    rsa_decrypt_file(infile, outfile, n, d);
    fclose(pvfile);
    fclose(infile);
    fclose(outfile);
    mpz_clears(n, d, NULL);
    return 0;
}

void help_message(void) {
    puts("SYNOPSIS");
    puts("   Dncrypts data using RSA encryption.");
    puts("   Encrypted data is encrypted by the encrypt program.\n");
    puts("USAGE");
    puts("   ./decrypt [-hv] [-i infile] [-o outfile] -n pubkey -d privkey\n");
    puts("OPTIONS");
    puts("   -h              Display program help and usage.");
    puts("   -v              Display verbose program output.");
    puts("   -i infile       Input file of data to encrypt (default: stdin)");
    puts("   -o outfile      Output file for encrypted data (default: stdout).");
    puts("   -n pvfile       Private key file (default: rsa.priv).");
    exit(0);
    return;
}
