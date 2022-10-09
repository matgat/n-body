#ifndef GUARD_base64_hpp
#define GUARD_base64_hpp
//  ---------------------------------------------
//  Facilities for base64 encoding
//  ---------------------------------------------
#include <cmath> // std::ceil
//#include <cctype> // std::isalnum



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace b64 //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
{

//----------------------------------------------------------------------------
int estimate_encoded_size(const int binary_size)
{
    return std::ceil(binary_size/3)*4;
}


//----------------------------------------------------------------------------
//[[nodiscard]] bool is_base64(const char c)
//{
//    return std::isalnum(static_cast<unsigned_char>(c)) || c=='+' || c=='/' || c=='=';
//}





//---------------------------------------------------------------------------
// Encode to base64 characters
void encode(TStream* const dec, TStream* const enc)
{
    const int base = 64;
    const char Base64Table[base] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const char chPad = '=';
    const int chunk_siz = 3; // [bytes]
    const int chunk_b64siz = 4; // [base64 symbols]
    const int restadd = chunk_b64siz/chunk_siz;

    unsigned char chunk_byte[chunk_siz];
    unsigned char chunk_b64[chunk_b64siz];
    char chunk_chars[chunk_b64siz];

    //enc->SetSize( b64::estimate_encoded_size(dec->Size) );
    int bytes_read;
    while( bytes_read=dec->Read(chunk_byte,chunk_siz) )
       {
        // Put to zero possible remaining bytes
        for(int i=bytes_read; i<chunk_siz; ++i) chunk_byte[i] = 0;
        // Encode
        //   |     byte 0    |     byte 1    |     byte 2    |
        //   |7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0|
        //   |           |           |           |           |
        //       b64 0       b64 1       b64 2       b64 3
        chunk_b64[0] = (chunk_byte[0] & 0xfc) >> 2;
        chunk_b64[1] = ((chunk_byte[0] & 0x03) << 4) + ((chunk_byte[1] & 0xf0) >> 4);
        chunk_b64[2] = ((chunk_byte[1] & 0x0f) << 2) + ((chunk_byte[2] & 0xc0) >> 6);
        chunk_b64[3] = chunk_byte[2] & 0x3f;
        // Now can encode these four 6-bit to the corresponding base64 char:
        for(int i=0; i<chunk_b64siz; ++i) chunk_chars[i] = Base64Table[chunk_b64[i]];
        // Possible padding chars
        for(int i=bytes_read+restadd; i<chunk_b64siz; ++i) chunk_chars[i] = chPad;
        // Write the encoded chars
        enc->Write(chunk_chars,chunk_b64siz);
       }
}

//---------------------------------------------------------------------------
// Decode from base64 characters
void decode(TStream* const enc, TStream* const dec)
{
    //const int base = 64;
    //const char Base64Table[base] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const char chPad = '=';
    const int chunk_siz = 3; // [bytes]
    const int chunk_b64siz = 4; // [base64 symbols]
    const int restadd = chunk_b64siz/chunk_siz;

    unsigned char chunk_byte[chunk_siz];
    unsigned char chunk_b64[chunk_b64siz];
    char chunk_chars[chunk_b64siz];

    //dec->SetSize( enc->Size );
    int chars_read, bytes_to_write;

    while( chars_read=enc->Read(chunk_chars,chunk_b64siz) )
       {
        // Put to pad char the possible remaining chars
        for(int i=chars_read; i<chunk_b64siz; ++i) chunk_chars[i] = chPad;

        // Decode the chars
        bytes_to_write = chars_read - restadd;
        for(int i=0; i<chunk_b64siz; ++i)
           {
            const char c = chunk_chars[i];
            // Determino l'indice del carattere in 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'
                 if(c>='A' && c<='Z') chunk_b64[i] = c-'A'; // 'A' corrisponde all'indice 0
            else if(c>='a' && c<='z') chunk_b64[i] = 26 + c-'a'; // 'a' è al 26-esimo posto
            else if(c>='0' && c<='9') chunk_b64[i] = 52 + c-'0'; // '0' è al 52-esimo posto
            else if(c == '+') chunk_b64[i] = 62; // '+' è al 62-esimo posto
            else if(c == '/') chunk_b64[i] = 63; // '/' è al 63-esimo posto
            else if(c==chPad) { chunk_b64[i]=0; --bytes_to_write; } // Padding char
            else throw Exception("Invalid base64 character \'" + String(c) + "\'");
           }
        //       b64 0       b64 1       b64 2        b64 3
        //   |           |           |           |           |
        //   |7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0|
        //   |     byte 0    |     byte 1    |     byte 2    |
        chunk_byte[0] = (chunk_b64[0] << 2) + ((chunk_b64[1] & 0x30) >> 4);
        chunk_byte[1] = ((chunk_b64[1] & 0xf) << 4) + ((chunk_b64[2] & 0x3c) >> 2);
        chunk_byte[2] = ((chunk_b64[2] & 0x3) << 6) + chunk_b64[3];
        // Write the decoded bytes
        dec->Write(chunk_byte,bytes_to_write);
       }
}


}//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


//---- end unit --------------------------------------------------------------
#endif
