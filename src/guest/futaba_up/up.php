<?
define(PHP_SELF, 'up.php');	#このスクリプト名
define(PHP_SELF2, 'up.htm');	#入り口ファイル名
define(LOGFILE, 'up.log');	#ログファイル名
define(SRC_DIR, 'src/');	#ファイル保存dir
define(PREFIX, 'f');	#ファイル接頭辞
define(MAX_FILE, '1073741824');	#ファイル最大bytes
define(MAX_LOG, '300');	#保存件数

while(list($key,$val)=each($_POST)){$_POST[$key]=Escstr($val);}
while(list($key,$val)=each($_GET)){$_GET[$key]=Escstr($val);}
extract($_POST);
$name=$_FILES["up"]["name"];
$mime=Escstr($_FILES['up']['type']);
$up=$_FILES["up"]["tmp_name"];
$allowext = array(asf,avi,bmp,c,exe,gif,htm,jpg,lzh,mid,mov,mp3,mpg,pdf,png,psd,ram,rar,rm,swf,txt,wav,wma,zip);

function regist($com,$up,$name,$mime,$pass){
	global $allowext;
	$time = time();
	$tim = $time.substr(microtime(),2,3);

	if( $_FILES["up"]["error"]==2 ){
		error("Error : regist : up error:2");
	}
	if( $name && $_FILES["up"]["size"]==0 ){
		error("Error : regist : up size=0");
	}
	if( !file_exists(SRC_DIR) ){
		mkdir(SRC_DIR);
	}
	if($up&&file_exists($up)&&$size=filesize($up)){
		if($size>MAX_FILE){
			error("Error : regist : でかい. size=".$size );
		}
		$dest = SRC_DIR.$tim.'.tmp';
		move_uploaded_file($up, $dest);
		if(!$pass){$pass='dummy';}
		$pass = md5($pass);

		$youbi = array('日','月','火','水','木','金','土');
		$now = date("y/m/d(",$time).(string)$youbi[date("w",$time)].date(")H:i",$time);

		$ext = ereg_replace("^.*\\.",".",$name);
		$ext = substr(strtolower($ext),0,5);
		$flag=FALSE;
		foreach($allowext as $value){
			if('.'.$value==$ext){$flag=TRUE;}
		}
		if(!$flag){$ext='.xxx';}
		$size=(int)($size/1000);

		if( !file_exists(LOGFILE) ){
			$fp = fopen(LOGFILE,"w");
			fclose($fp);
		}
		$fp = fopen(LOGFILE,"r+");

		flock($fp,2);
		rewind($fp);
		$buf=fread($fp,90000);
		$line = explode("\n",$buf);
		$countline=count($line);
		for($i = 0; $i < $countline; $i++){
			if($line[$i]){$line[$i].="\n";}
		}

		if($countline >= MAX_LOG){
			for($d=$countline-1;$d>=MAX_LOG-1;$d--){
				list($dno,$dname,)=explode("\t",$line[$d]);
				if(is_file(SRC_DIR.$dname)){unlink(SRC_DIR.$dname);}
				$line[$d]='';
			}
		}

		list($lastno,) = explode("\t", $line[0]);
		$no=sprintf("%04d",++$lastno);
		$name=PREFIX.$no.$ext;

		$newline = "$no\t$name\t$now\t$pass\t$com\t$mime\t$size\t\n";
		$newline.= implode('', $line);
		ftruncate($fp,0);
		set_file_buffer($fp, 0);
		rewind($fp);
		fputs($fp, $newline);
		fclose($fp);
		rename($dest,SRC_DIR.$name);
		chmod(SRC_DIR.$name,0666);
	}
}

function usrdel($no,$pass){
	if($pass){
		$pass = md5($pass);
		$fp = fopen(LOGFILE, "r+");
		flock($fp, 2);
		rewind($fp);
		$buf=fread($fp,90000);
		$line = explode("\n",$buf);
		$countline=count($line);
		for($i = 0; $i < $countline; $i++){
			if($line[$i]){$line[$i].="\n";}
			list($dno,$dname,,$dpass,)=explode("\t",$line[$i]);
			if($dno==$no){
				if($dpass==$pass){
					if(is_file(SRC_DIR.$dname)){unlink(SRC_DIR.$dname);}
					$line[$i]='';
				}else{error("パス違う。");}
			}
		}
		$newline= implode('', $line);
		ftruncate($fp,0);
		set_file_buffer($fp, 0);
		rewind($fp);
		fputs($fp, $newline);
		fclose($fp);
	}else{
		head($buf);
		$buf.='<form action="'.PHP_SELF.'" method=POST><h3>記事'.$no.'番を削除します</h3>
<input type=hidden name=delno value='.$no.'><input type=hidden name=mode value=del>
パスワード入力：<input type=password size=12 name=pass><br>
<input type=submit value="削除" class=button></form>';
		echo $buf;
		exit;
	}
}

function Escstr($str){
	$str = htmlspecialchars($str);
	if(get_magic_quotes_gpc()){$str=stripslashes($str);}
	return ereg_replace("[\r\n\x09]", "", $str);
}

function head(&$buf){
	$buf='<html><head>
<STYLE><!--
A	{text-decoration: none;}
A:link	{text-decoration: none;}
A:hover	{text-decoration: none; background-color: #508080; color: #FFFFFF;}
select	{background-color:#007f7f; color:#FFFFFF;}
.button	{background-color:#007f7f; color:#FFFFFF; border-color:#FFFFFF;}
--></STYLE>
</head><body bgcolor="#004040" text="#ffffff" link="#eeffee" vlink="#dddddd" alink="#ff0000">
<table width="100%"><tr><td bgcolor="#007f7f"><strong><font size=4>あぷ＠ふたば</font></strong></td></tr></table><br>phpテスト中…。<br>';
}

function updatelog(){
	head($buf);
	$buf.='<form enctype="multipart/form-data" action="'.PHP_SELF.'" method=post>
<input type=hidden name=MAX_FILE_SIZE value='.MAX_FILE.'><input type=hidden name=mode value=reg>
アプするファイル('.(MAX_FILE/1024/1024).'MBまで)<br><input name=up type=file size=40><br>
コメント<br><input type=text name=com size=40><input type=submit value=Send><br>
del pass:<input type=text name=pass size=10 maxlength=10><br></form><hr>';

	if( !file_exists(SRC_DIR) ){
		mkdir(SRC_DIR);
	}
	if( !file_exists(LOGFILE) ){
		$fp = fopen(LOGFILE,"w");
		fclose($fp);
	}
	$line=file(LOGFILE);
	$countline = count($line);
	$buf.="<table><tr><th>ACT</th><th>NAME</th><th>COMENT</th><th>SIZE(KB)</th><th>MIME</th><th>DATE</th></tr>\n";
	for($i=0;$i<$countline;$i++){
		list($no,$name,$now,,$com,$mime,$size,) = explode("\t", $line[$i]);
		$href=SRC_DIR.$name;
		$buf.="<tr><td><small><a href=\"".PHP_SELF."?del=$no\">D<a></small></td>".
			"<td>[<a href=\"$href\" target=\"_blank\">$name</a>]</td>".
			"<td><font size=2>$com</font></td><td align=right><b><font size=2>$size</font></b></td>".
			"<td><font color=\"#888888\" size=2>$mime</font></td><td><font size=2>$now</font></td></tr>\n";
	}
	$buf.='</table><hr></body></html>';

	$fp = fopen(PHP_SELF2, "w");
	set_file_buffer($fp, 0);
	fputs($fp,$buf);
	fclose($fp);
}

function error($mes,$dest=''){
	if($dest&&is_file($dest)) unlink($dest);
	head($dat);
	echo $dat."<br><br><hr size=1><br><br>".
		"<center><font color=red size=5><b>$mes<br><br><a href=".PHP_SELF2.">リロード</a></b></font></center>".
		"<br><br><hr size=1></body></html>";
	exit;
}

if($_GET["del"]){
	usrdel($_GET["del"],'');
}
switch($mode){
	case 'del':
		if(!$pass){$pass='dummy';}
		usrdel($delno,$pass);
	case 'reg':
		regist($com,$up,$name,$mime,$pass);
	default:
		updatelog();
		echo "<META HTTP-EQUIV=\"refresh\" content=\"0;URL=".PHP_SELF2."\">";
}
?>
