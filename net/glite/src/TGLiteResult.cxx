// @(#) root/glite:$Id: 341cb3fb00aaf4ff088449bebe1b7e95366bd11f $
// Author: Anar Manafov <A.Manafov@gsi.de> 2006-07-30

/*************************************************************************
 * Copyright (C) 1995-2008, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

/************************************************************************/
/*! \file TGLiteResult.cxx
 */ /*

         version number:    $LastChangedRevision: 1678 $
         created by:        Anar Manafov
                            2006-07-30
         last changed by:   $LastChangedBy: manafov $ $LastChangedDate: 2008-01-21 18:22:14 +0100 (Mon, 21 Jan 2008) $

         Copyright (c) 2006 GSI GridTeam. All rights reserved.
*************************************************************************/

//////////////////////////////////////////////////////////////////////////
//
// The TGLiteResult class is a part of RGLite plug-in and
// defines an interface to a gLite result set.
// Objects of this class are created by TGrid methods.
//
// Related classes are TGLite.
//
//////////////////////////////////////////////////////////////////////////


#include "TGLiteResult.h"
#include "TObjString.h"
#include "TMap.h"
#include "Riostream.h"
#include "TSystem.h"
#include "TUrl.h"
#include "TFileInfo.h"
// STD
#include <cstdlib>

ClassImp(TGLiteResult);

////////////////////////////////////////////////////////////////////////////////
/// Dump result set.

void TGLiteResult::DumpResult()
{
   std::cout << "BEGIN DUMP" << std::endl;
   std::cout << "END DUMP" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
///return a file name

const char *TGLiteResult::GetFileName(UInt_t i) const
{
   if (At(i)) {
      TObjString * entry;
      if ((entry = (TObjString*)((TMap*) At(i)) ->GetValue("name"))) {
         return entry->GetName();
      }
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
///return file name path

const char *TGLiteResult::GetFileNamePath(UInt_t i) const
{
   if (At(i)) {
      TObjString * entry;
      if ((entry = (TObjString*)((TMap*) At(i)) ->GetValue("name"))) {
         TObjString * path;
         if ((path = (TObjString*)((TMap*) At(i)) ->GetValue("path"))) {
            TString fFilePath(TString(path->GetName()) + TString(entry->GetName()));
            return fFilePath;
         }
      }
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
///return path

const char *TGLiteResult::GetPath(UInt_t i) const
{
   if (At(i)) {
      TObjString * entry;
      if ((entry = (TObjString*)((TMap*) At(i)) ->GetValue("path"))) {
         return entry->GetName();
      }
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
///return the key

const char *TGLiteResult::GetKey(UInt_t i, const char* key) const
{
   if (At(i)) {
      TObjString * entry;
      if ((entry = (TObjString*)((TMap*) At(i)) ->GetValue(key))) {
         return entry->GetName();
      }
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
///set the key

Bool_t TGLiteResult::SetKey(UInt_t i, const char* key, const char* value)
{
   if (At(i)) {
      TPair * entry;
      if ((entry = (TPair*)((TMap*) At(i)) ->FindObject(key))) {
         TObject * val = ((TMap*) At(i)) ->Remove((TObject*) entry->Key());
         if (val) {
            delete val;
         }
      }
      ((TMap*) At(i)) ->Add((TObject*)(new TObjString(key)),
            (TObject*)(new TObjString(value)));
      return kTRUE;
   }
   return kFALSE;
}

////////////////////////////////////////////////////////////////////////////////
///return a file info list

TList *TGLiteResult::GetFileInfoList() const
{
   TList * newfileinfolist = new TList();


   // TODO: Revise me!!! (according to what TGlite::Ls returns)

   newfileinfolist->SetOwner(kTRUE);

   for (Int_t i = 0; i < GetSize(); i++) {

      Long64_t size = -1;
      if (GetKey(i, "size"))
         size = atol(GetKey(i, "size"));

      const char* md5 = GetKey(i, "md5");
      const char* uuid = GetKey(i, "guid");
      const char* msd = GetKey(i, "msd");

      if (md5 && !md5[0])
         md5 = 0;
      if (uuid && !uuid[0])
         uuid = 0;
      if (msd && !msd[0])
         msd = 0;

      TString turl = GetKey(i, "turl");

      if (msd) {
         TUrl urlturl(turl);
         TString options = urlturl.GetOptions();
         options += "&msd=";
         options += msd;
         urlturl.SetOptions(options);
         turl = urlturl.GetUrl();
      }
      Info("GetFileInfoList", "Adding Url %s with Msd %s\n", turl.Data(), msd);
      newfileinfolist->Add(new TFileInfo(turl, size, uuid, md5));
   }
   return newfileinfolist;
}

////////////////////////////////////////////////////////////////////////////////
///print the GLiteResult info

void TGLiteResult::Print(Option_t * /*wildcard*/, Option_t *option) const
{
   if (TString(option) != TString("all")) {
      //  TODO: Complete me!
      // Long64_t totaldata = 0;
      // Int_t totalfiles = 0;
      //       // the default print out format is for a query
      //     for (Int_t i = 0; i< GetSize(); i++) {
      //       if (TString(option) == TString("l") ) {
      //  printf("( %06d ) LFN: %-80s   Size[Bytes]: %10s   GUID: %s\n",i,GetKey(i,"lfn"),GetKey(i,"size"),GetKey(i,"guid"));
      //       } else {
      //  printf("( %06d ) LFN: .../%-48s   Size[Bytes]: %10s   GUID: %s\n",i,gSystem->BaseName(GetKey(i,"lfn")),GetKey(i,"size"),GetKey(i,"guid"));
      //       }
      //       if (GetKey(i, "size")) {
      //  totaldata += atol(GetKey(i,"size"));
      //  totalfiles++;
      //       }
      //     }
      //     printf("------------------------------------------------------------\n");
      //     printf("-> Result contains %.02f MB in %d Files.\n",totaldata/1024./1024.,totalfiles);
   } else {
      TIter next(this);
      TMap *map;
      for (Int_t count = 0; (map = dynamic_cast<TMap*>(next())); ++count) {
         TIter next2(map->GetTable());
         TPair *pair;
         std::cout << "# " << setiosflags(std::ios::left) << std::setw(6) << count << " ------------------------------------------------------------" << std::endl;
         while ((pair = dynamic_cast< TPair*>(next2()))) {
            TObjString * keyStr = dynamic_cast<TObjString*>(pair->Key());
            TObjString* valueStr = dynamic_cast<TObjString*>(pair->Value());
            if (keyStr && valueStr) {
               std::cout
               << std::setw(16) << keyStr->GetName()
               << valueStr->GetName()
               << std::endl;
            }
         }
         std::cout << std::endl;
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
///print the GLiteResult info

void TGLiteResult::Print(Option_t *option) const
{
   Print("", option);
}
