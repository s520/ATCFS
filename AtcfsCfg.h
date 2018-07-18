//
// Generated on 2018/07/09 by inimoni 
//

/*
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Documentation
  Struct = Section name, Field = Key name.
  The function used is only a declaration, load(), and save().
  The constructor automatically reads initialization file [initFileName].
  Please rewrite [initFileName] in the source freely.
  It becomes Windows folder when there is not path. The relative path can be used.

Example
#include "SampleIni.h"
void CTestDlg::OnButton1()
{
    //initFileName = "./sample.ini";
    SampleIni data1;                   // Open ./sample.ini
    int v = data1.Sample.value;        // [Sample]-value Get
    data1.Sample.value = v+1;          // [Sample]-value Modify
    data1.save();                      // Save ./Sample.ini (Opened file)

    SampleIni data2("sample.ini");     // Open C:/WINDOWS/Sample.ini
    data2.load("./sample.ini");        // Open Specified file.
    data2.save();                      // Save Opened file (Making at not being)
}

Supplementation
  Reading and writing becomes the unit of the file.
  [initFileName] is the same as the argument of GetPrivateProfileString() of SDK.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
*/

#ifndef ATCFSCFG_H
#define ATCFSCFG_H

#include <string>
using namespace std;

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#ifdef _UNICODE
	typedef std::wstring string_t;
#else
	typedef std::string string_t;
#endif

namespace inimoni{
    template<class T>
    static bool inirw(int is_read, string_t& fname, string_t sec, string_t key, T& val_t);
};

/**
    atcfs.cfg input output class.
*/
class AtcfsCfg
{
public:                                                                 
                                                                        
    AtcfsCfg(string_t fname=_T(""))                          
    {                                                                   
        initFileName = _T("./atcfs.cfg"); //Default file.    
        init();                                                         
        load(fname);                                                    
    }                                                                   
                                                                        
    /**                                                                 
        INI file is read.                                               
        @param fname Filename                                           
        When there is not a file, It becomes initFileName.              
        When there is not a pass, It becomes Windows folder.            
    */                                                                  
    bool load(string_t fname=_T(""))                                  
    {                                                                   
        if(fname.empty()){                                              
            fname = initFileName;                                       
        }                                                               
        loadFileName = fname;                                           
        WIN32_FIND_DATA fd;                                             
        HANDLE h = ::FindFirstFile(fname.c_str(), &fd);                 
        if (h != INVALID_HANDLE_VALUE) {                                
            iniRW(fname,1);                                             
        }                                                               
        return (h != INVALID_HANDLE_VALUE);                             
    }                                                                   
                                                                        
    /**                                                                 
        It writes it in the INI file.                                   
        @param fname Filename                                           
        When there is not a file, It becomes open file.                 
        When there is not a pass, It becomes Windows folder.            
    */                                                                  
    bool save(string_t fname=_T(""))                                  
    {                                                                   
        if(fname.empty()){                                              
            fname = loadFileName;                                       
        }                                                               
        iniRW(fname,0);                                                 
        return true;                                                    
    }                                                                   

public:                                                                 

    // Emulate
    struct _Emulate
    {
        int      MaxDeceleration;
        int      LeverDelay;
    } Emulate;

    // Wiper
    struct _Wiper
    {
        int      WiperRate;
        int      WiperHoldPosition;
        int      WiperDelay;
        int      WiperSoundBehaviour;
        int      WiperWet;
    } Wiper;

    // ATC
    struct _ATC
    {
        int      AtcUse;
        int      AtcType;
        int      AtcMax;
        int      AtcSpeed3;
        int      AtcSpeed4;
        int      AtcSpeed5;
        int      AtcSpeed6;
        int      AtcSpeed7;
        int      AtcSpeed8;
        int      AtcSpeed9;
        int      AtcSpeed10;
        int      AtcSpeed11;
        int      AtcDeceleration1;
        int      AtcDeceleration2;
        int      AtcDeceleration3;
        int      AtcDeceleration4;
        int      Xe;
        int      Xo;
        int      Xk;
    } ATC;

    // ATS_P
    struct _ATS_P
    {
        int      AtspUse;
        int      AtspMax;
        int      AtspDeceleration;
    } ATS_P;

protected:

    string_t initFileName;
    string_t loadFileName;

    bool iniRW(string_t f, int r)    
    {                                
        string_t s;                  

        s = _T("Emulate");
        inimoni::inirw( r,f,s, _T("MaxDeceleration  "), Emulate.MaxDeceleration );
        inimoni::inirw( r,f,s, _T("LeverDelay       "), Emulate.LeverDelay );

        s = _T("Wiper");
        inimoni::inirw( r,f,s, _T("WiperRate        "), Wiper.WiperRate    );
        inimoni::inirw( r,f,s, _T("WiperHoldPosition"), Wiper.WiperHoldPosition );
        inimoni::inirw( r,f,s, _T("WiperDelay       "), Wiper.WiperDelay   );
        inimoni::inirw( r,f,s, _T("WiperSoundBehaviour"), Wiper.WiperSoundBehaviour );
        inimoni::inirw( r,f,s, _T("WiperWet         "), Wiper.WiperWet     );

        s = _T("ATC");
        inimoni::inirw( r,f,s, _T("AtcUse           "), ATC.AtcUse         );
        inimoni::inirw( r,f,s, _T("AtcType          "), ATC.AtcType        );
        inimoni::inirw( r,f,s, _T("AtcMax           "), ATC.AtcMax         );
        inimoni::inirw( r,f,s, _T("AtcSpeed3        "), ATC.AtcSpeed3      );
        inimoni::inirw( r,f,s, _T("AtcSpeed4        "), ATC.AtcSpeed4      );
        inimoni::inirw( r,f,s, _T("AtcSpeed5        "), ATC.AtcSpeed5      );
        inimoni::inirw( r,f,s, _T("AtcSpeed6        "), ATC.AtcSpeed6      );
        inimoni::inirw( r,f,s, _T("AtcSpeed7        "), ATC.AtcSpeed7      );
        inimoni::inirw( r,f,s, _T("AtcSpeed8        "), ATC.AtcSpeed8      );
        inimoni::inirw( r,f,s, _T("AtcSpeed9        "), ATC.AtcSpeed9      );
        inimoni::inirw( r,f,s, _T("AtcSpeed10       "), ATC.AtcSpeed10     );
        inimoni::inirw( r,f,s, _T("AtcSpeed11       "), ATC.AtcSpeed11     );
        inimoni::inirw( r,f,s, _T("AtcDeceleration1 "), ATC.AtcDeceleration1 );
        inimoni::inirw( r,f,s, _T("AtcDeceleration2 "), ATC.AtcDeceleration2 );
        inimoni::inirw( r,f,s, _T("AtcDeceleration3 "), ATC.AtcDeceleration3 );
        inimoni::inirw( r,f,s, _T("AtcDeceleration4 "), ATC.AtcDeceleration4 );
        inimoni::inirw( r,f,s, _T("Xe               "), ATC.Xe             );
        inimoni::inirw( r,f,s, _T("Xo               "), ATC.Xo             );
        inimoni::inirw( r,f,s, _T("Xk               "), ATC.Xk             );

        s = _T("ATS_P");
        inimoni::inirw( r,f,s, _T("AtspUse          "), ATS_P.AtspUse      );
        inimoni::inirw( r,f,s, _T("AtspMax          "), ATS_P.AtspMax      );
        inimoni::inirw( r,f,s, _T("AtspDeceleration "), ATS_P.AtspDeceleration );
        return true;                                                    
    }                                                                   

    void init()                                                         
    {                                                                   
        Emulate.MaxDeceleration      = 2700;
        Emulate.LeverDelay           = 250;
        Wiper.WiperRate              = 700;
        Wiper.WiperHoldPosition      = 0;
        Wiper.WiperDelay             = 700;
        Wiper.WiperSoundBehaviour    = 0;
        Wiper.WiperWet               = 0;
        ATC.AtcUse                   = 1;
        ATC.AtcType                  = 0;
        ATC.AtcMax                   = 220;
        ATC.AtcSpeed3                = 120;
        ATC.AtcSpeed4                = 170;
        ATC.AtcSpeed5                = 220;
        ATC.AtcSpeed6                = 230;
        ATC.AtcSpeed7                = 255;
        ATC.AtcSpeed8                = 270;
        ATC.AtcSpeed9                = 275;
        ATC.AtcSpeed10               = 285;
        ATC.AtcSpeed11               = 300;
        ATC.AtcDeceleration1         = 1500;
        ATC.AtcDeceleration2         = 1900;
        ATC.AtcDeceleration3         = 2400;
        ATC.AtcDeceleration4         = 2600;
        ATC.Xe                       = 5000;
        ATC.Xo                       = 10000;
        ATC.Xk                       = 0;
        ATS_P.AtspUse                = 0;
        ATS_P.AtspMax                = 140;
        ATS_P.AtspDeceleration       = 2600;
    }                                                                   
};
typedef AtcfsCfg AtcfsFile; //新旧互換

//---------------------------------------------------------------------------
// Common method                                                             
//---------------------------------------------------------------------------
#ifndef INIMONI_INIRW                                                        
#define INIMONI_INIRW                                                        
namespace inimoni                                                            
{                                                                            
    /*                                                                       
    Read and Write INI file                                                  
      int     is_read  1=Read mode, 0=Write mode                             
      string  fname    Filename (The Windows folder when there is not path)  
      string  sec      Section name                                          
      string  key      Key name                                              
      T       val_t    [Read]Init+Output, [Write]Input                       
    */                                                                       
    template<class T>                                                        
    bool inirw(int is_read, string_t& fname, string_t sec, string_t key, T& val_t)
	{                                                                        
		if(is_read){                                                         
            inimoni::read(fname.c_str(), sec.c_str(), key.c_str(), val_t);   
        }                                                                    
        else{                                                                
			inimoni::write(fname.c_str(), sec.c_str(), key.c_str(), val_t);  
        }                                                                    
		return true;                                                         
    }                                                                        
                                                                             
    bool read(string_t ifn, string_t sec, string_t key, int& dst)            
    {                                                                        
        dst = GetPrivateProfileInt( sec.c_str(), key.c_str(), dst, ifn.c_str() );
        return true;                                                         
    }                                                                        
                                                                             
    bool read(string_t ifn, string_t sec, string_t key, basic_string<TCHAR>& dst)
    {                                                                        
        TCHAR buf[256];                                                      
        GetPrivateProfileString(                                             
            sec.c_str(),                                                     
            key.c_str(),                                                     
            dst.c_str(),                                                     
            buf,                                                             
            sizeof(buf),                                                     
            ifn.c_str() );                                                   
        dst = buf;                                                           
        return true;                                                         
    }                                                                        
                                                                             
    bool read(string_t ifn, string_t sec, string_t key, double& dst)         
    {                                                                        
        string_t s;                                                          
        inimoni::read(ifn, sec, key, s);                                     
                                                                             
        TCHAR* e;                                                            
        double x = _tcstod(s.c_str(), &e);                                   
                                                                             
        dst = 0.0;                                                           
        if (!*e) {                                                           
            dst = x;                                                         
        }                                                                    
        return true;                                                         
    }                                                                        
                                                                             
    template<class T>                                                        
    bool write(string_t ifn, string_t sec, string_t key, T val_t)            
    {                                                                        
        TCHAR val[1024];                                                     
        inimoni::to_string(val, val_t);                                      
        WritePrivateProfileString( sec.c_str(), key.c_str(), val, ifn.c_str() );
		return true;                                                    
	}                                                                   
                                                                        
    void to_string(TCHAR* str, int val)                                 
    {                                                                   
        _stprintf( str, _T("%d"), val );                                
    }                                                                   
                                                                        
    void to_string(TCHAR* str, double val)                              
    {                                                                   
        _stprintf( str, _T("%f"), val );                                
    }                                                                   
                                                                        
    void to_string(TCHAR* str, basic_string<TCHAR> val)                 
    {                                                                   
        _stprintf( str, _T("%s"), val.c_str() );                        
    }                                                                   
};                                                                      
                                                                        
#endif                                                                  
#endif                                                                  

