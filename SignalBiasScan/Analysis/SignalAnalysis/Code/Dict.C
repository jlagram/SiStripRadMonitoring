// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME Dict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "../../../interface/TreeEvent.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *TreeHit_Dictionary();
   static void TreeHit_TClassManip(TClass*);
   static void *new_TreeHit(void *p = 0);
   static void *newArray_TreeHit(Long_t size, void *p);
   static void delete_TreeHit(void *p);
   static void deleteArray_TreeHit(void *p);
   static void destruct_TreeHit(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TreeHit*)
   {
      ::TreeHit *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::TreeHit));
      static ::ROOT::TGenericClassInfo 
         instance("TreeHit", "../../../interface/TreeEvent.h", 20,
                  typeid(::TreeHit), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &TreeHit_Dictionary, isa_proxy, 4,
                  sizeof(::TreeHit) );
      instance.SetNew(&new_TreeHit);
      instance.SetNewArray(&newArray_TreeHit);
      instance.SetDelete(&delete_TreeHit);
      instance.SetDeleteArray(&deleteArray_TreeHit);
      instance.SetDestructor(&destruct_TreeHit);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TreeHit*)
   {
      return GenerateInitInstanceLocal((::TreeHit*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::TreeHit*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *TreeHit_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::TreeHit*)0x0)->GetClass();
      TreeHit_TClassManip(theClass);
   return theClass;
   }

   static void TreeHit_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *TreeFullHit_Dictionary();
   static void TreeFullHit_TClassManip(TClass*);
   static void *new_TreeFullHit(void *p = 0);
   static void *newArray_TreeFullHit(Long_t size, void *p);
   static void delete_TreeFullHit(void *p);
   static void deleteArray_TreeFullHit(void *p);
   static void destruct_TreeFullHit(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TreeFullHit*)
   {
      ::TreeFullHit *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::TreeFullHit));
      static ::ROOT::TGenericClassInfo 
         instance("TreeFullHit", "../../../interface/TreeEvent.h", 59,
                  typeid(::TreeFullHit), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &TreeFullHit_Dictionary, isa_proxy, 4,
                  sizeof(::TreeFullHit) );
      instance.SetNew(&new_TreeFullHit);
      instance.SetNewArray(&newArray_TreeFullHit);
      instance.SetDelete(&delete_TreeFullHit);
      instance.SetDeleteArray(&deleteArray_TreeFullHit);
      instance.SetDestructor(&destruct_TreeFullHit);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TreeFullHit*)
   {
      return GenerateInitInstanceLocal((::TreeFullHit*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::TreeFullHit*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *TreeFullHit_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::TreeFullHit*)0x0)->GetClass();
      TreeFullHit_TClassManip(theClass);
   return theClass;
   }

   static void TreeFullHit_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *TreeTrack_Dictionary();
   static void TreeTrack_TClassManip(TClass*);
   static void *new_TreeTrack(void *p = 0);
   static void *newArray_TreeTrack(Long_t size, void *p);
   static void delete_TreeTrack(void *p);
   static void deleteArray_TreeTrack(void *p);
   static void destruct_TreeTrack(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TreeTrack*)
   {
      ::TreeTrack *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::TreeTrack));
      static ::ROOT::TGenericClassInfo 
         instance("TreeTrack", "../../../interface/TreeEvent.h", 108,
                  typeid(::TreeTrack), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &TreeTrack_Dictionary, isa_proxy, 4,
                  sizeof(::TreeTrack) );
      instance.SetNew(&new_TreeTrack);
      instance.SetNewArray(&newArray_TreeTrack);
      instance.SetDelete(&delete_TreeTrack);
      instance.SetDeleteArray(&deleteArray_TreeTrack);
      instance.SetDestructor(&destruct_TreeTrack);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TreeTrack*)
   {
      return GenerateInitInstanceLocal((::TreeTrack*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::TreeTrack*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *TreeTrack_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::TreeTrack*)0x0)->GetClass();
      TreeTrack_TClassManip(theClass);
   return theClass;
   }

   static void TreeTrack_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *TreeEvent_Dictionary();
   static void TreeEvent_TClassManip(TClass*);
   static void *new_TreeEvent(void *p = 0);
   static void *newArray_TreeEvent(Long_t size, void *p);
   static void delete_TreeEvent(void *p);
   static void deleteArray_TreeEvent(void *p);
   static void destruct_TreeEvent(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TreeEvent*)
   {
      ::TreeEvent *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::TreeEvent));
      static ::ROOT::TGenericClassInfo 
         instance("TreeEvent", "../../../interface/TreeEvent.h", 156,
                  typeid(::TreeEvent), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &TreeEvent_Dictionary, isa_proxy, 4,
                  sizeof(::TreeEvent) );
      instance.SetNew(&new_TreeEvent);
      instance.SetNewArray(&newArray_TreeEvent);
      instance.SetDelete(&delete_TreeEvent);
      instance.SetDeleteArray(&deleteArray_TreeEvent);
      instance.SetDestructor(&destruct_TreeEvent);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TreeEvent*)
   {
      return GenerateInitInstanceLocal((::TreeEvent*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::TreeEvent*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *TreeEvent_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::TreeEvent*)0x0)->GetClass();
      TreeEvent_TClassManip(theClass);
   return theClass;
   }

   static void TreeEvent_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_TreeHit(void *p) {
      return  p ? new(p) ::TreeHit : new ::TreeHit;
   }
   static void *newArray_TreeHit(Long_t nElements, void *p) {
      return p ? new(p) ::TreeHit[nElements] : new ::TreeHit[nElements];
   }
   // Wrapper around operator delete
   static void delete_TreeHit(void *p) {
      delete ((::TreeHit*)p);
   }
   static void deleteArray_TreeHit(void *p) {
      delete [] ((::TreeHit*)p);
   }
   static void destruct_TreeHit(void *p) {
      typedef ::TreeHit current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TreeHit

namespace ROOT {
   // Wrappers around operator new
   static void *new_TreeFullHit(void *p) {
      return  p ? new(p) ::TreeFullHit : new ::TreeFullHit;
   }
   static void *newArray_TreeFullHit(Long_t nElements, void *p) {
      return p ? new(p) ::TreeFullHit[nElements] : new ::TreeFullHit[nElements];
   }
   // Wrapper around operator delete
   static void delete_TreeFullHit(void *p) {
      delete ((::TreeFullHit*)p);
   }
   static void deleteArray_TreeFullHit(void *p) {
      delete [] ((::TreeFullHit*)p);
   }
   static void destruct_TreeFullHit(void *p) {
      typedef ::TreeFullHit current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TreeFullHit

namespace ROOT {
   // Wrappers around operator new
   static void *new_TreeTrack(void *p) {
      return  p ? new(p) ::TreeTrack : new ::TreeTrack;
   }
   static void *newArray_TreeTrack(Long_t nElements, void *p) {
      return p ? new(p) ::TreeTrack[nElements] : new ::TreeTrack[nElements];
   }
   // Wrapper around operator delete
   static void delete_TreeTrack(void *p) {
      delete ((::TreeTrack*)p);
   }
   static void deleteArray_TreeTrack(void *p) {
      delete [] ((::TreeTrack*)p);
   }
   static void destruct_TreeTrack(void *p) {
      typedef ::TreeTrack current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TreeTrack

namespace ROOT {
   // Wrappers around operator new
   static void *new_TreeEvent(void *p) {
      return  p ? new(p) ::TreeEvent : new ::TreeEvent;
   }
   static void *newArray_TreeEvent(Long_t nElements, void *p) {
      return p ? new(p) ::TreeEvent[nElements] : new ::TreeEvent[nElements];
   }
   // Wrapper around operator delete
   static void delete_TreeEvent(void *p) {
      delete ((::TreeEvent*)p);
   }
   static void deleteArray_TreeEvent(void *p) {
      delete [] ((::TreeEvent*)p);
   }
   static void destruct_TreeEvent(void *p) {
      typedef ::TreeEvent current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TreeEvent

namespace ROOT {
   static TClass *vectorlEunsignedsPchargR_Dictionary();
   static void vectorlEunsignedsPchargR_TClassManip(TClass*);
   static void *new_vectorlEunsignedsPchargR(void *p = 0);
   static void *newArray_vectorlEunsignedsPchargR(Long_t size, void *p);
   static void delete_vectorlEunsignedsPchargR(void *p);
   static void deleteArray_vectorlEunsignedsPchargR(void *p);
   static void destruct_vectorlEunsignedsPchargR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<unsigned char>*)
   {
      vector<unsigned char> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<unsigned char>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<unsigned char>", -2, "vector", 214,
                  typeid(vector<unsigned char>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEunsignedsPchargR_Dictionary, isa_proxy, 0,
                  sizeof(vector<unsigned char>) );
      instance.SetNew(&new_vectorlEunsignedsPchargR);
      instance.SetNewArray(&newArray_vectorlEunsignedsPchargR);
      instance.SetDelete(&delete_vectorlEunsignedsPchargR);
      instance.SetDeleteArray(&deleteArray_vectorlEunsignedsPchargR);
      instance.SetDestructor(&destruct_vectorlEunsignedsPchargR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<unsigned char> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<unsigned char>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEunsignedsPchargR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<unsigned char>*)0x0)->GetClass();
      vectorlEunsignedsPchargR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEunsignedsPchargR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEunsignedsPchargR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<unsigned char> : new vector<unsigned char>;
   }
   static void *newArray_vectorlEunsignedsPchargR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<unsigned char>[nElements] : new vector<unsigned char>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEunsignedsPchargR(void *p) {
      delete ((vector<unsigned char>*)p);
   }
   static void deleteArray_vectorlEunsignedsPchargR(void *p) {
      delete [] ((vector<unsigned char>*)p);
   }
   static void destruct_vectorlEunsignedsPchargR(void *p) {
      typedef vector<unsigned char> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<unsigned char>

namespace ROOT {
   static TClass *vectorlEfloatgR_Dictionary();
   static void vectorlEfloatgR_TClassManip(TClass*);
   static void *new_vectorlEfloatgR(void *p = 0);
   static void *newArray_vectorlEfloatgR(Long_t size, void *p);
   static void delete_vectorlEfloatgR(void *p);
   static void deleteArray_vectorlEfloatgR(void *p);
   static void destruct_vectorlEfloatgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<float>*)
   {
      vector<float> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<float>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<float>", -2, "vector", 214,
                  typeid(vector<float>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEfloatgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<float>) );
      instance.SetNew(&new_vectorlEfloatgR);
      instance.SetNewArray(&newArray_vectorlEfloatgR);
      instance.SetDelete(&delete_vectorlEfloatgR);
      instance.SetDeleteArray(&deleteArray_vectorlEfloatgR);
      instance.SetDestructor(&destruct_vectorlEfloatgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<float> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<float>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEfloatgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<float>*)0x0)->GetClass();
      vectorlEfloatgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEfloatgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEfloatgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<float> : new vector<float>;
   }
   static void *newArray_vectorlEfloatgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<float>[nElements] : new vector<float>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEfloatgR(void *p) {
      delete ((vector<float>*)p);
   }
   static void deleteArray_vectorlEfloatgR(void *p) {
      delete [] ((vector<float>*)p);
   }
   static void destruct_vectorlEfloatgR(void *p) {
      typedef vector<float> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<float>

namespace ROOT {
   static TClass *vectorlEboolgR_Dictionary();
   static void vectorlEboolgR_TClassManip(TClass*);
   static void *new_vectorlEboolgR(void *p = 0);
   static void *newArray_vectorlEboolgR(Long_t size, void *p);
   static void delete_vectorlEboolgR(void *p);
   static void deleteArray_vectorlEboolgR(void *p);
   static void destruct_vectorlEboolgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<bool>*)
   {
      vector<bool> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<bool>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<bool>", -2, "vector", 541,
                  typeid(vector<bool>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEboolgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<bool>) );
      instance.SetNew(&new_vectorlEboolgR);
      instance.SetNewArray(&newArray_vectorlEboolgR);
      instance.SetDelete(&delete_vectorlEboolgR);
      instance.SetDeleteArray(&deleteArray_vectorlEboolgR);
      instance.SetDestructor(&destruct_vectorlEboolgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<bool> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<bool>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEboolgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<bool>*)0x0)->GetClass();
      vectorlEboolgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEboolgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEboolgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<bool> : new vector<bool>;
   }
   static void *newArray_vectorlEboolgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<bool>[nElements] : new vector<bool>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEboolgR(void *p) {
      delete ((vector<bool>*)p);
   }
   static void deleteArray_vectorlEboolgR(void *p) {
      delete [] ((vector<bool>*)p);
   }
   static void destruct_vectorlEboolgR(void *p) {
      typedef vector<bool> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<bool>

namespace ROOT {
   static TClass *vectorlETreeTrackgR_Dictionary();
   static void vectorlETreeTrackgR_TClassManip(TClass*);
   static void *new_vectorlETreeTrackgR(void *p = 0);
   static void *newArray_vectorlETreeTrackgR(Long_t size, void *p);
   static void delete_vectorlETreeTrackgR(void *p);
   static void deleteArray_vectorlETreeTrackgR(void *p);
   static void destruct_vectorlETreeTrackgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TreeTrack>*)
   {
      vector<TreeTrack> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TreeTrack>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TreeTrack>", -2, "vector", 214,
                  typeid(vector<TreeTrack>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETreeTrackgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<TreeTrack>) );
      instance.SetNew(&new_vectorlETreeTrackgR);
      instance.SetNewArray(&newArray_vectorlETreeTrackgR);
      instance.SetDelete(&delete_vectorlETreeTrackgR);
      instance.SetDeleteArray(&deleteArray_vectorlETreeTrackgR);
      instance.SetDestructor(&destruct_vectorlETreeTrackgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TreeTrack> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<TreeTrack>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETreeTrackgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TreeTrack>*)0x0)->GetClass();
      vectorlETreeTrackgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETreeTrackgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETreeTrackgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TreeTrack> : new vector<TreeTrack>;
   }
   static void *newArray_vectorlETreeTrackgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TreeTrack>[nElements] : new vector<TreeTrack>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETreeTrackgR(void *p) {
      delete ((vector<TreeTrack>*)p);
   }
   static void deleteArray_vectorlETreeTrackgR(void *p) {
      delete [] ((vector<TreeTrack>*)p);
   }
   static void destruct_vectorlETreeTrackgR(void *p) {
      typedef vector<TreeTrack> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TreeTrack>

namespace ROOT {
   static TClass *vectorlETreeHitgR_Dictionary();
   static void vectorlETreeHitgR_TClassManip(TClass*);
   static void *new_vectorlETreeHitgR(void *p = 0);
   static void *newArray_vectorlETreeHitgR(Long_t size, void *p);
   static void delete_vectorlETreeHitgR(void *p);
   static void deleteArray_vectorlETreeHitgR(void *p);
   static void destruct_vectorlETreeHitgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TreeHit>*)
   {
      vector<TreeHit> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TreeHit>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TreeHit>", -2, "vector", 214,
                  typeid(vector<TreeHit>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETreeHitgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<TreeHit>) );
      instance.SetNew(&new_vectorlETreeHitgR);
      instance.SetNewArray(&newArray_vectorlETreeHitgR);
      instance.SetDelete(&delete_vectorlETreeHitgR);
      instance.SetDeleteArray(&deleteArray_vectorlETreeHitgR);
      instance.SetDestructor(&destruct_vectorlETreeHitgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TreeHit> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<TreeHit>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETreeHitgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TreeHit>*)0x0)->GetClass();
      vectorlETreeHitgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETreeHitgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETreeHitgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TreeHit> : new vector<TreeHit>;
   }
   static void *newArray_vectorlETreeHitgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TreeHit>[nElements] : new vector<TreeHit>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETreeHitgR(void *p) {
      delete ((vector<TreeHit>*)p);
   }
   static void deleteArray_vectorlETreeHitgR(void *p) {
      delete [] ((vector<TreeHit>*)p);
   }
   static void destruct_vectorlETreeHitgR(void *p) {
      typedef vector<TreeHit> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TreeHit>

namespace ROOT {
   static TClass *vectorlETreeFullHitgR_Dictionary();
   static void vectorlETreeFullHitgR_TClassManip(TClass*);
   static void *new_vectorlETreeFullHitgR(void *p = 0);
   static void *newArray_vectorlETreeFullHitgR(Long_t size, void *p);
   static void delete_vectorlETreeFullHitgR(void *p);
   static void deleteArray_vectorlETreeFullHitgR(void *p);
   static void destruct_vectorlETreeFullHitgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TreeFullHit>*)
   {
      vector<TreeFullHit> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TreeFullHit>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TreeFullHit>", -2, "vector", 214,
                  typeid(vector<TreeFullHit>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETreeFullHitgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<TreeFullHit>) );
      instance.SetNew(&new_vectorlETreeFullHitgR);
      instance.SetNewArray(&newArray_vectorlETreeFullHitgR);
      instance.SetDelete(&delete_vectorlETreeFullHitgR);
      instance.SetDeleteArray(&deleteArray_vectorlETreeFullHitgR);
      instance.SetDestructor(&destruct_vectorlETreeFullHitgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TreeFullHit> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<TreeFullHit>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETreeFullHitgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TreeFullHit>*)0x0)->GetClass();
      vectorlETreeFullHitgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETreeFullHitgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETreeFullHitgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TreeFullHit> : new vector<TreeFullHit>;
   }
   static void *newArray_vectorlETreeFullHitgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TreeFullHit>[nElements] : new vector<TreeFullHit>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETreeFullHitgR(void *p) {
      delete ((vector<TreeFullHit>*)p);
   }
   static void deleteArray_vectorlETreeFullHitgR(void *p) {
      delete [] ((vector<TreeFullHit>*)p);
   }
   static void destruct_vectorlETreeFullHitgR(void *p) {
      typedef vector<TreeFullHit> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TreeFullHit>

namespace ROOT {
   static TClass *vectorlETreeEventgR_Dictionary();
   static void vectorlETreeEventgR_TClassManip(TClass*);
   static void *new_vectorlETreeEventgR(void *p = 0);
   static void *newArray_vectorlETreeEventgR(Long_t size, void *p);
   static void delete_vectorlETreeEventgR(void *p);
   static void deleteArray_vectorlETreeEventgR(void *p);
   static void destruct_vectorlETreeEventgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TreeEvent>*)
   {
      vector<TreeEvent> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TreeEvent>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TreeEvent>", -2, "vector", 214,
                  typeid(vector<TreeEvent>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETreeEventgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<TreeEvent>) );
      instance.SetNew(&new_vectorlETreeEventgR);
      instance.SetNewArray(&newArray_vectorlETreeEventgR);
      instance.SetDelete(&delete_vectorlETreeEventgR);
      instance.SetDeleteArray(&deleteArray_vectorlETreeEventgR);
      instance.SetDestructor(&destruct_vectorlETreeEventgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TreeEvent> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<TreeEvent>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETreeEventgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TreeEvent>*)0x0)->GetClass();
      vectorlETreeEventgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETreeEventgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETreeEventgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TreeEvent> : new vector<TreeEvent>;
   }
   static void *newArray_vectorlETreeEventgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TreeEvent>[nElements] : new vector<TreeEvent>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETreeEventgR(void *p) {
      delete ((vector<TreeEvent>*)p);
   }
   static void deleteArray_vectorlETreeEventgR(void *p) {
      delete [] ((vector<TreeEvent>*)p);
   }
   static void destruct_vectorlETreeEventgR(void *p) {
      typedef vector<TreeEvent> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TreeEvent>

namespace {
  void TriggerDictionaryInitialization_Dict_Impl() {
    static const char* headers[] = {
"../../../interface/TreeEvent.h",
0
    };
    static const char* includePaths[] = {
"/cvmfs/cms.cern.ch/slc6_amd64_gcc530/lcg/root/6.08.07/include",
"/afs/cern.ch/user/n/ntonon/public/tracker_aging/CMSSW_9_2_10/src/SiStripRadMonitoring/SignalBiasScan/Analysis/SignalAnalysis/Code/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "Dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$../../../interface/TreeEvent.h")))  TreeHit;
namespace std{template <typename _Tp> class __attribute__((annotate("$clingAutoload$bits/allocator.h")))  __attribute__((annotate("$clingAutoload$string")))  allocator;
}
class __attribute__((annotate("$clingAutoload$../../../interface/TreeEvent.h")))  TreeFullHit;
class __attribute__((annotate("$clingAutoload$../../../interface/TreeEvent.h")))  TreeTrack;
class __attribute__((annotate("$clingAutoload$../../../interface/TreeEvent.h")))  TreeEvent;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "Dict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif
#ifndef NO_ORCA_CLASSES
  #define NO_ORCA_CLASSES 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "../../../interface/TreeEvent.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"TreeEvent", payloadCode, "@",
"TreeFullHit", payloadCode, "@",
"TreeHit", payloadCode, "@",
"TreeTrack", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("Dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_Dict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_Dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_Dict() {
  TriggerDictionaryInitialization_Dict_Impl();
}
