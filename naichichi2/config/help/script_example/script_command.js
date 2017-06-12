///@args1@must@サーバIP:PORT@例:192.168.1.2:15551@text
///@args2@must@サーバのsecret@例:SECRET
///@args3@must@コマンド@例:COMMAND
///@args4@optionl@引数@例:ARGS1 ARGS2 ARGS3
function main(host, secret,command,args)
{
	if (!args) args = "";
	execute_remote(host, secret,command,args);
}
