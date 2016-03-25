#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED

using namespace std;

class Tokenizer
{
    public:
        static const string DELIMITERS;

        Tokenizer(const string& s):
            m_string(s),
            m_offset(0),
            m_delimiters(DELIMITERS) {}

        Tokenizer(const string& s, const string& delimiters) :
            m_string(s),
            m_offset(0),
            m_delimiters(delimiters) {}

        bool NextToken()
        {
            return NextToken(m_delimiters);
        }

        bool NextToken(const string& delimiters)
        {
            size_t i = m_string.find_first_not_of(delimiters, m_offset);
            if (string::npos == i)
            {
                m_offset = m_string.length();
                return false;
            }

            size_t j = m_string.find_first_of(delimiters, i);
            if (string::npos == j)
            {
                m_token = m_string.substr(i);
                m_offset = m_string.length();
                return true;
            }

            m_token = m_string.substr(i, j - i);
            m_offset = j;
            return true;
        }

        string GetToken() {return m_token; };

        void Reset();
    protected:
        size_t m_offset;
        const string m_string;
        string m_token;
        string m_delimiters;
};

//CPP file
const string Tokenizer::DELIMITERS(" \t\n\r");


#endif // TOKENIZER_H_INCLUDED
