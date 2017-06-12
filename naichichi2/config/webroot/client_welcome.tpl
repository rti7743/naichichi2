<!DOCTYPE html>
<html lang="ja"><head>
	<meta name="format-detection" content="telephone=no" />
	<meta name="viewport" content="width=640,user-scalable=yes,maximum-scale=0.67" />
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=99" />
	<title>welcome and jump</title>
	<!-- このコードは GPL でライセンスされてます。 -->
	<base href="/"></base>
<body>
サーバに接続します・・・・<br />
<br />
<script>
url = "%SERVER_URL%" + "welcome/" 
	+ "?model=" + "%MODEL%" 
	+ "&backurl=" + "%BACKURL%"
	;

window.location.href = url;
</script>
</html>