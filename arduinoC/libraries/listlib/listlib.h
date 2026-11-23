/* MIT; original API by doubao/donny. Bounded, checked storage for K10. */
#ifndef LISTLIB_H
#define LISTLIB_H
#include <Arduino.h>
#include <new>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cerrno>
#include <climits>

class ListLib {
public:
    enum Error { OK=0, NOT_FOUND=1, TYPE_MISMATCH=2, BAD_INDEX=3,
                 LIMIT_EXCEEDED=4, OUT_OF_MEMORY=5, INVALID_INPUT=6,
                 KEY_NOT_FOUND=7, EMPTY_LIST=8 };
    enum { MAX_CONTAINERS=16, MAX_ITEMS=256, MAX_NAME_BYTES=64,
           MAX_TEXT_BYTES=256, MAX_INPUT_BYTES=4096, MAX_OUTPUT_BYTES=4096,
           MAX_PAYLOAD_BYTES=32768 };
private:
    struct Entry { String key; double value; Entry():value(0){} };
    struct Obj {
        String name;
        int type;
        size_t len, cap;
        double *n;
        String *s;
        Entry *d;
        Obj *next;
        explicit Obj(int t=0):type(t),len(0),cap(0),n(NULL),s(NULL),d(NULL),next(NULL){}
        ~Obj(){ release(); }
        void release(){delete[] n; delete[] s; delete[] d; n=NULL;s=NULL;d=NULL;len=cap=0;}
        size_t bytes() const {
            size_t result=0;
            for(size_t i=0;i<len;++i)
                result += type==0 ? sizeof(double) : type==1 ? sizeof(String)+s[i].length()+1 : sizeof(Entry)+d[i].key.length()+1;
            return result;
        }
        Obj(const Obj&)=delete;
        Obj& operator=(const Obj&)=delete;
    };
    struct State {
        Obj *head; size_t count; Error error;
        State():head(NULL),count(0),error(OK){}
        ~State(){while(head){Obj *p=head;head=head->next;delete p;}}
    };
    static State& state(){static State v;return v;}
    static void begin(){state().error=OK;}
    static bool fail(Error e){state().error=e;return false;}
    static bool validName(const String& name){
        return (name.length()>0 && name.length()<=MAX_NAME_BYTES) || fail(INVALID_INPUT);
    }
    static bool finite(double value){return std::isfinite(value) || fail(INVALID_INPUT);}
    static bool copyText(const String& src,String& dst){
        if(!src.length()){dst="";return true;}
        if(!dst.reserve(src.length()))return fail(OUT_OF_MEMORY);
        dst=src;
        return dst.length()==src.length() || fail(OUT_OF_MEMORY);
    }
    static Obj* lookup(const String& name,bool dict=false){
        for(Obj *p=state().head;p;p=p->next)
            if((p->type==2)==dict && p->name==name)return p;
        return NULL;
    }
    static Obj* typed(const String& name,int type){
        if(!validName(name))return NULL;
        Obj *p=lookup(name,type==2);
        if(!p){fail(NOT_FOUND);return NULL;}
        if(p->type!=type){fail(TYPE_MISMATCH);return NULL;}
        return p;
    }
    static size_t payload(){size_t n=0;for(Obj *p=state().head;p;p=p->next)n+=p->bytes();return n;}
    static bool budget(size_t oldBytes,size_t newBytes){
        return payload()-oldBytes+newBytes<=MAX_PAYLOAD_BYTES || fail(LIMIT_EXCEEDED);
    }
    // All array allocation is nothrow; old contents survive any failed growth.
    static bool reserve(Obj& o,size_t wanted){
        if(wanted>MAX_ITEMS)return fail(LIMIT_EXCEEDED);
        if(wanted<=o.cap)return true;
        size_t cap=o.cap ? o.cap*2 : 4;
        cap=std::min<size_t>(MAX_ITEMS,std::max(cap,wanted));
        if(o.type==0){
            double *p=new(std::nothrow) double[cap];
            if(!p)return fail(OUT_OF_MEMORY);
            for(size_t i=0;i<o.len;++i)p[i]=o.n[i];
            delete[] o.n;o.n=p;
        }else if(o.type==1){
            String *p=new(std::nothrow) String[cap];
            if(!p)return fail(OUT_OF_MEMORY);
            // String copying is checked: Arduino String does not throw on OOM.
            for(size_t i=0;i<o.len;++i)if(!copyText(o.s[i],p[i])){delete[] p;return false;}
            delete[] o.s;o.s=p;
        }else{
            Entry *p=new(std::nothrow) Entry[cap];
            if(!p)return fail(OUT_OF_MEMORY);
            for(size_t i=0;i<o.len;++i){
                if(!copyText(o.d[i].key,p[i].key)){delete[] p;return false;}
                p[i].value=o.d[i].value;
            }
            delete[] o.d;o.d=p;
        }
        o.cap=cap;return true;
    }
    static bool putNum(Obj& o,double value){
        if(!finite(value))return false;
        if(o.bytes()+sizeof(double)>MAX_PAYLOAD_BYTES)return fail(LIMIT_EXCEEDED);
        if(!reserve(o,o.len+1))return false;
        o.n[o.len++]=value;return true;
    }
    static bool putStr(Obj& o,const String& value){
        if(value.length()>MAX_TEXT_BYTES || o.bytes()+sizeof(String)+value.length()+1>MAX_PAYLOAD_BYTES)return fail(LIMIT_EXCEEDED);
        if(!reserve(o,o.len+1) || !copyText(value,o.s[o.len]))return false;
        ++o.len;return true;
    }
    static int keyIndex(const Obj& o,const String& key){
        for(size_t i=0;i<o.len;++i)if(o.d[i].key==key)return (int)i;
        return -1;
    }
    static bool putKey(Obj& o,const String& key,double value){
        if(key.length()>MAX_TEXT_BYTES)return fail(LIMIT_EXCEEDED);
        if(!finite(value))return false;
        int i=keyIndex(o,key);
        if(i>=0){o.d[i].value=value;return true;}
        if(o.bytes()+sizeof(Entry)+key.length()+1>MAX_PAYLOAD_BYTES)return fail(LIMIT_EXCEEDED);
        if(!reserve(o,o.len+1) || !copyText(key,o.d[o.len].key))return false;
        o.d[o.len].value=value;++o.len;
        // Preserve the original std::map's lexicographic key order.
        for(size_t j=o.len-1;j>0 && o.d[j].key<o.d[j-1].key;--j){
            std::swap(o.d[j].key,o.d[j-1].key);std::swap(o.d[j].value,o.d[j-1].value);
        }
        return true;
    }
    // Commit by swapping owned buffers. Failed replacement leaves the destination intact.
    static bool commit(const String& name,Obj& tmp){
        if(!validName(name))return false;
        Obj *p=lookup(name,tmp.type==2);
        if(!budget(p?p->bytes():0,tmp.bytes()))return false;
        if(!p){
            if(state().count>=MAX_CONTAINERS)return fail(LIMIT_EXCEEDED);
            p=new(std::nothrow) Obj(tmp.type);
            if(!p)return fail(OUT_OF_MEMORY);
            if(!copyText(name,p->name)){delete p;return false;}
            p->next=state().head;state().head=p;++state().count;
        }
        std::swap(p->type,tmp.type);std::swap(p->len,tmp.len);std::swap(p->cap,tmp.cap);
        std::swap(p->n,tmp.n);std::swap(p->s,tmp.s);std::swap(p->d,tmp.d);
        return true;
    }
    static int norm(int idx,int len){return idx<0 ? len+idx : idx;}
    static bool indexArgument(double value,int& idx){
        if(!std::isfinite(value) || value<INT_MIN || value>INT_MAX || std::trunc(value)!=value)return fail(INVALID_INPUT);
        idx=(int)value;return true;
    }
    static int checkedIndex(double value,const Obj& o){
        int idx;if(!indexArgument(value,idx))return -1;
        int i=norm(idx,(int)o.len);
        if(i<0 || i>=(int)o.len){fail(BAD_INDEX);return -1;}return i;
    }
    static bool eraseItem(Obj& o,size_t idx){
        // Move strings with swap, avoiding unchecked allocating assignment.
        for(size_t i=idx;i+1<o.len;++i){
            if(o.type==0)o.n[i]=o.n[i+1];
            else if(o.type==1)std::swap(o.s[i],o.s[i+1]);
            else {std::swap(o.d[i].key,o.d[i+1].key);o.d[i].value=o.d[i+1].value;}
        }
        --o.len;
        if(o.type==1){String empty;std::swap(empty,o.s[o.len]);}
        if(o.type==2){String empty;std::swap(empty,o.d[o.len].key);}
        return true;
    }
    static bool parse(const String& text,double& value){
        if(!text.length())return fail(INVALID_INPUT);
        errno=0;char *end=NULL;value=std::strtod(text.c_str(),&end);
        return (end!=text.c_str() && (size_t)(end-text.c_str())==text.length() && errno!=ERANGE && std::isfinite(value)) || fail(INVALID_INPUT);
    }
    static bool init(const String& name,const String& data,const String& sep,const String& kv,int type){
        if(!validName(name))return false;
        if(data.length()>MAX_INPUT_BYTES || sep.length()>MAX_TEXT_BYTES || kv.length()>MAX_TEXT_BYTES)return fail(LIMIT_EXCEEDED);
        Obj tmp(type);
        // No allocated fallback delimiter; empty separators use literal defaults.
        const String comma(","),colon(":");
        if(comma.length()!=1 || colon.length()!=1)return fail(OUT_OF_MEMORY);
        const String& delimiter=sep.length()?sep:comma;
        const String& keySep=kv.length()?kv:colon;
        int start=0,len=(int)data.length();
        while(start<=len){
            int pos=data.indexOf(delimiter,start),stop=pos<0?len:pos;
            String tok=data.substring(start,stop);
            if(tok.length()!=(size_t)(stop-start))return fail(OUT_OF_MEMORY);
            tok.trim();
            if(tok.length()){
                if(type==0){double v;if(!parse(tok,v)||!putNum(tmp,v))return false;}
                else if(type==1){if(!putStr(tmp,tok))return false;}
                else{
                    int k=tok.indexOf(keySep);
                    if(k<=0)return fail(INVALID_INPUT);
                    String key=tok.substring(0,k),val=tok.substring(k+keySep.length());
                    if(key.length()!=(size_t)k || val.length()!=tok.length()-k-keySep.length())return fail(OUT_OF_MEMORY);
                    key.trim();val.trim();double v;
                    if(!key.length())return fail(INVALID_INPUT);
                    if(!parse(val,v)||!putKey(tmp,key,v))return false;
                }
            }
            if(pos<0)break;
            start=pos+(int)delimiter.length();
        }
        return commit(name,tmp);
    }
    static void slice(const String& src,int start,int end,const String& dst,int type){
        Obj *p=typed(src,type);if(!p)return;
        int a=std::max(0,std::min((int)p->len,norm(start,(int)p->len)));
        int b=std::max(0,std::min((int)p->len,norm(end,(int)p->len)));
        Obj tmp(type);
        for(int i=a;i<b;++i)if(type==0 ? !putNum(tmp,p->n[i]) : !putStr(tmp,p->s[i]))return;
        commit(dst,tmp);
    }
    static bool appendOutput(String& out,const String& text){
        size_t wanted=out.length()+text.length();
        if(wanted>MAX_OUTPUT_BYTES)return fail(LIMIT_EXCEEDED);
        if(wanted && !out.reserve(wanted))return fail(OUT_OF_MEMORY);
        out+=text;return out.length()==wanted || fail(OUT_OF_MEMORY);
    }
    static String joined(const String& name,const String& sep,int type){
        Obj *p=typed(name,type);if(!p)return String();
        if(sep.length()>MAX_OUTPUT_BYTES){fail(LIMIT_EXCEEDED);return String();}
        String out;
        for(size_t i=0;i<p->len;++i){
            if(i && !appendOutput(out,sep))return String();
            if(type==0){
                String v(p->n[i],2);
                if(!v.length()){fail(OUT_OF_MEMORY);return String();}
                if(!appendOutput(out,v))return String();
            }else if(!appendOutput(out,type==1?p->s[i]:p->d[i].key))return String();
        }
        return out;
    }
public:
    // Error state belongs to the last operation; querying it does not reset it.
    static int lastError(){return state().error;}
    static String lastErrorText(){
        switch(state().error){
        case OK:return String("OK");case NOT_FOUND:return String("container not found");
        case TYPE_MISMATCH:return String("wrong list type");case BAD_INDEX:return String("index out of range");
        case LIMIT_EXCEEDED:return String("capacity limit exceeded");case OUT_OF_MEMORY:return String("out of memory");
        case INVALID_INPUT:return String("invalid input");case KEY_NOT_FOUND:return String("key not found");
        default:return String("empty list");}
    }
    static void resetError(){begin();}
    static void createNum(const String& name){begin();Obj tmp(0);commit(name,tmp);}
    static void createStr(const String& name){begin();Obj tmp(1);commit(name,tmp);}
    static void dictCreate(const String& name){begin();Obj tmp(2);commit(name,tmp);}
    static void clear(const String& name){
        begin();if(!validName(name))return;Obj *p=lookup(name);
        if(!p){fail(NOT_FOUND);return;}p->release();
    }
    static void dictClear(const String& name){begin();Obj *p=typed(name,2);if(p)p->release();}
    static bool destroy(const String& name,bool dict=false){
        begin();if(!validName(name))return false;
        Obj **link=&state().head;
        while(*link){Obj *p=*link;if((p->type==2)==dict && p->name==name){*link=p->next;delete p;--state().count;return true;}link=&p->next;}
        return fail(NOT_FOUND);
    }
    static bool dictDestroy(const String& name){return destroy(name,true);}
    static void resetAll(){begin();while(state().head){Obj *p=state().head;state().head=p->next;delete p;}state().count=0;}
    static int size(const String& name){begin();if(!validName(name))return 0;Obj *p=lookup(name);if(!p){fail(NOT_FOUND);return 0;}return (int)p->len;}
    static bool exists(const String& name){begin();return validName(name) && lookup(name)!=NULL;}
    static void addNum(const String& name,double value){
        begin();Obj *p=typed(name,0);if(!p)return;
        if(budget(0,sizeof(double)))putNum(*p,value);
    }
    static void addStr(const String& name,const String& value){
        begin();Obj *p=typed(name,1);if(!p)return;
        if(budget(0,sizeof(String)+value.length()+1))putStr(*p,value);
    }
    static void insertNum(const String& name,double index,double value){
        begin();Obj *p=typed(name,0);if(!p)return;
        int idx;if(!indexArgument(index,idx))return;
        int i=std::max(0,std::min((int)p->len,norm(idx,(int)p->len)));
        if(!budget(0,sizeof(double)) || !putNum(*p,value))return;
        for(size_t j=p->len-1;j>(size_t)i;--j)p->n[j]=p->n[j-1];p->n[i]=value;
    }
    static void insertStr(const String& name,double index,const String& value){
        begin();Obj *p=typed(name,1);if(!p)return;
        int idx;if(!indexArgument(index,idx))return;
        int i=std::max(0,std::min((int)p->len,norm(idx,(int)p->len)));
        if(!budget(0,sizeof(String)+value.length()+1) || !putStr(*p,value))return;
        for(size_t j=p->len-1;j>(size_t)i;--j)std::swap(p->s[j],p->s[j-1]);
    }
    static double getNum(const String& name,double idx){begin();Obj *p=typed(name,0);if(!p)return 0;int i=checkedIndex(idx,*p);return i<0?0:p->n[i];}
    static String getStr(const String& name,double idx){begin();Obj *p=typed(name,1);if(!p)return String();int i=checkedIndex(idx,*p);String v;if(i>=0)copyText(p->s[i],v);return v;}
    static void setNum(const String& name,double idx,double value){begin();Obj *p=typed(name,0);if(!p)return;int i=checkedIndex(idx,*p);if(i>=0 && finite(value))p->n[i]=value;}
    static void setStr(const String& name,double idx,const String& value){
        begin();Obj *p=typed(name,1);if(!p)return;int i=checkedIndex(idx,*p);if(i<0)return;
        if(value.length()>MAX_TEXT_BYTES){fail(LIMIT_EXCEEDED);return;}
        if(!budget(p->s[i].length(),value.length()))return;
        String tmp;if(copyText(value,tmp))std::swap(p->s[i],tmp);
    }
    static bool removeAt(const String& name,double idx){begin();if(!validName(name))return false;Obj *p=lookup(name);if(!p)return fail(NOT_FOUND);int i=checkedIndex(idx,*p);return i>=0 && eraseItem(*p,i);}
    static bool popBack(const String& name){return removeAt(name,-1);}
    static bool removeValueNum(const String& name,double value){begin();Obj *p=typed(name,0);if(!p)return false;for(size_t i=0;i<p->len;++i)if(p->n[i]==value)return eraseItem(*p,i);return false;}
    static bool removeValueStr(const String& name,const String& value){begin();Obj *p=typed(name,1);if(!p)return false;for(size_t i=0;i<p->len;++i)if(p->s[i]==value)return eraseItem(*p,i);return false;}
    static int indexOfNum(const String& name,double value){begin();Obj *p=typed(name,0);if(p)for(size_t i=0;i<p->len;++i)if(p->n[i]==value)return (int)i;return -1;}
    static int indexOfStr(const String& name,const String& value){begin();Obj *p=typed(name,1);if(p)for(size_t i=0;i<p->len;++i)if(p->s[i]==value)return (int)i;return -1;}
    static bool containsNum(const String& name,double value){return indexOfNum(name,value)>=0;}
    static bool containsStr(const String& name,const String& value){return indexOfStr(name,value)>=0;}
    static double sumNum(const String& name){begin();Obj *p=typed(name,0);double v=0;if(p)for(size_t i=0;i<p->len;++i){v+=p->n[i];if(!finite(v))return 0;}return v;}
    static double maxNum(const String& name){begin();Obj *p=typed(name,0);if(!p)return 0;if(!p->len){fail(EMPTY_LIST);return 0;}return *std::max_element(p->n,p->n+p->len);}
    static double minNum(const String& name){begin();Obj *p=typed(name,0);if(!p)return 0;if(!p->len){fail(EMPTY_LIST);return 0;}return *std::min_element(p->n,p->n+p->len);}
    static double avgNum(const String& name){double v=sumNum(name);if(lastError()!=OK)return 0;Obj *p=lookup(name);if(!p->len){fail(EMPTY_LIST);return 0;}return v/p->len;}
    static void sortNum(const String& name){begin();Obj *p=typed(name,0);if(p && p->len>1)std::sort(p->n,p->n+p->len);}
    static void sortStr(const String& name){begin();Obj *p=typed(name,1);if(p && p->len>1)std::sort(p->s,p->s+p->len);}
    static String joinStr(const String& name,const String& sep){begin();return joined(name,sep,1);}
    static String dumpNum(const String& name,const String& sep){begin();return joined(name,sep,0);}
    static void sliceNum(const String& src,double start,double end,const String& dst){begin();int a,b;if(indexArgument(start,a) && indexArgument(end,b))slice(src,a,b,dst,0);}
    static void sliceStr(const String& src,double start,double end,const String& dst){begin();int a,b;if(indexArgument(start,a) && indexArgument(end,b))slice(src,a,b,dst,1);}
    static void copy(const String& src,const String& dst){begin();if(!validName(src))return;Obj *p=lookup(src);if(!p){fail(NOT_FOUND);return;}slice(src,0,(int)p->len,dst,p->type);}
    static void dictSet(const String& name,const String& key,double value){
        begin();Obj *p=typed(name,2);if(!p)return;
        if(keyIndex(*p,key)>=0 || budget(0,sizeof(Entry)+key.length()+1))putKey(*p,key,value);
    }
    static double dictGet(const String& name,const String& key){begin();Obj *p=typed(name,2);if(!p)return 0;int i=keyIndex(*p,key);if(i<0){fail(KEY_NOT_FOUND);return 0;}return p->d[i].value;}
    static bool dictHas(const String& name,const String& key){begin();Obj *p=typed(name,2);return p && keyIndex(*p,key)>=0;}
    static bool dictRemove(const String& name,const String& key){begin();Obj *p=typed(name,2);if(!p)return false;int i=keyIndex(*p,key);if(i<0)return fail(KEY_NOT_FOUND);return eraseItem(*p,i);}
    static int dictSize(const String& name){begin();Obj *p=typed(name,2);return p?(int)p->len:0;}
    static String dictKeys(const String& name,const String& sep){begin();return joined(name,sep,2);}
    static void initNum(const String& name,const String& data,const String& sep){begin();init(name,data,sep,String(),0);}
    static void initStr(const String& name,const String& data,const String& sep){begin();init(name,data,sep,String(),1);}
    static void initDict(const String& name,const String& data,const String& sep,const String& kvSep){begin();init(name,data,sep,kvSep,2);}
};
#endif
