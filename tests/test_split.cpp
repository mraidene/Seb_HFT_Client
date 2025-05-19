#include <string>
#include <iostream>


template<typename C>
void test_custom(std::string const& s, char const* d, C& ret)
{
  C output;

  std::bitset<255> delims;
  while( *d )
  {
    unsigned char code = *d++;
    delims[code] = true;
  }
  std::string::const_iterator beg;
  bool in_token = false;
  for( std::string::const_iterator it = s.begin(), end = s.end();
    it != end; ++it )
  {
    if( delims[*it] )
    {
      if( in_token )
      {
        output.push_back(typename C::value_type(beg, it));
        in_token = false;
      }
    }
    else if( !in_token )
    {
      beg = it;
      in_token = true;
    }
  }
  if( in_token )
    output.push_back(typename C::value_type(beg, s.end()));
  output.swap(ret);
}

using namespace std;

int main()
{
    string test_1 = "String 1\n String 2";
    vector<string> test_res_1;
    test_custom<vector<string>>(test_1, "\n", test_res_1);

    cout << "TEST 1\n";
    for(auto elem: test_res_1)
    {
        cout << "ELEM PRINTS: ";
        cout << elem << '\n';
    }

    string test_2 = "String 1\n String 2\n";
    vector<string> test_res_2;
    test_custom<vector<string>>(test_2, "\n", test_res_2);

    cout << "TEST 2\n";
    for(auto elem: test_res_2)
    {
        cout << "ELEM PRINTS: ";
        cout << elem << '\n';
    }
}