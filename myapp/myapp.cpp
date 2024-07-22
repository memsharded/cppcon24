#include <openssl/sha.h>
#include <iostream>
#include <string>

int main(){
    unsigned char sha256_digest[SHA256_DIGEST_LENGTH];
    char sha256_string[SHA256_DIGEST_LENGTH*2+1] = {0};
    char string[] = "happy";

    SHA256((unsigned char*)&string, strlen(string), (unsigned char*)&sha256_digest);
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        snprintf(&sha256_string[i*2], sizeof(sha256_string)-i*2, "%02x", (unsigned int)sha256_digest[i]);
    }
    std::cout << "******* Hello world ************\n";
    std::cout << "sha256 digest: " << sha256_string << "\n";
}