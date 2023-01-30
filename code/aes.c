#include <stdio.h> // Muss noch entfernt werden
#include "tables.c"

typedef unsigned int uint32_t;

int nr = 10;
int nk = 4;

uint32_t rotWord(uint32_t word){
    return (word << 8) ^ (word >> 24);
}

uint32_t subWord(uint32_t word){
    return (s_box[word >> 24] << 24) ^ (s_box[(word >> 16) % 0x100] << 16) ^ (s_box[(word >> 8) % 0x100] << 8) ^ (s_box[word % 0x100]);
}

void keyexpansion( uint32_t* enckey, uint32_t* deckey , uint32_t* bkey){
    if(enckey == NULL && deckey == NULL){
        return;
    }

    if(enckey == NULL){
        uint32_t tempenckey[(nr+1)*4];
        enckey = tempenckey;
    }

    int nr = nk +6;
    int i = 0;
    uint32_t temp;

    for(;i < nk; i++){
        enckey[i] = bkey[i];
    }

    i = nk;

    while(i < 4*(nr+1)){
        temp = enckey[i-1];
        
        if(i % nk == 0){
            temp = subWord(rotWord(temp)) ^ rcon[(i/nk)-1];
        }
        
        else if(nk > 6 && i%nk == 4){
            temp = subWord(temp);
        }
        int a = i - nk;
        enckey[i] = enckey[i-nk] ^ temp;
        i = i + 1;
    }

    if(deckey == NULL){
        return;
    }

	for(int i = 0; i < (nr+1)*4; i += 4 ){
		int ei = (nr+1)*4 - i - 4;
		for(int j = 0; j < 4; j++ ){
			temp = enckey[ei+j];
			if( i > 0 && i+4 < (nr+1)*4 ){
				temp = (( mul2[ temp >> 24 ] ^ mul3[ temp >> 16 & 24 ] ^ s_box[ temp >> 8 & 0xff ] ^ s_box[ temp & 0xff ] ) << 24 ) ^ ( mul3[ temp >> 24 ] ^ s_box[ temp >> 16 & 0xff ] ^ s_box[ temp >> 8 & 0xff ] ^ mul2[ temp & 0xff ] << 16) ^ (( s_box[ temp >> 24 ] ^ s_box[ temp >> 16 & 0xff ] ^ mul2[ temp >> 8 & 0xff ] ^ mul3[ temp & 0xff ] ) << 8 ) ^ (( s_box[ temp >> 24 ] ^ mul2[ temp >> 16 & 0xff ] ^ mul3[ temp >> 8 & 0xff ] ^ s_box[ temp & 0xff ] ) ); 
			}
			deckey[i+j] = temp;
		}
	}
}


void encrypt(uint32_t* state, uint32_t* key){
    uint32_t t0,t1,t2,t3;
    state[0] = state[0] ^ key[0];
    state[1] = state[1] ^ key[1];
    state[2] = state[2] ^ key[2];
    state[3] = state[3] ^ key[3];

    for(int i = 1; i < nr; i++){
        t0 = ((state[0] >> 24) << 24) ^ ((state[1] >> 16 & 0xff) << 16) ^ ((state[2] >> 8 & 0xff) << 8) ^ (state[3] & 0xff);
        t1 = ((state[1] >> 24) << 24) ^ ((state[2] >> 16 & 0xff) << 16) ^ ((state[3] >> 8 & 0xff) << 8) ^ (state[0] & 0xff);
        t2 = ((state[2] >> 24) << 24) ^ ((state[3] >> 16 & 0xff) << 16) ^ ((state[0] >> 8 & 0xff) << 8) ^ (state[1] & 0xff);
        t3 = ((state[3] >> 24) << 24) ^ ((state[0] >> 16 & 0xff) << 16) ^ ((state[1] >> 8 & 0xff) << 8) ^ (state[2] & 0xff);
        
        state[0] = (( mul2[ t0 >> 24 ] ^ mul3[ t0 >> 16 & 0xff ] ^ s_box[ t0 >> 8 & 0xff ] ^ s_box[ t0 & 0xff ] ) << 24 ) ^ (( s_box[ t0 >> 24 ] ^ mul2[ t0 >> 16 & 0xff ] ^ mul3[ t0 >> 8 & 0xff ] ^ s_box[ t0 & 0xff ] ) << 16 ) ^ (( s_box[ t0 >> 24 ] ^ s_box[ t0 >> 16 & 0xff ] ^ mul2[ t0 >> 8 & 0xff ] ^ mul3[ t0 & 0xff ] ) << 8 ) ^ ( mul3[ t0 >> 24 ] ^ s_box[ t0 >> 16 & 0xff ] ^ s_box[ t0 >> 8 & 0xff ] ^ mul2[ t0 & 0xff ] );
        state[1] = (( mul2[ t1 >> 24 ] ^ mul3[ t1 >> 16 & 0xff ] ^ s_box[ t1 >> 8 & 0xff ] ^ s_box[ t1 & 0xff ] ) << 24 ) ^ (( s_box[ t1 >> 24 ] ^ mul2[ t1 >> 16 & 0xff ] ^ mul3[ t1 >> 8 & 0xff ] ^ s_box[ t1 & 0xff ] ) << 16 ) ^ (( s_box[ t1 >> 24 ] ^ s_box[ t1 >> 16 & 0xff ] ^ mul2[ t1 >> 8 & 0xff ] ^ mul3[ t1 & 0xff ] ) << 8 ) ^ ( mul3[ t1 >> 24 ] ^ s_box[ t1 >> 16 & 0xff ] ^ s_box[ t1 >> 8 & 0xff ] ^ mul2[ t1 & 0xff ] );
        state[2] = (( mul2[ t2 >> 24 ] ^ mul3[ t2 >> 16 & 0xff ] ^ s_box[ t2 >> 8 & 0xff ] ^ s_box[ t2 & 0xff ] ) << 24 ) ^ (( s_box[ t2 >> 24 ] ^ mul2[ t2 >> 16 & 0xff ] ^ mul3[ t2 >> 8 & 0xff ] ^ s_box[ t2 & 0xff ] ) << 16 ) ^ (( s_box[ t2 >> 24 ] ^ s_box[ t2 >> 16 & 0xff ] ^ mul2[ t2 >> 8 & 0xff ] ^ mul3[ t2 & 0xff ] ) << 8 ) ^ ( mul3[ t2 >> 24 ] ^ s_box[ t2 >> 16 & 0xff ] ^ s_box[ t2 >> 8 & 0xff ] ^ mul2[ t2 & 0xff ] );
        state[3] = (( mul2[ t3 >> 24 ] ^ mul3[ t3 >> 16 & 0xff ] ^ s_box[ t3 >> 8 & 0xff ] ^ s_box[ t3 & 0xff ] ) << 24 ) ^ (( s_box[ t3 >> 24 ] ^ mul2[ t3 >> 16 & 0xff ] ^ mul3[ t3 >> 8 & 0xff ] ^ s_box[ t3 & 0xff ] ) << 16 ) ^ (( s_box[ t3 >> 24 ] ^ s_box[ t3 >> 16 & 0xff ] ^ mul2[ t3 >> 8 & 0xff ] ^ mul3[ t3 & 0xff ] ) << 8 ) ^ ( mul3[ t3 >> 24 ] ^ s_box[ t3 >> 16 & 0xff ] ^ s_box[ t3 >> 8 & 0xff ] ^ mul2[ t3 & 0xff ] );

        state[0] = state[0] ^ key[i*4];
        state[1] = state[1] ^ key[i*4+1];
        state[2] = state[2] ^ key[i*4+2];
        state[3] = state[3] ^ key[i*4+3];

    }

    // last round subbytes
    t0 = (s_box[state[0] >> 24] << 24) ^ (s_box[state[0] >> 16 & 0xff] << 16) ^ (s_box[state[0] >> 8 & 0xff] << 8) ^ s_box[state[0] & 0xff];
    t1 = (s_box[state[1] >> 24] << 24) ^ (s_box[state[1] >> 16 & 0xff] << 16) ^ (s_box[state[1] >> 8 & 0xff] << 8) ^ s_box[state[1] & 0xff];
    t2 = (s_box[state[2] >> 24] << 24) ^ (s_box[state[2] >> 16 & 0xff] << 16) ^ (s_box[state[2] >> 8 & 0xff] << 8) ^ s_box[state[2] & 0xff];
    t3 = (s_box[state[3] >> 24] << 24) ^ (s_box[state[3] >> 16 & 0xff] << 16) ^ (s_box[state[3] >> 8 & 0xff] << 8) ^ s_box[state[3] & 0xff];
    
    // last round shiftrows
    state[0] = ((t0 >> 24) << 24) ^ ((t1 >> 16 & 0xff) << 16) ^ ((t2 >> 8 & 0xff) << 8) ^ (t3 & 0xff);
    state[1] = ((t1 >> 24) << 24) ^ ((t2 >> 16 & 0xff) << 16) ^ ((t3 >> 8 & 0xff) << 8) ^ (t0 & 0xff);
    state[2] = ((t2 >> 24) << 24) ^ ((t3 >> 16 & 0xff) << 16) ^ ((t0 >> 8 & 0xff) << 8) ^ (t1 & 0xff);
    state[3] = ((t3 >> 24) << 24) ^ ((t0 >> 16 & 0xff) << 16) ^ ((t1 >> 8 & 0xff) << 8) ^ (t2 & 0xff);
    
    
    state[0] = state[0] ^ key[nr*4];
    state[1] = state[1] ^ key[nr*4+1];
    state[2] = state[2] ^ key[nr*4+2];
    state[3] = state[3] ^ key[nr*4+3];
    
    return;
}


/*
void decrypt(uint32_t* state, uint32_t* key){
    uint32_t t0,t1,t2,t3;

    state[0] = state[0] ^ key[nr*4];
    state[1] = state[1] ^ key[nr*4+1];
    state[2] = state[2] ^ key[nr*4+2];
    state[3] = state[3] ^ key[nr*4+3];

    for(int n1 = 0; n1 < 4; n1++){
        printf("%x\n",state[n1]);
    }
    printf("\n");

    for(int n1 = 0; n1 < 4; n1++){
        t0 = ((state[0] >> 24) << 24) ^ ((state[1] >> 16 & 0xff) << 16) ^ ((state[2] >> 8 & 0xff) << 8) ^ (state[3] & 0xff);
        t1 = ((state[3] >> 24) << 24) ^ ((state[0] >> 16 & 0xff) << 16) ^ ((state[1] >> 8 & 0xff) << 8) ^ (state[2] & 0xff);
        t2 = ((state[2] >> 24) << 24) ^ ((state[3] >> 16 & 0xff) << 16) ^ ((state[0] >> 8 & 0xff) << 8) ^ (state[1] & 0xff);
        t3 = ((state[1] >> 24) << 24) ^ ((state[2] >> 16 & 0xff) << 16) ^ ((state[3] >> 8 & 0xff) << 8) ^ (state[0] & 0xff);

        state[0] = (( mul14[ t0 >> 24 ] ^ mul11[ t0 >> 16 & 0xff ] ^ mul13[ t0 >> 8 & 0xff ] ^ mul9[ t0 & 0xff ] ) << 24 ) ^ (( mul9[ t0 >> 24 ] ^ mul14[ t0 >> 16 & 0xff ] ^ mul11[ t0 >> 8 & 0xff ] ^ mul13[ t0 & 0xff ] ) << 16 ) ^ (( mul13[ t0 >> 24 ] ^ mul9[ t0 >> 16 & 0xff ] ^ mul14[ t0 >> 8 & 0xff ] ^ mul11[ t0 & 0xff ] ) << 8 ) ^ ( mul11[ t0 >> 24 ] ^ mul13[ t0 >> 16 & 0xff ] ^ mul9[ t0 >> 8 & 0xff ] ^ mul14[ t0 & 0xff ] );
    
        for(int n1 = 0; n1 < 4; n1++){
            printf("%x\n",state[n1]);
        }
        printf("\n");
    
    }
    printf("\n");

    state[0] = (invs_box[t0 >> 24] << 24) ^ (invs_box[t0 >> 16 & 0xff] << 16) ^ (invs_box[t0 >> 8 & 0xff] << 8) ^ invs_box[t0 & 0xff];
    state[1] = (invs_box[t1 >> 24] << 24) ^ (invs_box[t1 >> 16 & 0xff] << 16) ^ (invs_box[t1 >> 8 & 0xff] << 8) ^ invs_box[t1 & 0xff];
    state[2] = (invs_box[t2 >> 24] << 24) ^ (invs_box[t2 >> 16 & 0xff] << 16) ^ (invs_box[t2 >> 8 & 0xff] << 8) ^ invs_box[t2 & 0xff];
    state[3] = (invs_box[t3 >> 24] << 24) ^ (invs_box[t3 >> 16 & 0xff] << 16) ^ (invs_box[t3 >> 8 & 0xff] << 8) ^ invs_box[t3 & 0xff];

    
    /*
    for(int i = nr-1; i > 0; i--){
        t0 = ((state[0] >> 24) << 24) ^ ((state[1] >> 16 & 0xff) << 16) ^ ((state[2] >> 8 & 0xff) << 8) ^ (state[3] & 0xff);
        t1 = ((state[1] >> 24) << 24) ^ ((state[2] >> 16 & 0xff) << 16) ^ ((state[3] >> 8 & 0xff) << 8) ^ (state[0] & 0xff);
        t2 = ((state[2] >> 24) << 24) ^ ((state[3] >> 16 & 0xff) << 16) ^ ((state[0] >> 8 & 0xff) << 8) ^ (state[1] & 0xff);
        t3 = ((state[3] >> 24) << 24) ^ ((state[0] >> 16 & 0xff) << 16) ^ ((state[1] >> 8 & 0xff) << 8) ^ (state[2] & 0xff);
        
        state[0] = (( mul2[ t0 >> 24 ] ^ mul3[ t0 >> 16 & 0xff ] ^ s_box[ t0 >> 8 & 0xff ] ^ s_box[ t0 & 0xff ] ) << 24 ) ^ (( s_box[ t0 >> 24 ] ^ mul2[ t0 >> 16 & 0xff ] ^ mul3[ t0 >> 8 & 0xff ] ^ s_box[ t0 & 0xff ] ) << 16 ) ^ (( s_box[ t0 >> 24 ] ^ s_box[ t0 >> 16 & 0xff ] ^ mul2[ t0 >> 8 & 0xff ] ^ mul3[ t0 & 0xff ] ) << 8 ) ^ ( mul3[ t0 >> 24 ] ^ s_box[ t0 >> 16 & 0xff ] ^ s_box[ t0 >> 8 & 0xff ] ^ mul2[ t0 & 0xff ] );
        state[1] = (( mul2[ t1 >> 24 ] ^ mul3[ t1 >> 16 & 0xff ] ^ s_box[ t1 >> 8 & 0xff ] ^ s_box[ t1 & 0xff ] ) << 24 ) ^ (( s_box[ t1 >> 24 ] ^ mul2[ t1 >> 16 & 0xff ] ^ mul3[ t1 >> 8 & 0xff ] ^ s_box[ t1 & 0xff ] ) << 16 ) ^ (( s_box[ t1 >> 24 ] ^ s_box[ t1 >> 16 & 0xff ] ^ mul2[ t1 >> 8 & 0xff ] ^ mul3[ t1 & 0xff ] ) << 8 ) ^ ( mul3[ t1 >> 24 ] ^ s_box[ t1 >> 16 & 0xff ] ^ s_box[ t1 >> 8 & 0xff ] ^ mul2[ t1 & 0xff ] );
        state[2] = (( mul2[ t2 >> 24 ] ^ mul3[ t2 >> 16 & 0xff ] ^ s_box[ t2 >> 8 & 0xff ] ^ s_box[ t2 & 0xff ] ) << 24 ) ^ (( s_box[ t2 >> 24 ] ^ mul2[ t2 >> 16 & 0xff ] ^ mul3[ t2 >> 8 & 0xff ] ^ s_box[ t2 & 0xff ] ) << 16 ) ^ (( s_box[ t2 >> 24 ] ^ s_box[ t2 >> 16 & 0xff ] ^ mul2[ t2 >> 8 & 0xff ] ^ mul3[ t2 & 0xff ] ) << 8 ) ^ ( mul3[ t2 >> 24 ] ^ s_box[ t2 >> 16 & 0xff ] ^ s_box[ t2 >> 8 & 0xff ] ^ mul2[ t2 & 0xff ] );
        state[3] = (( mul2[ t3 >> 24 ] ^ mul3[ t3 >> 16 & 0xff ] ^ s_box[ t3 >> 8 & 0xff ] ^ s_box[ t3 & 0xff ] ) << 24 ) ^ (( s_box[ t3 >> 24 ] ^ mul2[ t3 >> 16 & 0xff ] ^ mul3[ t3 >> 8 & 0xff ] ^ s_box[ t3 & 0xff ] ) << 16 ) ^ (( s_box[ t3 >> 24 ] ^ s_box[ t3 >> 16 & 0xff ] ^ mul2[ t3 >> 8 & 0xff ] ^ mul3[ t3 & 0xff ] ) << 8 ) ^ ( mul3[ t3 >> 24 ] ^ s_box[ t3 >> 16 & 0xff ] ^ s_box[ t3 >> 8 & 0xff ] ^ mul2[ t3 & 0xff ] );

        state[0] = state[0] ^ key[i*4];
        state[1] = state[1] ^ key[i*4+1];
        state[2] = state[2] ^ key[i*4+2];
        state[3] = state[3] ^ key[i*4+3];

    }

    // last round subbytes
    t0 = (s_box[state[0] >> 24] << 24) ^ (s_box[state[0] >> 16 & 0xff] << 16) ^ (s_box[state[0] >> 8 & 0xff] << 8) ^ s_box[state[0] & 0xff];
    t1 = (s_box[state[1] >> 24] << 24) ^ (s_box[state[1] >> 16 & 0xff] << 16) ^ (s_box[state[1] >> 8 & 0xff] << 8) ^ s_box[state[1] & 0xff];
    t2 = (s_box[state[2] >> 24] << 24) ^ (s_box[state[2] >> 16 & 0xff] << 16) ^ (s_box[state[2] >> 8 & 0xff] << 8) ^ s_box[state[2] & 0xff];
    t3 = (s_box[state[3] >> 24] << 24) ^ (s_box[state[3] >> 16 & 0xff] << 16) ^ (s_box[state[3] >> 8 & 0xff] << 8) ^ s_box[state[3] & 0xff];
    
    // last round shiftrows

    
    state[0] = state[0] ^ key[0];
    state[1] = state[1] ^ key[1];
    state[2] = state[2] ^ key[2];
    state[3] = state[3] ^ key[3];
    return;
}
*/

void printstate( uint32_t* state ){
    for( int x = 0; x < 4; x++){
        printf("%x\n",state[x] );
    }
    printf("\n");
    return;
}

void main(){
   
    uint32_t basekey[] = { 0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c };  
    uint32_t state[] = { 0x3243f6a8, 0x885a308d, 0x313198a2, 0xe0370734};
    uint32_t out[] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000};      
    uint32_t enckey[(nr+1)*4];
    uint32_t deckey[(nr+1)*4];

    // Ist sicher korrekt https://cs.opensource.google/go/go/+/master:src/crypto/aes/aes_test.go
    keyexpansion(enckey, deckey, basekey);

    
    printstate(state);

    encrypt(state,enckey);

    printstate(state);

    //decrypt(state,expandedkey);
}


/*
            nk  nb  nr
AES-128     4   4   10
AES-192     6   4   12
AES-256     8   4   14
*/
