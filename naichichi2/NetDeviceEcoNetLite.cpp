//GPLでライセンスされています。(他のライセンス形態を望むならばお問い合わせください)

#include "common.h"
#include "NetDeviceEcoNetLite.h"
#include "MainWindow.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "XLGZip.h"
#include "SystemMisc.h"
#include "NetDevice.h"


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

string NetDeviceEcoNetLite::MakeActionName(const EcoNetLiteObjCode& deoj,const unsigned char& prop)
{
	char name[512] = {0};
	snprintf(name,512,"(#%02x)",prop);
	return CodeToActionNameDisp(deoj,prop) + name;
}

string NetDeviceEcoNetLite::CodeToActionNameDisp(const EcoNetLiteObjCode& deoj,const unsigned char& prop)
{
	//全部網羅しません.
	//全部作ると大変なので、get/setができて使いそうなものだけ

	if (prop == 0x80)		return "動作状態";
	if (prop == 0x81)		return "設置場所";
	if (prop == 0x87)		return "電流制限設定";
	if (prop == 0x8F)		return "節電動作設定";
	if (prop == 0x93)		return "遠隔操作設定";
	if (prop == 0x97)		return "現在時刻";
	if (prop == 0x98)		return "現在年月日";
	if (prop == 0x99)		return "電力制限設定";

	if (deoj.code[0] == 0x01)
	{
		if (prop == 0xA0)		return "風量設定";
		if (prop == 0xA1)		return "風量自動設定";
		if (prop == 0xA3)		return "風向スイング設定";
		if (prop == 0xA4)		return "風向上下設定";
		if (prop == 0xA5)		return "風向左右設定";
		if (prop == 0xB0)		return "運転モード設定";
		if (prop == 0xB1)		return "温度自動設定";
		if (prop == 0xB2)		return "急速動作モード設定";
		if (prop == 0xB3)		return "温度設定値";
		if (prop == 0xB4)		return "除湿モード時温度設定値";
		if (prop == 0xB5)		return "冷房モード時温度設定値";
		if (prop == 0xB6)		return "暖房モード時温度設定値";
		if (prop == 0xB7)		return "除湿モード時温度設定値";

		if (deoj.code[0] == 0x30 )
		{//エアコン
			if (prop == 0xC0)	return "換気モード設定";
			if (prop == 0xC1)	return "加湿モード設定";
			if (prop == 0xC2)	return "換気風量設定";
			if (prop == 0xC4)	return "加湿量設定";
		}
		if (deoj.code[0] == 0x33 || deoj.code[0] == 0x34)
		{//換気扇
			if (prop == 0xBF)		return "換気自動設定";
		}
		if (deoj.code[0] == 0x39 )
		{//加湿器
			if (prop == 0xC0)		return "加湿設定1";
			if (prop == 0xC1)		return "加湿設定2";
		}
	}
	else if (deoj.code[0] == 0x02)
	{
		if (deoj.code[1] == 0x88)
		{//メーター
		}
		if (deoj.code[1] == 0x90 || deoj.code[1] == 0x91)
		{//照明
			if (prop == 0xB6)		return "点灯モード設定";
			if (prop == 0xB7)		return "通常灯モード時高度レベル";
			if (prop == 0xB8)		return "通常灯モード時高度段数";
			if (prop == 0xB9)		return "常夜灯モード時高度レベル";
			if (prop == 0xBA)		return "常夜灯モード時高度段数";
			if (prop == 0xBB)		return "通常灯モード時光色";
			if (prop == 0xBC)		return "通常灯モード時光色段数";
			if (prop == 0xBD)		return "常夜灯モード時光色";
			if (prop == 0xBE)		return "常夜灯モード時光色段数";
			if (prop == 0xBF)		return "自動モード時自灯モード";
		}
		if (deoj.code[1] == 0xA0)
		{//ブザー
			if (prop == 0xB1)		return "音発生設定";
			if (prop == 0xE0)		return "ブザー音種別";
		}
		if (deoj.code[1] == 0x67)
		{//散水機
			if (prop == 0xE0)		return "散水弁開閉";
		}
		if (deoj.code[1] == 0x60 || deoj.code[1] == 0x61 
		||  deoj.code[1] == 0x63 || deoj.code[1] == 0x64
		||  deoj.code[1] == 0x65 || deoj.code[1] == 0x66
		||  deoj.code[1] == 0x6F
		)
		{//ブラインド 電動シャッター 電子錠
			if (prop == 0xD0)		return "開速度";
			if (prop == 0xD1)		return "閉速度";
			if (prop == 0xE0)		return "開閉";
			if (prop == 0xE1)		return "開度レベル";
			if (prop == 0xE3)		return "開度速度";
			if (prop == 0xE5)		return "電気錠設定";
		}
	}
	else if (deoj.code[0] == 0x06)
	{
		if (prop == 0xB8)		return "音量";
		if (prop == 0xB9)		return "ミュート";
	}

	return "NAZO";
}

void NetDeviceEcoNetLite::GetValueTemp50(list<string>* outRet)
{
	char buf[30];
	for(int i = 0 ; i < 50 ; i++)
	{
		snprintf(buf,30,"%d℃(#%02x)",i,i);
		outRet->push_back(buf);
	}
}
void NetDeviceEcoNetLite::GetValueParsent100(list<string>* outRet)
{
	char buf[30];
	for(int i = 0 ; i < 100 ; i++)
	{
		snprintf(buf,30,"%d%%(#%02x)",i,i);
		outRet->push_back(buf);
	}
}
void NetDeviceEcoNetLite::GetLevel8(bool enableAuto41,list<string>* outRet)
{
	if (enableAuto41)
	{
		outRet->push_back("自動設定(#41)");
	}
	outRet->push_back("レベル1(#31)");
	outRet->push_back("レベル2(#32)");
	outRet->push_back("レベル3(#33)");
	outRet->push_back("レベル4(#34)");
	outRet->push_back("レベル5(#35)");
	outRet->push_back("レベル6(#36)");
	outRet->push_back("レベル7(#37)");
	outRet->push_back("レベル8(#38)");
}

bool NetDeviceEcoNetLite::GetValue(const EcoNetLiteObjCode& deoj,const unsigned char& prop,list<string>* outRet)
{
	//全部網羅しません.
	//全部作ると大変なので、get/setができて使いそうなものだけ

	if (prop == 0x80)
	{//	return "動作状態";
		outRet->push_back("ON(#30)");
		outRet->push_back("OFF(#31)");
		return true;
	}
	if (prop == 0x8F)
	{//return "節電動作設定";
		outRet->push_back("節電動作中(#41)");
		outRet->push_back("通常動作中(#42)");
		return true;
	}

	if (deoj.code[0] == 0x01)
	{
		if (prop == 0xA0)
		{//return "風量設定";
			GetLevel8(true,outRet);
			return true;
		}
		if (prop == 0xA1)
		{//return "風量自動設定";
			outRet->push_back("AUTO(#41)");
			outRet->push_back("非AUTO(#42)");
			outRet->push_back("上下(#43)");
			outRet->push_back("左右(#44)");
			return true;
		}
		if (prop == 0xA3)
		{//return "風向スイング設定";
			outRet->push_back("風向スイングOFF(#31)");
			outRet->push_back("上下(#41)");
			outRet->push_back("左右(#42)");
			outRet->push_back("上下左右(#43)");
			return true;
		}
		if (prop == 0xA4)
		{//return "風向上下設定";
			outRet->push_back("上(#41)");
			outRet->push_back("下(#42)");
			outRet->push_back("中央(#43)");
			outRet->push_back("上中(#44)");
			outRet->push_back("下中(#45)");
			return true;
		}
		if (prop == 0xA5)
		{//return "風向左右設定";
			outRet->push_back("右(#41)");
			outRet->push_back("左(#42)");
			outRet->push_back("中央(#43)");
			outRet->push_back("左右(#44)");
			//etc.
			return true;
		}
		if (prop == 0xB0)
		{//return "運転モード設定";
			outRet->push_back("自動(#41)");
			outRet->push_back("冷房(#42)");
			outRet->push_back("暖房(#43)");
			outRet->push_back("除湿(#44)");
			outRet->push_back("送風(#45)");
			outRet->push_back("その他(#40)");
			return true;
		}
		if (prop == 0xB1)
		{//return "温度自動設定";
			outRet->push_back("AUTO(#41)");
			outRet->push_back("非AUTO(#42)");
			return true;
		}
		if (prop == 0xB2)
		{//return "急速動作モード設定";
			outRet->push_back("通常運転(#41)");
			outRet->push_back("急速運転(#42)");
			outRet->push_back("静音運転(#43)");
			return true;
		}
		if (prop == 0xB3)
		{//return "温度設定値";
			GetValueTemp50(outRet);
			return true;
		}
		if (prop == 0xB4)
		{//return "除湿モード時温度設定値";
			GetValueParsent100(outRet);
			return true;
		}
		if (prop == 0xB5)
		{//return "冷房モード時温度設定値";
			GetValueTemp50(outRet);
			return true;
		}
		if (prop == 0xB6)
		{//return "暖房モード時温度設定値";
			GetValueTemp50(outRet);
			return true;
		}
		if (prop == 0xB7)
		{//return "除湿モード時温度設定値";
			GetValueTemp50(outRet);
			return true;
		}
		if (deoj.code[0] == 0x30 )
		{
			if (prop == 0xC0)
			{//return "換気モード設定";
				outRet->push_back("換気ON排気方向(#41)");
				outRet->push_back("換気OFF(#42)");
				outRet->push_back("換気ON吸気方向(#43)");
				return true;
			}
			if (prop == 0xC1)
			{//return "加湿モード設定";
				outRet->push_back("ON(#41)");
				outRet->push_back("OFF(#42)");
			}
			if (prop == 0xC2)
			{//return "換気風量設定";
				GetLevel8(true,outRet);
			}
			if (prop == 0xC4)
			{//return "加湿量設定";
				GetLevel8(true,outRet);
			}
		}
		if (deoj.code[0] == 0x33 || deoj.code[0] == 0x34)
		{//換気扇
			if (prop == 0xBF)
			{//return "換気自動設定";
				outRet->push_back("AUTO(#41)");
				outRet->push_back("非AUTO(#42)");
				return true;
			}
		}
		if (deoj.code[0] == 0x39 )
		{
			if (prop == 0xC0)
			{//return "加湿設定1";
				GetValueParsent100(outRet);
				outRet->push_back("自動設定(#70)");
				outRet->push_back("連続運転(#71)");
				outRet->push_back("間欠運転(#72)");
				return true;
			}
			if (prop == 0xC1)
			{//return "加湿設定2";
				GetLevel8(false,outRet);
				outRet->push_back("自動設定(#70)");
				outRet->push_back("連続運転(#71)");
				outRet->push_back("間欠運転(#72)");
				return true;
			}
		}
	}
	else if (deoj.code[0] == 0x02)
	{
		if (deoj.code[1] == 0x90 || deoj.code[1] == 0x91)
		{//照明
			if (prop == 0xB6)
			{//return "点灯モード設定";
				outRet->push_back("自動(#41)");
				outRet->push_back("通常灯(#42)");
				outRet->push_back("常夜灯(#43)");
				outRet->push_back("カラー灯(#44)");
				return true;
			}
			if (prop == 0xB7)
			{//return "通常灯モード時高度レベル";
				GetValueParsent100(outRet);
				return true;
			}
			if (prop == 0xB8)
			{//return "通常灯モード時高度段数";
			}
			if (prop == 0xB9)
			{//return "常夜灯モード時高度レベル";
				GetValueParsent100(outRet);
				return true;
			}
			if (prop == 0xBA)
			{//return "常夜灯モード時高度段数";
			}
			if (prop == 0xBB)
			{//return "通常灯モード時光色";
				outRet->push_back("電球色(#41)");
				outRet->push_back("白色(#42)");
				outRet->push_back("昼白色(#43)");
				outRet->push_back("昼光色(#44)");
				outRet->push_back("その他(#40)");
				return true;
			}
			if (prop == 0xBC)
			{//return "通常灯モード時光色段数";
			}
			if (prop == 0xBD)
			{//	return "常夜灯モード時光色";
				outRet->push_back("電球色(#41)");
				outRet->push_back("白色(#42)");
				outRet->push_back("昼白色(#43)");
				outRet->push_back("昼光色(#44)");
				outRet->push_back("その他(#40)");
				return true;
			}
			if (prop == 0xBE)
			{//return "常夜灯モード時光色段数";
			}
			if (prop == 0xBF)
			{//return "自動モード時自灯モード";
				outRet->push_back("通常灯(#42)");
				outRet->push_back("常夜灯(#43)");
				outRet->push_back("消灯(#44)");
				outRet->push_back("カラー灯(#45)");
				return true;
			}
		}
		if (deoj.code[1] == 0xA0)
		{//ブザー
			if (prop == 0xB1)
			{//return "音発生設定";
				outRet->push_back("ON(#41)");
				outRet->push_back("OFF(#42)");
				return true;
			}
			if (prop == 0xE0)
			{//return "ブザー音種別";
				GetLevel8(false,outRet);
				return true;
			}
		}
		if (deoj.code[1] == 0x67)
		{//散水機
			if (prop == 0xE0)
			{//return "散水弁開閉";
				outRet->push_back("自動(#40)");
				outRet->push_back("手動ON(#41)");
				outRet->push_back("手動OFF(#42)");
				return true;
			}
		}

		if (deoj.code[1] == 0x60 || deoj.code[1] == 0x61 
		||  deoj.code[1] == 0x63 || deoj.code[1] == 0x64
		||  deoj.code[1] == 0x65 || deoj.code[1] == 0x66
		||  deoj.code[1] == 0x6F
		)
		{//ブラインド 電動シャッター 電子錠
			if (prop == 0xD0)
			{//return "開速度";
				outRet->push_back("低(#41)");
				outRet->push_back("中(#42)");
				outRet->push_back("高(#43)");
				return true;
			}
			if (prop == 0xD1)
			{//return "閉速度";
				outRet->push_back("低(#41)");
				outRet->push_back("中(#42)");
				outRet->push_back("高(#43)");
				return true;
			}
			if (prop == 0xE0)
			{//return "開閉";
				outRet->push_back("開(#41)");
				outRet->push_back("閉(#42)");
				outRet->push_back("停止(#43)");
				return true;
			}
			if (prop == 0xE1)
			{//return "開度レベル";
				GetValueParsent100(outRet);
				return true;
			}
			if (prop == 0xE3)
			{//return "開度速度";
				outRet->push_back("低(#41)");
				outRet->push_back("中(#42)");
				outRet->push_back("高(#43)");
				return true;
			}
			if (prop == 0xE5)
			{//return "電気錠設定";
				outRet->push_back("施錠(#41)");
				outRet->push_back("開錠(#42)");
				return true;
			}
		}
	}
	else if (deoj.code[0] == 0x06)
	{
		if (prop == 0xB8)
		{//return "音量";
			GetValueParsent100(outRet);
		}
		if (prop == 0xB9)
		{//return "ミュート";
			outRet->push_back("ミュートON(#30)");
			outRet->push_back("ミュートOFF(#31)");
			return true;
		}
	}

	return true;
}


string NetDeviceEcoNetLite::MakeName(const string& identification,const EcoNetLiteObjCode& deoj)
{
	return CodeToNameDisp(deoj) + "(enl://" + identification + ")";
}

string NetDeviceEcoNetLite::CodeToNameDisp(const EcoNetLiteObjCode& deoj)
{
	if (deoj.code[0] == 0x00)
	{
		switch(deoj.code[1])
		{
		case 0x01:	return "ガス漏れセンサ";
		case 0x02:	return "防犯センサ";
		case 0x03:	return "非常ボタン";
		case 0x04:	return "救急用センサ";
		case 0x05:	return "地震センサ";
		case 0x06:	return "漏電センサ";
		case 0x07:	return "人体検知センサ";
		case 0x08:	return "来客センサ";
		case 0x09:	return "呼び出しセンサ";
		case 0x0A:	return "結露センサ";
		case 0x0B:	return "空気汚染センサ";
		case 0x0C:	return "酸素センサ";
		case 0x0D:	return "照度センサ";
		case 0x0E:	return "音センサ";
		case 0x0F:	return "投函センサ";
		case 0x10:	return "重荷センサ";
		case 0x11:	return "温度センサ";
		case 0x12:	return "湿度センサ";
		case 0x13:	return "雨センサ";
		case 0x14:	return "水位センサ";
		case 0x15:	return "風呂水位センサ";
		case 0x16:	return "風呂沸き上がりセンサ";
		case 0x17:	return "水漏れセンサ";
		case 0x18:	return "水あふれセンサ";
		case 0x19:	return "火災センサ";
		case 0x1A:	return "タバコ煙センサ";
		case 0x1B:	return "ＣＯ２センサ";
		case 0x1C:	return "ガスセンサ";
		case 0x1D:	return "ＶＯＣセンサ";
		case 0x1E:	return "差圧センサ";
		case 0x1F:	return "風速センサ";
		case 0x20:	return "臭いセンサ";
		case 0x21:	return "炎センサ";
		case 0x22:	return "電力量センサ";
		case 0x23:	return "電流量センサ";
		case 0x24:	return "昼光センサ";
		case 0x25:	return "水流量センサ";
		case 0x26:	return "微動センサ";
		case 0x27:	return "通貨センサ";
		case 0x28:	return "在床センサ";
		case 0x29:	return "開閉センサ";
		case 0x2A:	return "活動量センサ";
		case 0x2B:	return "人体位置センサ";
		case 0x2C:	return "雪センサ";
		case 0x2D:	return "気圧センサ";
		}
	}
	else if (deoj.code[0] == 0x01)
	{
		switch(deoj.code[1])
		{
		case 0x30:	return "家庭用エアコン";
		case 0x31:	return "冷風機";
		case 0x32:	return "扇風機";
		case 0x33:	return "換気扇";
		case 0x34:	return "空調換気扇";
		case 0x35:	return "空気清浄機";
		case 0x36:	return "冷風扇";
		case 0x37:	return "サーキュレータ";
		case 0x38:	return "除湿機";
		case 0x39:	return "加湿器";
		case 0x3A:	return "天井扇";
		case 0x3B:	return "電気こたつ";
		case 0x3C:	return "電気あんか";
		case 0x3D:	return "電気毛布";
		case 0x3E:	return "ストーブ";
		case 0x3F:	return "パネルヒータ";
		case 0x40:	return "電気カーペット";
		case 0x41:	return "フロアヒータ";
		case 0x42:	return "電気暖房器";
		case 0x43:	return "ファンヒータ";
		case 0x44:	return "充電器";
		case 0x45:	return "業務用エアコン室内機";
		case 0x46:	return "業務用エアコン室外機";
		case 0x47:	return "業務用エアコン蓄熱ユニット";
		case 0x48:	return "業務用ファンコイルユニット";
		case 0x49:	return "業務用空調冷熱源";
		case 0x50:	return "業務用空調温熱源";
		case 0x51:	return "業務用空調";
		case 0x52:	return "業務用空調エアハンドリグ";
		case 0x53:	return "ユニットクーラ";
		case 0x54:	return "業務用コンデシグユニット";
		case 0x55:	return "電気蓄熱暖房器";
		}
	}
	else if (deoj.code[0] == 0x02)
	{
		switch(deoj.code[1])
		{
		case 0x60:	return "ブラインド";
		case 0x61:	return "シャッター";
		case 0x62:	return "カーテン";
		case 0x63:	return "雨戸・シャッター";
		case 0x64:	return "ゲート";
		case 0x65:	return "電動窓";
		case 0x66:	return "電動玄関ドア";
		case 0x67:	return "散水器（庭用）";
		case 0x68:	return "散水器（火災用）";
		case 0x69:	return "噴水";
		case 0x6A:	return "瞬間湯沸器";
		case 0x6B:	return "電気温水器";
		case 0x6C:	return "太陽熱温水器";
		case 0x6D:	return "循環ポンプ";
		case 0x6E:	return "電気便座";
		case 0x6F:	return "電気錠";
		case 0x70:	return "ガス元弁";
		case 0x71:	return "ホームサウナ";
		case 0x72:	return "瞬間式給湯器";
		case 0x73:	return "浴室暖房乾燥機";
		case 0x74:	return "ホームエレベータ";
		case 0x75:	return "電動間仕切り";
		case 0x76:	return "水平トランスファ";
		case 0x77:	return "電動物干し";
		case 0x78:	return "浄化槽";
		case 0x79:	return "住宅用太陽光発電";
		case 0x7A:	return "冷温水熱源機";
		case 0x7B:	return "床暖房";
		case 0x7C:	return "燃料電池";
		case 0x7D:	return "蓄電池";
		case 0x7E:	return "電気自動車充放電器";
		case 0x7F:	return "エンジンコージェネレーション";
		case 0x80:	return "電力量メータ";
		case 0x81:	return "水流量メータ";
		case 0x82:	return "ガスメータ";
		case 0x83:	return "LP ガスメータ";
		case 0x84:	return "時計";
		case 0x85:	return "自動ドア";
		case 0x86:	return "業務用エレベータ";
		case 0x87:	return "分電盤メータリング";
		case 0x88:	return "低圧スマート電力量メータ";
		case 0x89:	return "スマートガスメータ";
		case 0x8A:	return "高圧スマート電力量メータ";
		case 0x8B:	return "灯油メータ";
		case 0x8C:	return "スマート灯油メータ";
		case 0x90:	return "一般照明";
		case 0x91:	return "単機能照明";
		case 0x99:	return "非常照明";
		case 0x9D:	return "設備照明";
		case 0xA0:	return "ブザー";
		case 0xA1:	return "電気自動車充電器";
		}
	}
	else if (deoj.code[0] == 0x03)
	{
		switch(deoj.code[1])
		{
		case 0xB0:	return "コーヒメーカ";
		case 0xB1:	return "コーヒミル";
		case 0xB2:	return "電気ポット";
		case 0xB3:	return "電気こんろ";
		case 0xB4:	return "トースター";
		case 0xB5:	return "ジューサー・ミキサー";
		case 0xB6:	return "フードプロセッサ";
		case 0xB7:	return "冷凍蔵庫";
		case 0xB8:	return "オーブンレンジ";
		case 0xB9:	return "クッキングヒータ";
		case 0xBA:	return "オーブン";
		case 0xBB:	return "炊飯器";
		case 0xBC:	return "電子ジャー";
		case 0xBD:	return "食器洗い機";
		case 0xBE:	return "食器乾燥機";
		case 0xBF:	return "電気もちつき機";
		case 0xC0:	return "保温機";
		case 0xC1:	return "精米機";
		case 0xC2:	return "自動製パン機";
		case 0xC3:	return "スロークッカ";
		case 0xC4:	return "電気漬物機";
		case 0xC5:	return "洗濯機";
		case 0xC6:	return "衣類乾燥機";
		case 0xC7:	return "電気アイロン";
		case 0xC8:	return "ズボンプレッサ";
		case 0xC9:	return "ふとん乾燥機";
		case 0xCA:	return "小物・くつ乾燥機";
		case 0xCB:	return "電気掃除機";
		case 0xCC:	return "ディスポーザ";
		case 0xCD:	return "電気蚊取り機";
		case 0xCE:	return "業務用ショーケース";
		case 0xCF:	return "業務用冷蔵庫";
		case 0xD0:	return "業務用ホットケース";
		case 0xD1:	return "業務用フライヤー";
		case 0xD2:	return "業務用電子レンジ";
		case 0xD3:	return "洗濯乾燥機";
		case 0xD4:	return "業務用ショーケース向け室外機";
		}
	}
	else if (deoj.code[0] == 0x04)
	{
		switch(deoj.code[1])
		{
		case 0x01:	return "体重計";
		case 0x02:	return "体温計";
		case 0x03:	return "血圧計";
		case 0x04:	return "血糖値計";
		case 0x05:	return "体脂肪計";
		}
	}
	else if (deoj.code[0] == 0x05)
	{
		switch(deoj.code[1])
		{
		case 0xFA:	return "並列処理併用型電力制御";
		case 0xFB:	return "イベントコントローラ";
		case 0xFC:	return "セキュア通信用共有鍵設定ノード";
		case 0xFD:	return "スイッチ";
		case 0xFE:	return "携帯端末";
		case 0xFF:	return "コントローラ ";
		}
	}
	else if (deoj.code[0] == 0x06)
	{
		switch(deoj.code[1])
		{
		case 0x01:	return "ディスプレー";
		case 0x02:	return "テレビ";
		case 0x03:	return "オーディオ";
		case 0x04:	return "ネットワークカメラ";
		}
	}

	return "NAZO";
}

string NetDeviceEcoNetLite::MakeValue(const EcoNetLiteObjCode& deoj,const unsigned char& prop,const std::list<unsigned char>& data)
{
	ASSERT( !data.empty() );

	//16進数で値を求める. (#A0) or (#A0A0A0A0) 
	std::string ret = "(#";
	{
		char name[4] = {0};
		for(auto it = data.begin() ; it != data.end() ; it++ )
		{
			snprintf(name,4,"%02x",*it);
			ret += name;
		}
	}
	ret += ")";

	if (data.size() == 1)
	{//機能の名前がわかるならつけよう.
		list<string> l;
		if (! GetValue(deoj,prop,&l) )
		{
			for(auto it = l.begin() ; it != l.end() ; it++ )
			{
				if ( it->find(ret) != std::string::npos )
				{
					return *it;
				}
			}
		}
	}

	return "NAZO" + ret;
}

string NetDeviceEcoNetLite::ResolveName(const string& name)
{
	const string id = XLStringUtil::cut(name,"enl://","",NULL);
	if (! id.empty())
	{
		return id;
	}
	//あきらめる
	return "";
}

bool NetDeviceEcoNetLite::IsThisDevice(const string& name)
{
	return name.find("enl://")!=string::npos;
}

//すべての端末名を取得
list<string> NetDeviceEcoNetLite::GetAll()
{
	list<string> ret;

	list<EcoNetLiteMap> servers;
	MainWindow::m()->EcoNetLiteServer.GetAll(&servers);
	for(auto it = servers.begin() ; it != servers.end() ; it++ )
	{
		const string name = MakeName(it->identification,it->deoj);
		ret.push_back(name);
	}
	NOTIFYLOG("発見した機材 " << ret);
	return ret;
}

list<string> NetDeviceEcoNetLite::GetSetActionAll(const string& name)
{
	list<string> ret;
	string id = ResolveName(name);
	if (id.empty())
	{
		ERRORLOG("機材名がEcoNetLiteのものではありません name:" << name);
		return ret;
	}

	EcoNetLiteMap m;
	bool r = MainWindow::m()->EcoNetLiteServer.Get(id,&m,2);
	if (!r)
	{
		ERRORLOG("機材データが発見デバイスにありません name:" << name << " id:" << id);
		return ret;
	}

	for(auto it = m.setProp.begin() ; it != m.setProp.end() ; it++ )
	{
		const string name = MakeActionName(m.deoj,*it);
		ret.push_back(name);
	}
	NOTIFYLOG("設定できる機能を返します name:" << name << " id:" << id << " ret:" << ret);
	return ret;
}

list<string> NetDeviceEcoNetLite::GetSetValueAll(const string& name,const string& action)
{
	list<string> ret;
	string id;
	id = ResolveName(name);
	if (id.empty())
	{
		ERRORLOG("機材名がEcoNetLiteのものではありません name:" << name << " action:" << action);
		return ret;
	}
	EcoNetLiteMap m;
	bool r = MainWindow::m()->EcoNetLiteServer.Get(id,&m,2);
	if(!r)
	{
		ERRORLOG("機材データが発見デバイスにありません name:" << name << " action:" << action << " id:" << id);
		return ret;
	}

	unsigned char prop = NetDevice::ResolveValueName(action);
	if (prop <= 0)
	{
		ERRORLOG("動作名を解決できません name:" << name << " action:" << action << " id:" << id);
		return ret;
	}

	r = GetValue(m.deoj,prop,&ret);
	if(!r)
	{
		ERRORLOG("動作の情報を取得できません name:" << name << " action:" << action << " id:" << id);
		return ret;
	}

	NOTIFYLOG("設定できる値を返します name:" << name << " action:" << action << " id:" << id << " ret:" << ret);
	return ret;
}


list<string> NetDeviceEcoNetLite::GetGetActionAll(const string& name)
{
	list<string> ret;
	string id = ResolveName(name);
	if (id.empty())
	{
		ERRORLOG("機材名がEcoNetLiteのものではありません name:" << name);
		return ret;
	}

	EcoNetLiteMap m;
	bool r = MainWindow::m()->EcoNetLiteServer.Get(id,&m,2);
	if (!r)
	{
		ERRORLOG("機材データが発見デバイスにありません name:" << name << " id:" << id);
		return ret;
	}

	for(auto it = m.getProp.begin() ; it != m.getProp.end() ; it++ )
	{
		const string name = MakeActionName(m.deoj,*it);
		ret.push_back(name);
	}

	NOTIFYLOG("取得できる機能を返します name:" << name << " id:" << id << " ret:" << ret);
	return ret;
}

bool NetDeviceEcoNetLite::Fire(const string& name,const string& action,const string& value)
{
	string id;
	id = ResolveName(name);
	if (id.empty())
	{
		ERRORLOG("機材名がEcoNetLiteのものではありません name:" << name << " action:" << action << " value:" << value);
		return false;
	}
	EcoNetLiteMap m;
	bool r = MainWindow::m()->EcoNetLiteServer.Get(id,&m,5);
	if(!r)
	{
		ERRORLOG("機材データが発見デバイスにありません name:" << name << " id:" << id << " action:" << action << " value:" << value);
		return false;
	}

	unsigned char prop = NetDevice::ResolveValueName(action);
	if (prop <= 0)
	{
		ERRORLOG("動作名を解決できません name:" << name << " id:" << id << " action:" << action << " value:" << value);
		return false;
	}

	unsigned char valueUC = NetDevice::ResolveValueName(value);

	NOTIFYLOG("機材へ信号を送ります name:" << name << " id:" << id << " action:" << action << " value:" << value << " prop:" << prop << " valueUC:" << valueUC );

	//リクエストの送信.UDPなので結果は不定.
	MainWindow::m()->EcoNetLiteServer.sendSetRequest(m.ip,m.deoj,prop,valueUC);

	return true;
}

string NetDeviceEcoNetLite::Pickup(const string& name,const string& action )
{
	string id;
	id = ResolveName(name);
	if (id.empty())
	{
		ERRORLOG("機材名がEcoNetLiteのものではありません name:" << name << " action:" << action);
		return "";
	}
	EcoNetLiteMap m;
	bool r = MainWindow::m()->EcoNetLiteServer.Get(id,&m,5);
	if(!r)
	{
		ERRORLOG("機材データが発見デバイスにありません name:" << name << " id:" << id << " action:" << action);
		return "";
	}

	unsigned char prop = NetDevice::ResolveValueName(action);
	if (prop <= 0)
	{
		ERRORLOG("動作名が解決できません name:" << name << " id:" << id << " action:" << action);
		return "";
	}

	bool isUpdate = false;
	std::string ret;
	ECONETLITESERVER_TIDCALLBACK callback = [&](const EcoNetLiteData* data,const char* buffer,size_t size)->void 
	{
		list<unsigned char> getvalue;
		EcoNetLiteServer::ParseDataList(data,buffer,size,&getvalue);

		if ( getvalue.empty() )
		{
			ERRORLOG("戻り値が空です name:" << name << " action:" << action << " id:" << id);
			isUpdate = true;
			return ;
		}
		else 
		{
			ret = MakeValue(data->seoj,prop,getvalue);
			isUpdate = true;
			return ;
		}
	};

	unsigned short tid = 
		MainWindow::m()->EcoNetLiteServer.sendGetRequest(m.ip,m.deoj,prop,callback);

	//更新されるまで待つ.
	for(int i = 0 ; i < 20 ; i ++)
	{
		if (isUpdate)
		{
			return ret;
		}
		MiriSleep(100);
	}

	MainWindow::m()->EcoNetLiteServer.EraseTidWatch(tid);
	return ret;
}
