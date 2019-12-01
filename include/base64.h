#ifndef BASE64_H
#define BASE64_H


#include <iostream>

class base64
{
    public:
        base64();
        virtual ~base64();
        int base64_encode(unsigned char *, unsigned char const* , unsigned int len);
        std::string base64_decode(std::string const& s);
        int  base64_encode1(unsigned char *dest,const unsigned char *src, int len);
        int  base64_encode2(unsigned char *dest,const unsigned char *src, int len);

        int  base64_decode2( const char *in, unsigned char *out, size_t outlen);

    protected:

    private:
};

#endif // BASE64_H
