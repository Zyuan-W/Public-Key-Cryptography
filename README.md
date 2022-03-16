# Assignment 6 Public Key Cryptography

This program implements the RSA encryption algorithm,
uses modular exponentiation and Euler functions to
generate public key and private key, and performs 
signature and verification. keygen is used to generate
p, q, n, public key and private key. encrypt is used to
encrypt the content of an unencrypted file. decrypt is
used to decrypt files that have been encypted.

# Building

Run the following code to build the 'keygen' 'encrypt' 'decrypt'program:

```
$ make
```

# Runing

To run keygen:
```
./keygen [-hv] [-b bits] -n pbfile -d pvfile
```

To run encrypt:
```
./encrypt [-hv] [-i infile] [-o outfile] -n pubkey -d privkey
```

TO run decrypt:
```
./decrypt [-hv] [-i infile] [-o outfile] -n pubkey -d privkey
```

# Help Message  
```
keygen  
	-h              Display program help and usage.  
	-v              Display verbose program output.  
	-b bits         Minimum bits needed for public key n (default: 256).  
	-c confidence   Miller-Rabin iterations for testing primes (default: 50).  
	-n pbfile       Public key file (default: rsa.pub).  
	-d pvfile       private key file (default: rsa.priv).    
	-s seed         Random seed for testing.  
  
encrypt   
	-h              Display program help and usage.  
	-v              Display verbose program output.    
	-i infile       Input file of data to encrypt (default: stdin).  
	-o outfile      Output file for encrypted data (default: stdout).  
	-n pbfile       Public key file (default: rsa.pub).  
  
decrypt  
	-h              Display program help and usage.  
	-v              Display verbose program output.
	-i infile       Input file of data to decrypt (default: stdin).
	-o outfile      Output file for decrypted data (default: stdout).  
	-d pvfile       Private key file (default: rsa.priv).   
```
