//input 4 bit, key 6 bit
short int func(short int input, short int key){
    short int a, b, c, d;
    a = (input >> 3)&1;
    b = (input)&1;
    short int output = (b << 5) | (input << 1) | (a); //expansion p-box
    output = output ^ key; //exor
    //compression s-box
    short int bits[6];
    bits[0] = (output >> 0) & 1;
    bits[1] = (output >> 1) & 1;
    bits[2] = (output >> 2) & 1;
    bits[3] = (output >> 3) & 1;
    bits[4] = (output >> 4) & 1;
    bits[5] = (output >> 5) & 1;
    a = bits[0] ^ bits[1] ^ (bits[2] & bits[3]);
    b = bits[1] ^ bits[4] ^ (bits[0] & bits[5]);
    c = bits[2] ^ bits[3] ^ (bits[1] | bits[4]);
    d = (bits[3] & bits[5]) ^ (bits[0] ^ bits[2]);
    // straight s-box
    bits[0] = a;
    bits[1] = b;
    bits[2] = c;
    bits[3] = d;
    a = bits[2];
    b = bits[1];
    c = bits[3];
    d = bits[0];
    output = (a << 3) | (b << 2) | (c << 1) | d;
    return output;
}