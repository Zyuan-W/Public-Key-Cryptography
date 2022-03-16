#include "numtheory.h"
#include "randstate.h"
#include <time.h>

// compute gcd of (a,b)
void gcd(mpz_t d, mpz_t a, mpz_t b) {
    mpz_t tmp_a, tmp_b, tmp;
    mpz_inits(tmp_a, tmp_b, tmp, NULL);
    mpz_set(tmp_a, a);
    mpz_set(tmp_b, b);
    while (mpz_cmp_ui(tmp_b, 0) != 0) { // while b != 0
        mpz_set(tmp, tmp_b); // t <- b
        mpz_mod(tmp_b, tmp_a, tmp_b); // b <- a mod b
        mpz_set(tmp_a, tmp); // a <- t
    }
    mpz_set(d, tmp_a);
    mpz_clears(tmp, tmp_a, tmp_b, NULL);
    return;
}

// compute mod inverse of a mod n
void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {
    mpz_t r, r1, t, t1;
    mpz_init_set(r, n);
    mpz_init_set(r1, a);
    mpz_init_set_ui(t, 0);
    mpz_init_set_ui(t1, 1);
    mpz_t q; // quotienties
    mpz_init(q);
    mpz_t mul;
    mpz_init(mul);
    mpz_t tmp_r, tmp_t;
    mpz_inits(tmp_r, tmp_t, NULL);

    while (mpz_cmp_ui(r1, 0) != 0) {
        mpz_fdiv_q(q, r, r1); // q <- floor(r/r1)
        mpz_set(tmp_r, r);
        mpz_set(r, r1);
        // r1 = r - q * r1;
        mpz_mul(mul, q, r1);
        mpz_sub(r1, tmp_r, mul);

        mpz_set(tmp_t, t);
        mpz_set(t, t1);
        // t1 = t - q * t1;
        mpz_mul(mul, q, t1);
        mpz_sub(t1, tmp_t, mul);
    }
    if (mpz_cmp_ui(r, 1) > 0) {
        mpz_set_ui(i, 0); // no inverse
        return;
    }
    if (mpz_cmp_ui(t, 0) < 0) {
        mpz_add(t, t, n);
    }
    mpz_set(i, t);
    mpz_clears(r, r1, t, t1, q, mul, tmp_r, tmp_t, NULL);
    return;
}

// check if number is odd
bool isodd(mpz_t num) {
    mpz_t tmp;
    mpz_init(tmp);
    mpz_mod_ui(tmp, num, 2);
    if (mpz_cmp_ui(tmp, 1) == 0) {
        mpz_clear(tmp);
        return true;
    }
    mpz_clear(tmp);
    return false;
}

// compute base ^ exponent mod modulus
void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
    mpz_t v, p, e;
    mpz_init_set_ui(v, 1);
    mpz_init_set(p, base);
    mpz_init_set(e, exponent);
    //printf("magic\n");
    mpz_t mul;
    mpz_init(mul);
    while (mpz_cmp_ui(e, 0) > 0) {
        //mpz_mod_ui(mod, exponent, 2); // d mod 2
        //mpz_odd_p(e)
        if (isodd(e)) { // if d is odd
            mpz_mul(mul, v, p); // v * p
            mpz_mod(v, mul, modulus); // v <- (v*p) mod n
        }
        mpz_fdiv_q_ui(e, e, 2); // d <- floor(d/2)
        mpz_mul(mul, p, p); // p * p
        mpz_mod(p, mul, modulus); // p <- (p*p) mod n
    }
    mpz_set(out, v);

    mpz_clears(v, p, e, mul, NULL);
    return;
}

// check if number is prime
bool is_prime(mpz_t n, uint64_t iters) {
    if (mpz_cmp_ui(n, 0) == 0 || mpz_cmp_ui(n, 1) == 0) {
        return false;
    }
    if (mpz_cmp_ui(n, 3) == 0) {
        return true;
    }
    mpz_t n_1, r;
    mpz_inits(n_1, r, NULL);
    // n-1
    mpz_sub_ui(n_1, n, 1);
    //2^s
    mp_bitcnt_t s = 2;
    while (mpz_divisible_2exp_p(n_1, s)) { // n_1 divided by 2^s
        s += 1;
    }
    s -= 1;
    mpz_tdiv_q_2exp(r, n_1, s); // r = (n-1)/2^s

    mpz_t a, n_3, y;
    mpz_inits(a, n_3, y, NULL);
    mpz_t ex;
    mpz_init_set_ui(ex, 2);

    for (uint64_t i = 1; i <= iters; i += 1) {
        mpz_sub_ui(n_3, n, 3);
        mpz_urandomm(a, state, n_3); // 0 ... (n - 3) - 1
        mpz_add_ui(a, a, 2); // 2 ... n - 2
        pow_mod(y, a, r, n); // a^r mond n
        //mpz_powm(y, a, r, n);
        mpz_t tmp_y, j;
        mpz_inits(tmp_y, j, NULL);
        if ((mpz_cmp_ui(y, 1) != 0) && mpz_cmp(y, n_1) != 0) {
            while (mpz_cmp_ui(j, s - 1) <= 0 && mpz_cmp(y, n_1) != 0) {
                mpz_set(tmp_y, y);
                pow_mod(y, tmp_y, ex, n);
                //mpz_powm(y, y, ex, n);
                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clears(n_1, r, a, n_3, y, tmp_y, j, ex, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }
            if (mpz_cmp(y, n_1) != 0) {
                mpz_clears(n_1, r, a, n_3, y, tmp_y, j, ex, NULL);
                return false;
            }
        }
        mpz_clears(tmp_y, j, NULL);
    }
    mpz_clears(n_1, r, a, n_3, y, ex, NULL);
    return true;
}

// get offset for urandomb()
void get_offset(mpz_t offset, uint64_t bits) {
    mpz_t two, pow;
    mpz_inits(two, pow, NULL);
    mpz_set_ui(two, 2);
    mpz_pow_ui(pow, two, bits); // 2^n
    mpz_sub_ui(offset, pow, 1); // 2^n - 1

    mpz_clears(two, pow, NULL);
    return;
}

// Generates a new prime number stored in p (at least bis number of bits long)
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    mpz_t offset;
    mpz_init(offset);
    get_offset(offset, bits);
    mpz_urandomb(p, state, bits);
    mpz_add(p, p, offset); // p = urandomb(bits) + bits
    while (!is_prime(p, iters)) {
        mpz_urandomb(p, state, bits);
        mpz_add(p, p, offset);
    }
    mpz_clear(offset);
    return;
}
