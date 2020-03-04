package znkproject.moai;

import android.app.Activity;
import android.Manifest;
import android.os.Bundle;
import android.os.Build;
import android.os.Environment;
import android.graphics.Color;
import android.view.Menu;
import android.view.MenuItem;
import android.view.KeyEvent;
import android.view.View;
import android.content.res.AssetManager;
import android.content.pm.PackageManager;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.ClipboardManager;
import android.content.ClipData;
import android.content.ClipDescription;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.Button;
import android.text.Html;
import android.net.Uri;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Handler;
import android.provider.MediaStore;
import android.support.annotation.NonNull;

import java.io.File;
import java.io.IOException;

public class MainActivity extends Activity
	implements ActivityCompat.OnRequestPermissionsResultCallback
{
	final int RequestCode_MoaiPermission = 1;

	static AssetManager   assetManager;
	static PackageManager packageManager;
	static TextView       textView;

	static {
		System.loadLibrary("main");
	}

    public static void copyFromAssets_inDir( AssetManager assetManager, String asset_dst_dir, String dir_path ) {
		try {
			String[] nameList = assetManager.list( dir_path );
			if( dir_path == "" ){
				for( int i = 0; i < nameList.length; ++i ){
					copyFromAssets( assetManager,
							nameList[ i ],
							asset_dst_dir,
							nameList[ i ] );
				}
			} else {
				for( int i = 0; i < nameList.length; ++i ){
					copyFromAssets( assetManager,
							dir_path + "/" + nameList[ i ],
							asset_dst_dir + "/" + dir_path,
							nameList[ i ] );
				}
			}
		} catch ( IOException e ) {
		}
	}

	public static String testFromAssets( AssetManager assetManager ) {
		String ans = "[";
		try {
			String[] nameList = assetManager.list( "" );
			for( int i = 0; i < nameList.length; ++i ){
				ans += nameList[ i ] + ",";
			}
		} catch ( IOException e ) {
		}
		ans += "]";
		return ans;
	}

	public static String getPackageSourceDir( PackageManager packageManager ) {
		ApplicationInfo info = null;
		String path = null;
		try {
			info = packageManager.getApplicationInfo(
					"znkproject.moai",
					PackageManager.GET_META_DATA );
			path = info.sourceDir;
		} catch ( NameNotFoundException e ) {
		}
		return path;
	}

	public static String getNativeLibraryDir( PackageManager packageManager ) {
		ApplicationInfo info = null;
		String path = null;
		try {
			info = packageManager.getApplicationInfo(
					"znkproject.moai",
					PackageManager.GET_SHARED_LIBRARY_FILES );
			path = info.nativeLibraryDir;
		} catch ( NameNotFoundException e ) {
		}
		return path;
	}
	
	/**
	 * For Android6.0 Runtime Permission
	 * Reference: https://developer.android.com/training/permissions/requesting
	 */
	private boolean checkSelfPermission_forPermAry( String[] perm_ary ){
		for( String perm : perm_ary ){
			if( ContextCompat.checkSelfPermission( this.getApplicationContext(), perm ) != PackageManager.PERMISSION_GRANTED ){
				return false;
			}
		}
		return true;
	}
	private boolean checkGrantResult( int[] grantResults ){
		/**
		 * If request is cancelled, the result arrays are empty.
		 */
		if( grantResults.length == 0 ){
			return false;
		}
		for( int grant : grantResults ){
			if( grant != PackageManager.PERMISSION_GRANTED ){
				return false;
			}
		}
		return true;
	}
	private void confirmPermission(){
		String[] perm_ary = new String[]{
			Manifest.permission.READ_EXTERNAL_STORAGE,
			Manifest.permission.WRITE_EXTERNAL_STORAGE
		};

		if( !checkSelfPermission_forPermAry( perm_ary ) ){
			/**
			 * Permission is not granted.
			 * Should we show an explanation?
			 */
			if( ActivityCompat.shouldShowRequestPermissionRationale( this,
						Manifest.permission.WRITE_EXTERNAL_STORAGE ) )
			{
				/**
				 * Show an explanation to the user *asynchronously* -- don't block
				 * this thread waiting for the user's response! After the user
				 * sees the explanation, try again to request the permission.
				 */
				Toast.makeText( this, "Permission is OFF.", Toast.LENGTH_LONG ).show();
				
			} else {
				/**
				 * No explanation needed; request the permission
				 */
				ActivityCompat.requestPermissions( this,
						perm_ary,
						RequestCode_MoaiPermission );
				/**
				 * RequestCode_MoaiPermission is an app-defined int constant.
				 * The callback method gets the result of the request.
				 */
			}
		} else{
			/* Permission has already been granted */
		}
	}

	@Override
	public void onRequestPermissionsResult( int requestCode, String[] perm_ary, int[] grantResults ){
		switch( requestCode ){
		case RequestCode_MoaiPermission: {
			if( checkGrantResult( grantResults ) ){
				/**
				 * permission was granted, yay!
				 * Do the contacts-related task you need to do.
				 */
			} else {
				/**
				 * permission denied, boo!
				 * Disable the functionality that depends on this permission.
				 */
				Toast.makeText( this, "Permission denied.", Toast.LENGTH_LONG ).show();
			}
			return;
		}
		}
		/**
		 * other 'case' lines to check for other perm_ary this app might request.
		 */
		super.onRequestPermissionsResult( requestCode, perm_ary, grantResults );
	}


	private void openUri( String uri_str ){
		Uri uri = Uri.parse( uri_str );
		Intent intent = new Intent( Intent.ACTION_VIEW, uri );
		startActivity(intent);
	}
	private boolean mkdirAndSetRW( String dir, StringBuilder ermsg ){
		File file_obj = new File( dir );
		if( !file_obj.exists() ){
			file_obj.mkdirs();
		}
		if( file_obj.exists() ){
			file_obj.setReadable( true, true );
			file_obj.setWritable( true, true );
			if( !file_obj.canWrite() ){
				ermsg.append( "[<font color=\"#ffff00\">FYI</font>] " + dir + " cannot write.<br>" );
				return false;
			}
			ermsg.append( "[<font color=\"#00ff00\">OK</font>] " + dir + " auto-detected.<br>" );
			return true;
		} else {
			ermsg.append( "[<font color=\"#ffff00\">FYI</font>] " + dir + " cannot mkdir.<br>" );
		}
		return false;
	}

    @Override
	protected void onCreate( Bundle savedInstanceState ){

    	exitIfOldProcessExist();
		try {
			super.onCreate(savedInstanceState);
			setContentView(R.layout.activity_main);
			String private_path = getFilesDir().getPath();
			//String private_external_path = getExternalFilesDir(null).getPath();
			String asset_dst_dir = private_path; 
			/**
			 * TODO:
			 *
			 * PCとMTPによりUSB接続したときに、一部のファイルが表示されない、あるいは削除したファイルが表示されるといった現象がおきる.
			 * これはパーミッションの問題ではなく、android側で該当ファイルがメディアスキャンされていないために起きる.
			 * 逆に言えば、MTPを介して表示されるファイルはメディアスキャンされたもののみであるということである.
			 * メディアスキャンはandroid端末自体を再起動すれば、そのタイミングで再スキャンされるので、
			 * そうするのがもっとも確実ではあるが、ここではMoaiを起動したタイミングでせめてmoai_profileだけでもスキャンさせておきたいところ.
			 *
			 * 尚、通常メディアスキャンされたファイルは例えばそれが画像なら「画像」という仮想フォルダ直下にすべて表示されてしまう.
			 * moai_profileでは負荷分散のため大量の画像ファイルをいくつかのフォルダに分散しながら保存しているが、
			 * それらがすべて「画像」直下に並列表示されてしまっては全く意味がない.
			 * これを防ぐには moai_profile 直下に .nomedia という空のファイルを置けばよい(これを置いた場合でもMTPでの表示は行われる).
			 * ちなみに /sdcard/Android/data にもこの .nomedia がデフォルトで置かれてあり、配下の画像が「画像」のプレビュー対象に
			 * ならないようにしてあるようだ.
			 */


			int    sdk_int = Build.VERSION.SDK_INT;
			String abi_str = "";
			String pkg_path = "";
			String nativ_lib_path = "";
			StringBuilder ermsg = new StringBuilder( "" );

			if( sdk_int >= 21 ){
				/**
				 * SUPPORTED_ABISはsdk 21以上でなければ使えない.
				 * (20以下のときこれを実行するとそのようなフィールドは存在しないためクラッシュする).
				 */
				String[] abi_strs = Build.SUPPORTED_ABIS;
				if( abi_strs.length > 0 ){
					abi_str = abi_strs[ 0 ];
				}
			} else {
				abi_str = Build.CPU_ABI;
			}

			if( sdk_int <= 15 ){
				/**
				 * android 4.0.3 以下
				 *   このバージョンのandroidでは非PIEでコンパイルしたバイナリのみが起動できる.
				 *   PIEでコンパイルしたバイナリで起動するとSegmentation faultを起こす.
				 *   ただしPIEでコンパイルした動的ライブラリをロードして使う分には問題ない.
				 *   つまりJniでライブラリをロードする分には何も問題ないが、MoaiCGIのようにさらに奥で子プロセスを起動させるとき
				 *   その子プロセスがPIEバイナリであった場合は問題が起こるのではないかと思われる.
				 *
				 *   この問題を解決するため、以下の方法が考えられる.
				 *
				 *   1. 基本はPIEでコンパイルしておいて、4.0.3以下用にrun_pieを挟むという方法.
				 *      MoaiCGIでうまくいくかどうかなどまだ検討の余地は大きい.
				 *
				 *   2. libEst, libCBなどと本体をライブラリ化(これはPIEで)しておいて、bin/ bin/npie 等に最低限の起動プログラム(PIE版と非PIE版)を置く.
				 *      これならシンボリックリンクをうまく切り替えることでeaster.cgi, custom_boy.cgi という名前を共通化できそうではある.
				 */
			} else if( sdk_int >= 21 ){
				/**
				 * android 5.0 以上
				 *   このバージョンのandroidではPIEでコンパイルしたバイナリのみが起動できる.
				 *   非PIEでコンパイルしたバイナリで起動すると Not supported というメッセージが出て起動できない.
				 *   またMoaiCGIのようにさらに子プロセスを起動させるとき
				 *   その子プロセスが非PIEバイナリであった場合は問題が起こるのではないかと思われる.
				 */
			} else {
				/**
				 * android 4.1 以上 4.4 以下
				 *   このバージョンのandroidではPIEと非PIEのどちらでコンパイルしたバイナリでも起動できる.
				 */
			}

			assetManager   = getAssets();
			packageManager = getPackageManager();
    		pkg_path       = getPackageSourceDir( packageManager );
    		nativ_lib_path = getNativeLibraryDir( packageManager );

			copyFromAssets_inDir( assetManager, asset_dst_dir, "" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "default/filters" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "doc_root" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "doc_root/common" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "doc_root/imgs" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "doc_root/moai2.0" );

			copyFromAssets_inDir( assetManager, asset_dst_dir, "bin/" + abi_str );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "bin/" + abi_str + "/cgi_developers/protected" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "bin/" + abi_str + "/plugins" );

			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/cgi_developers/protected" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/cgi_developers/publicbox" );

			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/easter" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/easter/default" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/easter/publicbox" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/easter/publicbox/icons" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/easter/publicbox/bbs_5ch" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/easter/publicbox/bbs_futaba" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/easter/publicbox/pascua" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/easter/alternative/5ch/agree.5ch.net/js/ad.js" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/easter/alternative/5ch/itest.5ch.net/assets/js/android/application.js" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/easter/alternative/5ch/itest.5ch.net/assets/js/iphone/application.js" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/easter/templates" );

			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/custom_boy" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/custom_boy/publicbox" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/custom_boy/RE" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/custom_boy/templates" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/custom_boy/UA" );

			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/proxy_finder" );
			copyFromAssets_inDir( assetManager, asset_dst_dir, "cgis/proxy_finder/templates" );

			copyFromAssets_inDir( assetManager, asset_dst_dir, "plugins" );

			/**
			 * Android 6, API 23以上
			 */
			if( sdk_int >= 23 ){
				confirmPermission();
			}

			{
				/**
				 * ext_storage_dir:
				 *   exam: /storage/emulated/0
				 * download_dir:
				 *   exam: /storage/emulated/0/Download
				 */
				boolean success = false;
				if( !success ){
					String ext_storage_dir = Environment.getExternalStorageDirectory().getAbsolutePath();
					String profile_dir     = ext_storage_dir + "/moai_profile";
					success = mkdirAndSetRW( profile_dir, ermsg );
				}
				if( !success ){
					String download_dir = Environment.getExternalStoragePublicDirectory( Environment.DIRECTORY_DOWNLOADS ).getAbsolutePath();
					String profile_dir  = download_dir + "/moai_profile";
					success = mkdirAndSetRW( profile_dir, ermsg );
				}
				if( !success ){
					ermsg.append( "[<font color=\"#ff0000\">NG</font>] " + " moai_profile auto-detection.<br>" );
				}
			}

			textView = (TextView)findViewById( R.id.text_view );
			//textView.setTextIsSelectable( true );
			//textView.setHorizontallyScrolling( true );
			textView.setText( "Moai Ver2.2.3(prototype8)\n" + "(" + abi_str + " sdk:" + sdk_int + ")\n" );
			if( this.init(asset_dst_dir, abi_str, pkg_path, nativ_lib_path) ){
				textView.append( Html.fromHtml("[<font color=\"#00ff00\">OK</font>] init.<br>") );
			} else {
				textView.append( Html.fromHtml("[<font color=\"#ff0000\">NG</font>] init Error. See moai_jni.log.<br>") );
			}
			
			this.mainLoop(); /* Moai Thread Start */

			Toast.makeText( this, "Moai Thread Start.", Toast.LENGTH_LONG ).show();
			textView.append( Html.fromHtml(ermsg.toString()) );

			/**
			 * ボタンopen_top
			 */
			{
				Button button = (Button)findViewById( R.id.open_top );
				button.setAllCaps( false );
				button.setOnClickListener( new View.OnClickListener(){
					@Override
					public void onClick( View v ){
						openUri( "http://127.0.0.1:8124" );
					}
				} );
			}
			/**
			 * ボタンopen_easter
			 */
			{
				Button button = (Button)findViewById( R.id.open_easter );
				button.setAllCaps( false );
				button.setOnClickListener( new View.OnClickListener(){
					@Override
					public void onClick( View v ){
						openUri( "http://127.0.0.1:8124/easter" );
					}
				} );
			}
			/**
			 * ボタンopen_virtual_users
			 */
			{
				Button button = (Button)findViewById( R.id.open_virtual_users );
				button.setAllCaps( false );
				button.setOnClickListener( new View.OnClickListener(){
					@Override
					public void onClick( View v ){
						openUri( "http://127.0.0.1:8124/cgis/custom_boy/custom_boy.cgi?cb_target=futaba&cb_type=automatic" );
					}
				} );
			}
			/**
			 * ボタンopen_engine_config
			 */
			{
				Button button = (Button)findViewById( R.id.open_engine_config );
				button.setAllCaps( false );
				button.setOnClickListener( new View.OnClickListener(){
					@Override
					public void onClick( View v ){
						openUri( "http://127.0.0.1:8124/config" );
					}
				} );
			}
			/**
			 * ボタンcopy_log_to_clipboard
			 */
			{
				Button button = (Button)findViewById( R.id.copy_log_to_clipboard );
				button.setAllCaps( false );
				button.setOnClickListener( new View.OnClickListener(){
					@Override
					public void onClick( View v ){
						ClipData.Item item = new ClipData.Item( textView.getText() );
						String[] mimeType = new String[1];
						mimeType[0] = ClipDescription.MIMETYPE_TEXT_PLAIN;
						 
						ClipData cd = new ClipData(new ClipDescription("moai_log_data", mimeType), item);
						ClipboardManager cm = (ClipboardManager) getSystemService(CLIPBOARD_SERVICE);
						cm.setPrimaryClip(cd);
					}
				} );
			}
			

		}catch( Exception ex ){
			//ex.printStackTrace();
			Toast.makeText( this, "Exception.", Toast.LENGTH_LONG ).show();
		}
    }

    public static native boolean exitIfOldProcessExist();
    public static native boolean init( String private_path, String abi_str, String pkg_path, String nativ_lib_path );
    public static native String mainLoop();
	public static native boolean copyFromAssets( AssetManager assetManager,
			String src_filename, String dst_dir, String dst_filename );


}
