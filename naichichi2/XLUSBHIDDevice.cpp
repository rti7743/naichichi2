#if _WITH_USB
//MITライセンスでライセンスされています
#include "common.h"
#include "XLUSBHIDDevice.h"

#if _MSC_VER
	#include <setupapi.h>
	#pragma comment(lib, "setupapi.lib")

#else
	//参考
	//http://kentai-shiroma.blogspot.jp/2012/06/usbptu2f3linux.html?showComment=1351979073666#c8439821432021535316
	#include <usb.h>
#endif

XLUSBHIDDevice::XLUSBHIDDevice()
{
#if _MSC_VER
	this->Handle = NULL;

	this->HibLib.Load("hid.dll");
	this->_HidD_GetHidGuid = (_HidD_GetHidGuidDef) this->HibLib.GetProcAddress("HidD_GetHidGuid");
	this->_HidD_GetAttributes = (_HidD_GetAttributesDef)this->HibLib.GetProcAddress("HidD_GetAttributes");
	this->_HidD_GetFeature = (_HidD_GetFeatureDef)this->HibLib.GetProcAddress("HidD_GetFeature");
	this->_HidD_SetFeature = (_HidD_SetFeatureDef)this->HibLib.GetProcAddress("HidD_SetFeature");
#else
	this->Bus = NULL;
	this->Dev = NULL;
	this->Handle = NULL;

	this->ReadEndPoint = 0;
	this->WriteEndPoint = 0;
	this->ReadEndPointMaxWriteSize = 0;
	this->WriteEndPointMaxWriteSize = 0;
#endif
}
XLUSBHIDDevice::~XLUSBHIDDevice()
{
#if _MSC_VER
	if (this->Handle)
	{
		CloseHandle(this->Handle);
	}
#else
	if (this->Handle)
	{
		usb_release_interface(this->Handle, 0);
		usb_close(this->Handle);
	}
#endif
}


//iVendorID iProductID で利用するUSBデバイスを特定し、ハンドルを開きます。
void XLUSBHIDDevice::Open(int iVendorID, int iProductID)
{
#if _MSC_VER
	GUID hidGuid;
	this->_HidD_GetHidGuid(&hidGuid);
	 
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&hidGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (hDevInfo == INVALID_HANDLE_VALUE )
	{
		throw XLEXCEPTION("SetupDiGetClassDevsに失敗");
	}

	for(unsigned short i = 0; i < 128;i++)
	{
		SP_DEVICE_INTERFACE_DATA spDid = { 0 } ;
		spDid.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		if(!SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &hidGuid, i, &spDid)) continue;
		DWORD dwRequiredLength = 0;
		SetupDiGetDeviceInterfaceDetail(hDevInfo, &spDid, NULL, 0, &dwRequiredLength, NULL);

		vector<char> buffer(dwRequiredLength);
		PSP_DEVICE_INTERFACE_DETAIL_DATA pspDidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)&buffer[0];
		pspDidd->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		if(!SetupDiGetDeviceInterfaceDetail(hDevInfo, &spDid, pspDidd, dwRequiredLength, &dwRequiredLength, NULL))
		{
			continue;
		}
		//0x00916b74 "\\?\hid#vid_0711&pid_0028#8&24d77392&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}"
		const char * vid = strstr(pspDidd->DevicePath,"vid_");
		if (vid == NULL)
		{
			continue;
		}
		vid += sizeof("vid_") - 1;  
		const char * pid = strstr(vid,"pid_");
		if (pid == NULL)
		{
			continue;
		}
		pid += sizeof("pid_") - 1;  

		const char * type = strstr(pid,"#");
		if (type == NULL)
		{
			continue;
		}
		type += sizeof("#") - 1;  

		char* e;
		int vecndorID = strtoul( vid ,&e , 16); 
		int productID = strtoul( pid ,&e , 16); 
//		int typeID = strtoul( type ,&e , 16); 

		if ( vecndorID != iVendorID || productID != iProductID)
		{
			continue;
		}

		HANDLE hDevHandle = CreateFile(pspDidd->DevicePath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if(hDevHandle == INVALID_HANDLE_VALUE)
		{
			continue;
		}

		//見つかった!
		this->Handle = hDevHandle;
		break;
	}

	if (this->Handle == NULL )
	{
		throw XLEXCEPTION("USB デバイスがありません VendorID:" << iVendorID << "ProductID:" << iProductID);
	}

#else
	//準備
	usb_init();
	usb_find_busses();
	usb_find_devices();
	this->Bus = usb_get_busses();

	//USBデバイスを検索します
	for(auto bus=this->Bus; bus; bus=bus->next)
	{
		for(auto dev=bus->devices; dev; dev=dev->next) 
		{
			if( (dev->descriptor.idVendor==iVendorID) && (dev->descriptor.idProduct==iProductID) )
			{
				this->Dev = dev;
				break;
			}
		}
	}
	if (!this->Dev)
	{
		throw XLEXCEPTION("USB デバイスがありません VendorID:" << iVendorID << "ProductID:" << iProductID);
	}

	//デバイスを開きます
	this->Handle = usb_open(this->Dev);
	if(! this->Handle )
	{
		throw XLEXCEPTION("usb_openに失敗:" << usb_strerror() << " VendorID:" << iVendorID << "ProductID:" << iProductID));
	}

	//オマジナイの儀式
	if( usb_set_configuration(this->Handle,this->Dev->config->bConfigurationValue)<0 )
	{
		if( usb_detach_kernel_driver_np(this->Handle,this->Dev->config->interface->altsetting->bInterfaceNumber)<0 )
		{
//			fprintf(stderr,"usb_set_configuration Error.\n");
//			fprintf(stderr,"usb_detach_kernel_driver_np Error.(%s)\n",usb_strerror());
			throw XLEXCEPTION("usb_set_configuration失敗後のusb_detach_kernel_driver_npに失敗:" << usb_strerror() << " VendorID:" << iVendorID << "ProductID:" << iProductID);
		}
	}

	if( usb_claim_interface(this->Handle,this->Dev->config->interface->altsetting->bInterfaceNumber)<0 )
	{
		if( usb_detach_kernel_driver_np(this->Handle,this->Dev->config->interface->altsetting->bInterfaceNumber)<0 )
		{
//			fprintf(stderr,"usb_claim_interface Error.\n");
//			fprintf(stderr,"usb_detach_kernel_driver_np Error.(%s)\n",usb_strerror());
			throw XLEXCEPTION("usb_claim_interface失敗後のusb_detach_kernel_driver_npに失敗:" << usb_strerror() << " VendorID:" << iVendorID << "ProductID:" << iProductID);
		}
	}

	if( usb_claim_interface(this->Handle,this->Dev->config->interface->altsetting->bInterfaceNumber)<0 )
	{
//		fprintf(stderr,"usb_claim_interface Error.(%s)\n",usb_strerror());
		throw XLEXCEPTION("usb_claim_interfaceに失敗:" << usb_strerror() << " VendorID:" + num2str(iVendorID) + "ProductID:" <<iProductID);
	}
	
	//エントリーポイントを検索します 複数あったら最後にあったやつをとりあえず採用するw
	auto config = this->Dev->config;
	for (unsigned int i = 0 ; i < config->bNumInterfaces ; ++ i)
	{
		auto inter = &(config->interface[i]);
		for(unsigned int j = 0; j < inter->num_altsetting; j++) {
			auto interdesc = &inter->altsetting[j];

			for(int k = 0; k < (int) interdesc->bNumEndpoints; k++) {
				auto endpoint = &interdesc->endpoint[k];

				unsigned int wmax = endpoint->wMaxPacketSize; //le16 little endianらしい？
				unsigned int maxsize = wmax & 0x7ff;  //一回に読み書き込める最大値

				//see
				//https://github.com/gregkh/usbutils/blob/master/lsusb.c
				//http://subversion.assembla.com/svn/cJmcWSaLSr3OBaeJe5aVNr/comport_linux_dcu60.cpp
				if (endpoint->bEndpointAddress & 0x80)
				{//読み込み IN
					this->ReadEndPoint = endpoint->bEndpointAddress ;
					this->ReadEndPointMaxWriteSize = maxsize;
				}
				else
				{//書き込み OUT
					this->WriteEndPoint = endpoint->bEndpointAddress ;
					this->WriteEndPointMaxWriteSize = maxsize;
				}
			}
		}
	}
	
	printf("read %d %d\r\nwrite %d %d\r\n",this->ReadEndPoint,this->ReadEndPointMaxWriteSize,this->WriteEndPoint,this->WriteEndPointMaxWriteSize);
	if (this->ReadEndPointMaxWriteSize <= 0)
	{
		throw XLEXCEPTION("ReadEndPointがないです VendorID:" << iVendorID << "ProductID:" <<iProductID));
	}
	if (this->WriteEndPointMaxWriteSize <= 0)
	{
		throw XLEXCEPTION("WriteEndPointがないです VendorID:" << iVendorID << "ProductID:" << iProductID);
	}
#endif

}

// HID Class-Specific Requests values. See section 7.2 of the HID specifications 
#define HID_GET_REPORT                0x01 
#define HID_GET_IDLE                  0x02 
#define HID_GET_PROTOCOL              0x03 
#define HID_SET_REPORT                0x09 
#define HID_SET_IDLE                  0x0A 
#define HID_SET_PROTOCOL              0x0B 
#define HID_REPORT_TYPE_INPUT         0x01 
#define HID_REPORT_TYPE_OUTPUT        0x02 
#define HID_REPORT_TYPE_FEATURE       0x03 

#define CTRL_IN                (LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE)
#define CTRL_OUT		       (LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE)

void XLUSBHIDDevice::SetFeature(unsigned char* data,int len) 
{
#if _MSC_VER
	_HidD_SetFeature(this->Handle,data,len);
#else
//	int ret = usb_control_transfer(this->Handle,CTRL_OUT ,HID_GET_REPORT,(HID_REPORT_TYPE_FEATURE<<8)|0x00,0, data,len, 100000); 
//	if (ret < 0)
//	{
//		throw XLEXCEPTION("libusb_control_transfer書き込みエラー:" << ret);
//	}
#endif
}

void XLUSBHIDDevice::GetFeature(unsigned char* data,int len) 
{
#if _MSC_VER
	_HidD_GetFeature(this->Handle,data,len);
#else
//	int ret = usb_control_transfer(this->Handle,CTRL_IN,HID_SET_REPORT,(HID_REPORT_TYPE_FEATURE<<8)|0x00, 0,data,len, 100000);  
//	if (ret < 0)
//	{
//		throw XLEXCEPTION("libusb_control_transfer読み込みエラー:" << ret);
//	}
#endif
}



int XLUSBHIDDevice::Write(unsigned char* data,int len,unsigned int timeoutMS) 
{
#if _MSC_VER
	OVERLAPPED ol = {0};
	ol.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	DWORD r ;
	if (! WriteFile(this->Handle,data,len,&r,&ol) )
	{
		DWORD lasterror = GetLastError();
		if (lasterror != ERROR_IO_PENDING)
		{
			CloseHandle(ol.hEvent);
			throw XLEXCEPTION("WriteFileに失敗 " << XLException::StringWindows(lasterror)  );
		}
		WaitForSingleObject(ol.hEvent,timeoutMS);
		if (!GetOverlappedResult( this->Handle, &ol, &r, FALSE ))
		{//timeout
			CancelIoEx(this->Handle,&ol);
			CloseHandle(ol.hEvent);
			throw XLEXCEPTION("WriteFileに失敗 タイムアウト");
		}
	}

	CloseHandle(ol.hEvent);
	return r;
#else
	int ret=usb_bulk_write(this->Handle, this->WriteEndPoint, (char*)data, len, timeoutMS);
	if (ret < 0)
	{
		throw XLEXCEPTION("usb_bulk_writeに失敗:" << ret<< " :" << usb_strerror() );
	}
	return ret;
#endif
}

int XLUSBHIDDevice::Read(unsigned char* data,int len,unsigned int timeoutMS) 
{
#if _MSC_VER
	OVERLAPPED ol = {0};
	ol.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	DWORD r ;
	if (! ReadFile(this->Handle,data,len,&r,&ol) )
	{
		DWORD lasterror = GetLastError();
		if (lasterror != ERROR_IO_PENDING)
		{
			CloseHandle(ol.hEvent);
			throw XLEXCEPTION("ReadFileに失敗" << XLException::StringWindows(lasterror) );
		}
		WaitForSingleObject(ol.hEvent,timeoutMS);
		if (!GetOverlappedResult( this->Handle, &ol, &r, FALSE ))
		{//timeout
			CancelIoEx(this->Handle,&ol);
			CloseHandle(ol.hEvent);
			throw XLEXCEPTION("ReadFileに失敗 timeout" );
		}
	}

	CloseHandle(ol.hEvent);
	return r;
#else
	int ret=usb_bulk_read(this->Handle, this->ReadEndPoint,(char*) data, len, timeoutMS);
	if (ret < 0)
	{
		throw XLEXCEPTION("usb_bulk_readに失敗:" << ret << " :" << usb_strerror() );
	}
	return ret;
#endif
}
#endif //_WITH_USB
