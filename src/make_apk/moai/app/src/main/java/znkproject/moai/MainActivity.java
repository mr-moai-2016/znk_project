package znkproject.moai;

import android.app.Activity;
import android.os.Bundle;
import android.os.Build;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;
import android.content.res.AssetManager;

import java.io.IOException;

public class MainActivity extends Activity {

	static AssetManager assetManager;

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

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
		String private_path = getFilesDir().getPath();
		String private_external_path = getExternalFilesDir(null).getPath();
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

		assetManager = getAssets();

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

		TextView textView = (TextView)findViewById(R.id.my_label);
		if( this.init(asset_dst_dir, abi_str) ){
			textView.setText( "Moai Ver2.2\nOK. Start browsing!\n" + "(" + abi_str + " sdk:" + sdk_int + ")" );
		} else {
			textView.setText( "Moai Ver2.2\nNG. init Error.\n" + "See moai_jni.log.\n" );
		}
		
		this.mainLoop(); /* Moai Thread Start */
    }

    public static native boolean init( String private_path, String abi_str );
    public static native String mainLoop();
	public static native boolean copyFromAssets( AssetManager assetManager,
			String src_filename, String dst_dir, String dst_filename );


}
