/**
  Akasha 中枢クラス群
  */

#ifndef AKASHA_APPLICATION__H
#define AKASHA_APPLICATION__H



//Akashaヘッダ TODO ヘッダ順序の整理
#include "net/FrontEnd.hpp"
#include "net/Network.hpp"

#include <wisp_v3/wisp.hpp>
#include <wisp_v3/irrExt.hpp>

//STLヘッダ
#include <vector>
#include <iostream>
#include <memory>

//Boostヘッダ
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/timer/timer.hpp>

//Irrlichtヘッダ
#include "irrlicht.h"





namespace akasha
{
	//fwd
	class AkashaWorld;
	namespace input
	{
		class IAkashaControllerInput;
		typedef boost::scoped_ptr<IAkashaControllerInput> pIAkashaControllerInput;
	}


	typedef boost::scoped_ptr<AkashaWorld> pAkashaWorld;

	//################################################################
	//! Akasha アプリケーション
	/**
	 * Akashaの中枢クラス。全てのAPIを提供。
	 * - Irrlichtの基本設定を管理。
	 */
	class AkashaApplication
	{
		public:
			/**
			 * ウィンドウハンドルが必須
			 */
			AkashaApplication( HWND windowHandle );
			/**
			 * この段階で内部の全てのオブジェクトはdeleteされる。
			 */
			~AkashaApplication();

			//! プログラムループ
			/**
			 * メインループ。
			 * while()で包むヨロシ。
			 * @return ループフラグ。終了でfalse。
			 */
			bool run();

			//! リサイズ処理
			/**
			 * ウィンドウサイズ変更時に呼ぶ必要がある。
			 * @param w 横幅ピクセル数
			 * @param h 縦幅ピクセル数
			 */
			void onResize( const int w, const int h );

			//Close
			void close();

			// GuiEvent
			void postGuiEvent( irr::SEvent const&);

		public:

			//! FPS取得
			/**
			 * IrrlichtのFPSを提供。
			 */
			int getFPS() const
			{
				return m_irrFps;
			}

			//! NetworkFrontEndの提供
			boost::shared_ptr< net::NetworkFrontEnd >
				getNetworkFrontEnd();

			//! ネットワーク・サーバー
			void runNetworkServer(net::ConnectionInfo const&, std::string const&);
			//! ネットワーククライアント
			void runNetworkClient(
					net::ConnectionInfo const&, std::string const& address, std::string const& port);
			//! ネットワーククローズ
			void closeNetwork();

			//! Frameレート取得
			/**
			 * 自前のやっつけタイマーによるフレームレート取得
			 */
			inline double getFrameRate()
			{
				return m_trueFrameRate;
			}



			//メッセージ系処理################
			//! モデルファイルを開く
			/**
			 * モデルファイルのパスをもらってモデルを開く
			 */
			bool openModelFile( LPCTSTR modelFilePath );

			//! Open SubModel by File
			bool openSubModelFile( LPCTSTR filePath );

			//! ランドファイルを開く
			/**
			 * ランドファイルのパスをもらって判定の後に開く
			 */
			bool openLandFile( LPCTSTR landFilePath );

			//! 作用力表示トグル
			/**
			 * 表示状態を返す
			 */
			bool toggleShowExternForce();

			//! デバック表示トグル
			/**
			 * 表示状態を返す
			 */
			bool toggleShowDebug();

			//! 物理エンジンのデバック描画パラメータを設定する
			void setBulletDebugDrawFlag( int flg );
			int getBulletDebugDrawFlag() const;

		private:
			HWND m_windowHandle; // Window ハンドル

			irrExt::IrrExtention m_irrExt;

			pAkashaWorld m_pAkashaWorld; //!<Akashaワールド
			boost::timer::cpu_timer m_frameRateTimer;// FPS制御のためのタイマー
			double m_trueFrameRate; // 1Frameの処理時間

			int m_irrFps;
			net::Network m_network;
			boost::shared_ptr< net::NetworkFrontEnd > m_networkFrontEnd;

	};

}

#endif //AKASHA_APPLICATION__H
