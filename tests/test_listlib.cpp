#include <new>
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <limits>
#include <cassert>
namespace TestMemory { int arrayCountdown=-1; int stringCountdown=-1;
 bool arrayFail(){if(arrayCountdown<0)return false;if(arrayCountdown==0)return true;--arrayCountdown;return false;}}
void* operator new(std::size_t n,const std::nothrow_t&) noexcept {return TestMemory::arrayFail()?NULL: ::operator new(n);}
void* operator new[](std::size_t n,const std::nothrow_t&) noexcept {return TestMemory::arrayFail()?NULL: ::operator new[](n);}
#include "listlib.h"
static void ok(){assert(ListLib::lastError()==ListLib::OK);}
static void testBasic(){
 ListLib::resetAll();ListLib::initNum("n","1,2,3",",");ok();
 assert(ListLib::getNum("n",-1)==3);ok();
 assert(ListLib::getNum("n",999)==0);assert(ListLib::lastError()==ListLib::BAD_INDEX);
 assert(ListLib::getNum("n",INT_MIN)==0);assert(ListLib::lastError()==ListLib::BAD_INDEX);
 ListLib::getNum("n",1e100);assert(ListLib::lastError()==ListLib::INVALID_INPUT);
 ListLib::getNum("n",std::numeric_limits<double>::quiet_NaN());assert(ListLib::lastError()==ListLib::INVALID_INPUT);
 ListLib::insertNum("n",0.5,9);assert(ListLib::lastError()==ListLib::INVALID_INPUT);assert(ListLib::size("n")==3);
 ListLib::sliceNum("n",0,std::numeric_limits<double>::infinity(),"n");assert(ListLib::lastError()==ListLib::INVALID_INPUT);assert(ListLib::size("n")==3);
 ListLib::sliceNum("n",0,2,"n");ok();assert(ListLib::size("n")==2);assert(ListLib::getNum("n",1)==2);
 ListLib::copy("n","n");ok();assert(ListLib::size("n")==2);
 ListLib::copy("n","copy");ListLib::setNum("copy",0,9);assert(ListLib::getNum("n",0)==1);
 ListLib::sliceNum("missing",0,2,"copy");assert(ListLib::lastError()==ListLib::NOT_FOUND);assert(ListLib::getNum("copy",0)==9);
 ListLib::sliceNum("n",INT_MIN,INT_MAX,"copy");assert(ListLib::size("copy")==2);
 ListLib::sliceNum("n",2,0,"copy");assert(ListLib::size("copy")==0);
 ListLib::initStr("s","a,b,c",",");ListLib::sliceStr("s",0,2,"s");ok();assert(ListLib::joinStr("s",",")==String("a,b"));
 ListLib::insertStr("s",1,"x");assert(ListLib::joinStr("s",",")==String("a,x,b"));
 ListLib::removeAt("s",1);assert(ListLib::joinStr("s",",")==String("a,b"));
 ListLib::insertNum("n",1,8);assert(ListLib::getNum("n",1)==8);ListLib::sortNum("n");assert(ListLib::getNum("n",-1)==8);
 ListLib::createStr("n");assert(ListLib::size("n")==0);ListLib::addNum("n",4);assert(ListLib::lastError()==ListLib::TYPE_MISMATCH);
 ListLib::addStr("absent","x");assert(ListLib::lastError()==ListLib::NOT_FOUND);assert(!ListLib::exists("absent"));
 ListLib::getNum("absent",0);int error=ListLib::lastError();assert(ListLib::lastErrorText()==String("container not found"));assert(ListLib::lastError()==error);
 ListLib::resetError();ok();ListLib::maxNum("n");assert(ListLib::lastError()==ListLib::TYPE_MISMATCH);
 ListLib::createNum("empty");ListLib::avgNum("empty");assert(ListLib::lastError()==ListLib::EMPTY_LIST);
}
static void testDictionary(){
 ListLib::resetAll();ListLib::initDict("d","old:1",",",":");ListLib::initDict("d","new:2",",",":");ok();
 assert(!ListLib::dictHas("d","old"));assert(ListLib::dictGet("d","new")==2);
 ListLib::initDict("d","",",",":");assert(ListLib::dictSize("d")==0);
 ListLib::dictSet("d","z",3);ListLib::dictSet("d","a",1);ListLib::dictSet("d","b",2);
 assert(ListLib::dictKeys("d",",")==String("a,b,z"));ListLib::dictRemove("d","b");assert(ListLib::dictGet("d","z")==3);
 ListLib::initDict("d","bad:no_number",",",":");assert(ListLib::lastError()==ListLib::INVALID_INPUT);assert(ListLib::dictSize("d")==2);
 ListLib::dictCreate("d");assert(ListLib::dictSize("d")==0);
 ListLib::dictGet("d","absent");assert(ListLib::lastError()==ListLib::KEY_NOT_FOUND);
 ListLib::createNum("d");assert(ListLib::dictDestroy("d"));assert(ListLib::exists("d"));
}
static void testLimits(){
 ListLib::resetAll();ListLib::createNum("n");
 for(int i=0;i<ListLib::MAX_ITEMS;++i)ListLib::addNum("n",i);ok();
 ListLib::addNum("n",999);assert(ListLib::lastError()==ListLib::LIMIT_EXCEEDED);assert(ListLib::size("n")==ListLib::MAX_ITEMS);
 ListLib::clear("n");ok();assert(ListLib::size("n")==0);
 for(int j=0;j<100;++j){for(int i=0;i<256;++i)ListLib::addNum("n",i);ListLib::clear("n");}
 String oversized(std::string(ListLib::MAX_TEXT_BYTES+1,'x'));ListLib::createStr("s");ListLib::addStr("s",oversized);assert(ListLib::lastError()==ListLib::LIMIT_EXCEEDED);assert(ListLib::size("s")==0);
 ListLib::createNum(String(std::string(65,'x')));assert(ListLib::lastError()==ListLib::INVALID_INPUT);
 ListLib::initNum("n","4,5",",");ListLib::initNum("n","1,12garbage",",");assert(ListLib::lastError()==ListLib::INVALID_INPUT);assert(ListLib::getNum("n",0)==4);
 ListLib::addNum("n",std::numeric_limits<double>::quiet_NaN());assert(ListLib::lastError()==ListLib::INVALID_INPUT);
 ListLib::addNum("n",std::numeric_limits<double>::infinity());assert(ListLib::lastError()==ListLib::INVALID_INPUT);
 ListLib::initNum("n","1,,2,","");assert(ListLib::size("n")==2);
 ListLib::initNum("n",String(std::string(4097,'1')),",");assert(ListLib::lastError()==ListLib::LIMIT_EXCEEDED);assert(ListLib::size("n")==2);
 ListLib::resetAll();
 for(int i=0;i<ListLib::MAX_CONTAINERS;++i){char n[16];std::snprintf(n,sizeof(n),"n%d",i);ListLib::createNum(n);ok();}
 ListLib::dictCreate("overflow");assert(ListLib::lastError()==ListLib::LIMIT_EXCEEDED);
 ListLib::destroy("n0");ListLib::dictCreate("overflow");ok();
 ListLib::resetAll();String text(std::string(256,'x'));
 ListLib::createStr("s");bool limited=false;
 for(int i=0;i<256;++i){ListLib::addStr("s",text);if(ListLib::lastError()==ListLib::LIMIT_EXCEEDED){limited=true;break;}}
 assert(limited);int size=ListLib::size("s");assert(size>0 && size<256);
 ListLib::joinStr("s",",");assert(ListLib::lastError()==ListLib::LIMIT_EXCEEDED);
 ListLib::copy("s","copy");assert(ListLib::lastError()==ListLib::LIMIT_EXCEEDED);assert(!ListLib::exists("copy"));
 ListLib::clear("s");ListLib::createStr("s2");ListLib::addStr("s2",text);ok();
}
static void testFailures(){
 ListLib::resetAll();String n("n"),s("s"),a("a"),key("key"),data("2,3"),sep(",");
 TestMemory::arrayCountdown=0;ListLib::createNum(n);assert(ListLib::lastError()==ListLib::OUT_OF_MEMORY);TestMemory::arrayCountdown=-1;assert(!ListLib::exists(n));
 ListLib::createNum(n);TestMemory::arrayCountdown=0;ListLib::addNum(n,1);assert(ListLib::lastError()==ListLib::OUT_OF_MEMORY);TestMemory::arrayCountdown=-1;assert(ListLib::size(n)==0);
 ListLib::addNum(n,1);TestMemory::arrayCountdown=0;ListLib::initNum(n,data,sep);assert(ListLib::lastError()==ListLib::OUT_OF_MEMORY);TestMemory::arrayCountdown=-1;assert(ListLib::getNum(n,0)==1);
 ListLib::createStr(s);ListLib::addStr(s,a);TestMemory::stringCountdown=0;ListLib::setStr(s,0,key);assert(ListLib::lastError()==ListLib::OUT_OF_MEMORY);TestMemory::stringCountdown=-1;assert(ListLib::getStr(s,0)==a);
 TestMemory::stringCountdown=0;ListLib::getStr(s,0);assert(ListLib::lastError()==ListLib::OUT_OF_MEMORY);TestMemory::stringCountdown=-1;
 // Every possible allocation failure during replacement must preserve the old data.
 for(int failAt=0;failAt<30;++failAt){
   ListLib::initStr(s,"old",sep);TestMemory::stringCountdown=failAt;
   ListLib::initStr(s,"a,b,c,d,e",sep);int error=ListLib::lastError();TestMemory::stringCountdown=-1;
   assert(error==ListLib::OK || error==ListLib::OUT_OF_MEMORY);
   if(error)assert(ListLib::getStr(s,0)==String("old"));else assert(ListLib::size(s)==5);
 }
 for(int failAt=0;failAt<8;++failAt){
   ListLib::initNum(n,"9",sep);TestMemory::arrayCountdown=failAt;
   ListLib::sliceNum(n,0,1,n);int error=ListLib::lastError();TestMemory::arrayCountdown=-1;
   assert(error==ListLib::OK || error==ListLib::OUT_OF_MEMORY);assert(ListLib::getNum(n,0)==9);
 }
 ListLib::resetAll();
}
int main(){testBasic();testDictionary();testLimits();testFailures();std::puts("PASS: lists, dictionaries, boundaries, budgets, release/reuse, allocation failures");}
