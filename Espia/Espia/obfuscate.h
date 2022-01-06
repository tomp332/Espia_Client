#define KEY 0x55

#define STR(str) \
    []() -> char* { \
        constexpr auto size = sizeof(str)/sizeof(str[0]); \
        constexpr auto obfuscated_str = obfuscator<size>(str); \
        static char original_string[size]; \
        obfuscated_str.deobfoscate((unsigned char *)original_string); \
        return original_string; \
    }()

template <unsigned int N>
struct obfuscator {

    char m_data[N] = { 0 };
    constexpr obfuscator(const char* data) {
        for (unsigned int i = 0; i < N; i++) {
            m_data[i] = data[i] ^ KEY;
        }
    }
    void deobfoscate(unsigned char* des) const {
        int i = 0;
        do {
            des[i] = m_data[i] ^ KEY;
            i++;
        } while (des[i - 1]);
    }

};