//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#if WITH_CLIENT_ONLY_CODE==1
//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)
#include "common.h"
#include "HomeKitServer.h"
#include "MainWindow.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "XLGZip.h"
#include "XLSocket.h"
#include "ScriptRemoconWeb.h"

#include "../Personal-HomeKit-HAP/PHKNetworkIP.h"
#include "../Personal-HomeKit-HAP/PHKAccessory.h"
#if _MSC_VER
	#pragma comment(lib, "Personal-HomeKit-HAP.lib")
#endif


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
HomeKitServer::HomeKitServer()
{
	this->HomeKitThread = NULL;
	this->StopFlag = false;
	this->HomekitUpdateSelf = false;
	this->PHKNetwork = NULL;

	struct _ref
	{
		static int logger(const char* msg,...)
		{
			va_list ap;
			vector<char> buffer(1024*4+1);

			va_start(ap,msg);
			vsnprintf(&buffer[0],1024*4, msg, ap);
			buffer[1024*4] = 0;
			va_end(ap);

			sexylog::m()->Echo(&buffer[0]);
			return 0;
		}
	};
	Personal_homekit_set_logger_function(_ref::logger);
}
HomeKitServer::~HomeKitServer()
{
	DEBUGLOG("stop...");

	Stop();

	DEBUGLOG("done");
}

bool HomeKitServer::Create()
{
	ASSERT(this->HomeKitThread == NULL);

	if ( ! MainWindow::m()->Config.GetBool("homekit_boot", false ) )
	{
		NOTIFYLOG("homekit server is disable");
		return false;
	}

	if (! SystemMisc::setup_mDNS() )
	{
		ERRORLOG("setup_mDNS is disable! can nt start homekit.");
		return false;
	}

	this->StopFlag = false;

	this->PinCode = MainWindow::m()->Config.Get("homekit_pin", "" );
	this->DeviceIdentity = MainWindow::m()->Config.Get("homekit_deviceidentity", "" );
	if (this->PinCode.empty() || this->DeviceIdentity.empty() )
	{
		RegeneratePinCode();
	}
	
	//upnpスレッド作成.
	NOTIFYLOG("homekit server...");
	this->HomeKitThread = new thread([=](){
		this->HomeKitThreadMain(); 
	});

	return true;
}

void HomeKitServer::Stop()
{
	this->StopFlag = true;

	if (this->HomeKitThread)
	{
		if (this->PHKNetwork)
		{
			NOTIFYLOG("closeAcceptConnection start");
			this->PHKNetwork->closeAcceptConnection();
			DEBUGLOG("closeAcceptConnection done");
		}
		NOTIFYLOG("HomeKitThread->join ...");
		this->HomeKitThread->join();
		DEBUGLOG("HomeKitThread->join done");

		delete this->PHKNetwork;
		this->PHKNetwork = NULL;

		delete this->HomeKitThread;
		this->HomeKitThread = NULL;

		NOTIFYLOG("HomeKitThread cleanup ...");
	}
}


void HomeKitServer::HomeKitThreadMain()
{
	BuildAccessory();

	const string controllerRecordsAddress = MainWindow::m()->GetConfigBasePath("homekit/controllerRecords.dat");

	this->PHKNetwork = new PHKNetworkIP("FHC",this->PinCode,this->DeviceIdentity,controllerRecordsAddress);
	while(! this->StopFlag )	
	{
		this->PHKNetwork->handleConnection();
	}
}

//新しいPINCODEを生成する.
string HomeKitServer::MakeNewPinCode() const
{
	int code[8];
	for(int i = 0 ; i < 8 ; i++)
	{
		code[i] = (xlrandom(9)) + 1;
	}
	char output[50];
	snprintf(output,50,"%1d%1d%1d-%1d%1d-%1d%1d%1d"
		,code[0]
		,code[1]
		,code[2]

		,code[3]
		,code[4]

		,code[5]
		,code[6]
		,code[7]
	);
	return output;
}

string HomeKitServer::MakeNewDeviceIdentity() const
{
	int code[2];
	for(int i = 0 ; i < 2 ; i++)
	{
		code[i] = (xlrandom(255));
	}
	char output[50];
	snprintf(output,50,"EC:22:3D:FC:%02X:%02X"
		,code[0]
		,code[1]
	);
	return output;
}


//PINコードの新規生成.
void HomeKitServer::RegeneratePinCode()
{
	//PINコード生成
	this->PinCode = this->MakeNewPinCode();
	MainWindow::m()->Config.Set("homekit_pin", this->PinCode );
	NOTIFYLOG("pinを生成しました " << this->PinCode);

	this->DeviceIdentity = this->MakeNewDeviceIdentity();
	MainWindow::m()->Config.Set("homekit_deviceidentity", this->DeviceIdentity );
	NOTIFYLOG("deviceidentityを生成しました " << this->DeviceIdentity);

	NOTIFYLOG("すでに登録されている コントローラー情報を消去しに行きます resetControllerAll");

	//すでに登録されているHomekit情報を破棄
	resetControllerAll();

	NOTIFYLOG("resetControllerAll done");
}

//アクセサリーの再読み込み
void HomeKitServer::ReloadAccessory() 
{
	if (this->PHKNetwork)
	{
		{
			AccessorySetAutoLock autolock;
			BuildAccessory() ;
		}
		updateConfiguration();
	}
}

class HomekitUpdateSelf_Auto
{
public:
	HomekitUpdateSelf_Auto()
	{
		MainWindow::m()->HomeKitServer.setHomekitUpdateSelf(true);
	}
	virtual ~HomekitUpdateSelf_Auto()
	{
		MainWindow::m()->HomeKitServer.setHomekitUpdateSelf(false);
	}
};


//電源制御bool
class boolCharacteristicsFHC: public boolCharacteristics {
	string elecName;
	string onActionName;
	string offActionName;
	string exclusiveType;
public:
	boolCharacteristicsFHC(const string& elecName
		,const string& onActionName
		,const string& offActionName
		,const string& exclusiveType
		,unsigned short _type
		,int _premission
		)
		: elecName(elecName)
		, onActionName(onActionName)
		, offActionName(offActionName)
		, exclusiveType(exclusiveType)
		, boolCharacteristics(_type, _premission) 
	{
	}

	bool isONStatus(const string& status) const
	{
		if (status != offActionName )
		{//OFFではない
			if (this->exclusiveType.empty() )
			{//特殊な指定がなければ OFF でないなら ON であるといえる.
				return true;
			}
			//冷房　暖房とかの特殊指定
			if ( status.find(this->exclusiveType) != string::npos )
			{//OFFではなく、 相方の 冷房 とかの文字列が入っている
				//相方が選択されているので、自分は OFF だろうと思われる.
				return false;
			}
			//自分が選択されているので ON だろうと思われる.
			return true;
		}
		return false;
	}

	void updateValue()
	{
		const string status = ScriptRemoconWeb::getElecStatus(this->elecName);
		this->_value = isONStatus(status);
	}

	virtual string value()
	{
		updateValue();
		return boolCharacteristics::value();
	}
	virtual void setValue(string str) {
		bool newValue = false;
		if (strncmp("true", str.c_str(), 4)==0 || strncmp("1", str.c_str(), 1)==0 )
		{
			newValue = true;
		}
		callback(newValue);
		updateValue();
	}
	virtual string describe(){
		updateValue();
		return boolCharacteristics::describe();
	}
	void callback(bool newValue)
	{
		const string status = ScriptRemoconWeb::getElecStatus(this->elecName);

		string useAction ;
		if (newValue)
		{
			if ( isONStatus(status) )
			{//すでにON
				return ;
			}
			useAction = onActionName;
		}
		else
		{
//OFFはもう一度送っても無害かなあー
//			if ( ! isONStatus(status) )
//			{//すでにOFF
//				return ;
//			}
			useAction = offActionName;

		}

		HomekitUpdateSelf_Auto al;
		MainWindow::m()->ScriptManager.FireElec(CallbackPP::NoCallback(),this->elecName,useAction);
	}
};

//数値
class intCharacteristicsFHC: public intCharacteristics {
	string elecName;
	vector<string> actionNames;
public:

	intCharacteristicsFHC(const string& elecName
		,const vector<string>& actionNames
		,unsigned short _type
		,int _premission
		)
		: elecName(elecName)
		, actionNames(actionNames)
		, intCharacteristics(_type, _premission,0 ,actionNames.size()-1, 1 ,unit_none)
	{
		ASSERT(!actionNames.empty());
	}

	int parseNowStatus(const string& status) const
	{
		int i = 0;
		for(auto it = this->actionNames.begin() ; it != this->actionNames.end() ; it++ ,i++ )
		{
			if ( *it == status )
			{
				return i;
			}
		}

		//見つからないので、とりあえず最少値にしておく.
		return 0;
	}

	void updateValue()
	{
		const string status = ScriptRemoconWeb::getElecStatus(this->elecName);

		this->_value =  parseNowStatus(status);
	}

	virtual string value()
	{
		updateValue();
		return intCharacteristics::value();
	}
	virtual void setValue(string str) {
        int newValue = atoi(str.c_str());

		callback(newValue);
		updateValue();
	}
	virtual string describe(){
		updateValue();
		return intCharacteristics::describe();
	}
	void callback(int newValue)
	{
		unsigned int i = newValue;

		if(i < 0 )
		{
			i = 0;
		}
		else if ( i >= this->actionNames.size() )
		{
			i = this->actionNames.size() - 1;
		}

		const string action = this->actionNames[i];
		HomekitUpdateSelf_Auto al;
		MainWindow::m()->ScriptManager.FireElec(CallbackPP::NoCallback(),this->elecName,action);
	}
};



//アクセサリーの状態変更通知
void HomeKitServer::NotifyAccessory()
{
	if (this->PHKNetwork)
	{
		if (this->HomekitUpdateSelf)
		{//Homekitからの状態変更の場合二重通知になるので通知しない
			return ;
		}
		updateConfiguration();
	}
}


//アクセサリーの構築
void HomeKitServer::BuildAccessory() 
{
	NOTIFYLOG("BuildAccessory...");
	currentDeviceType = deviceType_programmableSwitch;

	//アクセサリーはなぜかsingletonで実装されている。 謎だ.
	//すべてのアクセサリーの破棄
	AccessorySet::getInstance().RemoveALL();

	//ソート済みのIDを取得
	const auto configmap = MainWindow::m()->Config.FindGetsToMap("elec_",false);
	vector<unsigned int> arr = ScriptRemoconWeb::getElecID_Array(configmap,false);

	//Homekitのaidの仕様は必ず1から始まる連番である必要がある。
	//FHCだと、機材の並び順に描画した場合、ユーザーが機材を並び替えてしまう可能性がある。
	//そうなると、aidがかわってしまい Homekitでおかしなことがおきる.
	//このHomekitのクソな仕様で矛盾を最小にするには、 機材の並び順(order)ではなく、機材id(id)で並べなおす必要がある.
	//もちろん、機材の削除があったばあい、歯抜けが出るのがおかしなことになるが、
	//追加と並び替えでは矛盾は発生しない。
	sort(arr.begin(),arr.end());

	string jsonstring;
	for(auto it = arr.begin() ; it != arr.end() ; ++it )
	{
		const auto typeIT = configmap.find("elec_" + num2str(*it) + "_type");
		if (typeIT == configmap.end())
		{
			continue;
		}
		if (typeIT->second == "SPACE" )
		{//余白
			continue;
		}

//表示非表示でも、順番が変わるのはさすがにまずい。メニューから簡単にできるので
//しかたなので、家電が非表示でもHomekit側へは通知することにする.
//		const auto showremoconIT = configmap.find("elec_" + num2str(*it) + "_showremocon");
//		if (showremoconIT == configmap.end())
//		{
//			continue;
//		}
//		if (! stringbool(showremoconIT->second) )
//		{//非表示家電だったら表示しない
//			continue;
//		}

		//家電アクション一覧の取得
		unsigned int elecID = 0;
		vector<string> actions;
		const vector<unsigned int> actionARR = 
			ScriptRemoconWeb::getElecActionID_Array(configmap,typeIT->second,&elecID,false);
		for(auto actionIT = actionARR.begin() ; actionIT != actionARR.end() ; ++actionIT )
		{
			const auto actiontypeIT = configmap.find("elec_" + num2str(elecID)  + "_action_" + num2str(*actionIT) + "_actiontype");
			if (actiontypeIT == configmap.end())
			{
				continue;
			}
			if (actiontypeIT->second == "SPACE")
			{
				continue;
			}
			actions.push_back(actiontypeIT->second);
		}

		if (typeIT->second == "エアコン" )
		{
			AddOneAccessory(elecID,"エアコン",actions);
			AddOneAccessory(elecID,"暖房",actions);
		}
		else
		{
			AddOneAccessory(elecID,typeIT->second,actions);
		}
	}
}




//アクセサリーの追加
void HomeKitServer::AddOneAccessory(int elecID,const string& type,const vector<string>& actions) 
{
	struct _inner
	{
	static void selectAction(const vector<string>& actions,vector<string>* actionpicker,const string& actionname )
	{
		if ( find(actions.begin(),actions.end() , actionname) == actions.end())
		{
			return ;
		}
		actionpicker->push_back(actionname);
	}

	static string selectAction(const vector<string>& actions,const string& normalaction, const string& onaction)
	{
		for(auto it = actions.begin() ; it != actions.end() ; it++)
		{
			if ( it->find(normalaction) != string::npos )
			{
				return *it;
			}
		}
		return onaction;
	}
	};

	string postType = type;
	vector<string> actionpicker;

	string model;
	Service *service;

	string exclusiveType;

	const string offActionType = "けす";

	actionpicker.push_back(offActionType);
	_inner::selectAction(actions,&actionpicker,"最小");
	_inner::selectAction(actions,&actionpicker,"普通");
	_inner::selectAction(actions,&actionpicker,"最大");

	if (type == "照明")
	{
		service = new Service(serviceType_lightBulb);
		model = "Light";
	}
	else if (type == "エアコン")
	{
		service = new Service(serviceType_fan);		
		model = "Thermostat";

		if (actionpicker.size() <= 1)
		{
			_inner::selectAction(actions,&actionpicker,"冷房最小");
			_inner::selectAction(actions,&actionpicker,"冷房普通");
			_inner::selectAction(actions,&actionpicker,"冷房最大");
			exclusiveType = "暖房"; //相方の家電
		}
	}
	else if (type == "暖房")
	{
		service = new Service(serviceType_fan);		
		model = "Thermostat";
		postType = "エアコン";

		if (actionpicker.size() <= 1)
		{
			_inner::selectAction(actions,&actionpicker,"暖房最小");
			_inner::selectAction(actions,&actionpicker,"暖房普通");
			_inner::selectAction(actions,&actionpicker,"暖房最大");
			exclusiveType = "冷房"; //相方の家電
		}
	}
	else if (type == "テレビ")
	{
		service = new Service(serviceType_switch);
		model = "TV";
	}
	else if (type == "カーテン")
	{
		service = new Service(serviceType_switch);
		model = "curtain";
	}
	else if (type == "空気清浄機")
	{
		service = new Service(serviceType_switch);
			model = "Air cleaner";
	}
	else if (type == "コンポーザ")
	{
		service = new Service(serviceType_switch);
		model = "Composer";
	}
	else if (type == "ゲーム機")
	{
		service = new Service(serviceType_switch);
		model = "Game";
	}
	else if (type == "扇風機")
	{
		service = new Service(serviceType_fan);
		model = "Fan";
	}
	else if (type == "衛星放送")
	{
		service = new Service(serviceType_switch);
		model = "broadcast tuner";
	}
	else if (type == "ディスプレイ")
	{
		service = new Service(serviceType_switch);
		model = "Display";
	}
	else if (type == "レコーダ")
	{
		service = new Service(serviceType_switch);
		model = "Recoder";
	}
	else if (type == "ドア")
	{
		service = new Service(serviceType_switch);
		model = "Door";
		return ;
	}
	else if (type == "パソコン")
	{
		service = new Service(serviceType_switch);
		model = "PC";
	}
	else if (type == "プロジェクタ")
	{
		service = new Service(serviceType_switch);
		model = "Projector";
	}
	else if (type == "ファン")
	{
		service = new Service(serviceType_fan);
		model = "Fan";
	}
	else if (type == "窓")
	{
		service = new Service(serviceType_switch);
		model = "Window";
	}
	else
	{
		if(!XLStringUtil::checkSafePath(type))
		{
			ERRORLOG("家電 type: " << type << " は、危険文字を含むので登録できません");
			return ;
		}
		
		service = new Service(serviceType_switch);
		model = "Other";
	}

	Accessory *acc = new Accessory();
	addInfoServiceToAccessory(acc, _A2U(type), "FHC", _A2U(model), "FHCx"+num2str(elecID), NULL);
	//アクセサリー登録.
	AccessorySet::getInstance().addAccessory(acc);

	acc->addService(service);

	stringCharacteristics *serviceName = new stringCharacteristics(charType_serviceName, premission_read, 0);
	serviceName->setValue(_A2U(type));
	acc->addCharacteristics(service, serviceName);

	const string onActionType = _inner::selectAction(actionpicker,"普通","つける");

	boolCharacteristicsFHC *powerState = new boolCharacteristicsFHC(postType,onActionType,offActionType,exclusiveType,charType_on, premission_read|premission_write|premission_notify);
	acc->addCharacteristics(service, powerState);

	NOTIFYLOG(" Homekit add " << type );

	if (actionpicker.size() <= 1)
	{
		return;
	}

	if (service->uuid == serviceType_lightBulb)
	{
		intCharacteristicsFHC *a = new intCharacteristicsFHC(postType
			,actionpicker
			,charType_brightness
			,premission_read|premission_write|premission_notify);
		acc->addCharacteristics(service, a);
	}
	else if (service->uuid == serviceType_fan)
	{
		intCharacteristicsFHC *a = new intCharacteristicsFHC(postType
			,actionpicker
			,charType_rotationSpeed
			,premission_read|premission_write|premission_notify);
		acc->addCharacteristics(service, a);
	}


	return ;
}

//Homekit内で家電状態を変えたときに、ブロードキャストしないようにバリアを立てる
void HomeKitServer::setHomekitUpdateSelf(bool flag)
{
	this->HomekitUpdateSelf = flag;
}

#endif