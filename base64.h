#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
// START - Altered by B. van der Drift
unsigned char *base64_decode(std::string const& s, int &decoded_length);
// END - Altered by B. van der Drift