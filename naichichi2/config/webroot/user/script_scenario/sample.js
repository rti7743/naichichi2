// 実行されるごとに呼ばせれます
function main()
{
	onvoice("こんにちわ" //は
		,function(e){
			speak("はい、こんにちは");
		}
	)
	onvoice("hello" //はろー
		,function(e){
			speak("hello!");
		}
	)
}
