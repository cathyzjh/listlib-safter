#pragma once
// Host-only Arduino String adapter, with deterministic String allocation failures.
#include <string>
#include <sstream>
#include <iomanip>
#include <utility>
#include <cstdlib>
namespace TestMemory { extern int stringCountdown; inline bool stringFail(){if(stringCountdown<0)return false;if(stringCountdown==0)return true;--stringCountdown;return false;} }
class String {
    std::string s;
public:
    String(){}
    String(const char* v){if(!*v || !TestMemory::stringFail())s=v;}
    String(std::string v){if(v.empty() || !TestMemory::stringFail())s=std::move(v);}
    String(const String& v){if(v.s.empty() || !TestMemory::stringFail())s=v.s;}
    String(String&& v) noexcept:s(std::move(v.s)){}
    String& operator=(String&& v) noexcept{s=std::move(v.s);return *this;}
    String& operator=(const String& v){if(this!=&v){if(v.s.empty() || v.s.size()<=s.capacity() || !TestMemory::stringFail())s=v.s;else s.clear();}return *this;}
    String& operator=(const char* v){s=v;return *this;}
    String(double v,int digits){std::ostringstream o;o<<std::fixed<<std::setprecision(digits)<<v;if(!TestMemory::stringFail())s=o.str();}
    bool reserve(unsigned int n){if(TestMemory::stringFail())return false;s.reserve(n);return true;}
    unsigned int length()const{return (unsigned int)s.size();}
    const char* c_str()const{return s.c_str();}
    int indexOf(const String& sub,unsigned int start=0)const{auto p=s.find(sub.s,start);return p==std::string::npos?-1:(int)p;}
    String substring(unsigned int a,unsigned int b)const{if(a>b)std::swap(a,b);a=std::min(a,length());b=std::min(b,length());return String(s.substr(a,b-a));}
    String substring(unsigned int a)const{return substring(a,length());}
    void trim(){auto a=s.find_first_not_of(" \n\r\t");if(a==std::string::npos)s.clear();else s=s.substr(a,s.find_last_not_of(" \n\r\t")-a+1);}
    bool operator==(const String& v)const{return s==v.s;}
    bool operator<(const String& v)const{return s<v.s;}
    String& operator+=(const String& v){s+=v.s;return *this;}
};
