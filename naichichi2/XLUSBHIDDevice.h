#pragma once
#if _WITH_USB

///linuxとwindows両対応なUSBコントロールクラス
//結局使わなかったけどな

#if _MSC_VER
	#include "XLLoadLibraryHelper.h"
#else
struct usb_bus;
struct usb_device;
struct usb_dev_handle;
#endif

class XLUSBHIDDevice
{
public:
	XLUSBHIDDevice();
	virtual ~XLUSBHIDDevice() ;

	//iVendorID iProductID で利用するUSBデバイスを特定し、ハンドルを開きます。
	virtual void Open(int iVendorID, int iProductID) ;

	virtual void SetFeature(unsigned char* data,int len) ;
	virtual void GetFeature(unsigned char* data,int len) ;

	virtual int Write(unsigned char* data,int len,unsigned int timeoutMS) ;
	virtual int Read(unsigned char* data,int len,unsigned int timeoutMS) ;

private:
#if _MSC_VER
		HANDLE Handle;
		XLLoadLibraryHelper HibLib;

		typedef struct _HIDD_ATTRIBUTES {
			ULONG   Size; // = sizeof (struct _HIDD_ATTRIBUTES)

			//
			// Vendor ids of this hid device
			//
			USHORT  VendorID;
			USHORT  ProductID;
			USHORT  VersionNumber;

			//
			// Additional fields will be added to the end of this structure.
			//
		} HIDD_ATTRIBUTES, *PHIDD_ATTRIBUTES;

		typedef void (__stdcall *_HidD_GetHidGuidDef) (OUT LPGUID   HidGuid);
		typedef BOOLEAN (__stdcall *_HidD_GetAttributesDef) (IN  HANDLE              HidDeviceObject,OUT PHIDD_ATTRIBUTES    Attributes);
		typedef BOOLEAN (__stdcall *_HidD_GetFeatureDef) (IN    HANDLE   HidDeviceObject, OUT   PVOID    ReportBuffer, IN    ULONG    ReportBufferLength);
		typedef BOOLEAN (__stdcall *_HidD_SetFeatureDef) (IN    HANDLE   HidDeviceObject, IN    PVOID    ReportBuffer, IN    ULONG    ReportBufferLength);

		_HidD_GetHidGuidDef _HidD_GetHidGuid;
		_HidD_GetAttributesDef _HidD_GetAttributes;
		_HidD_GetFeatureDef _HidD_GetFeature;
		_HidD_SetFeatureDef _HidD_SetFeature;
	
		
		

#else   
		struct usb_bus *Bus;
		struct usb_device *Dev;
		struct usb_dev_handle *Handle;

		unsigned int ReadEndPoint;
		unsigned int WriteEndPoint;
		unsigned int ReadEndPointMaxWriteSize;
		unsigned int WriteEndPointMaxWriteSize;
#endif

};
#endif //_WITH_USB
