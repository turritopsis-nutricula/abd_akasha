/*
 * DebugConsol.hpp
 *
 *  Created on: 2012/01/03
 *      Author: ely
 */

#ifndef DEBUGCONSOL_HPP_
#define DEBUGCONSOL_HPP_


/*################################################################
  * DebugConsol
  */
//! デバックコンソール

/*class DebugConsolDialog : public CEdit
{
public:
  //typedef boost::function<void (const akasha::tstring&)>  CommandFunc;
  typedef boost::function<void ()>  CommandFunc;
  typedef std::map<akasha::tstring, CommandFunc> CommandMap;
public:
  DebugConsolDialog() :
  CEdit(),
  m_lineCountBuffer(0)
    {
          }

  ~DebugConsolDialog(){ }

  //! コンソールに文字列を追加
  template <typename T>
  void AddText(const T& str, bool newLine = true)
    {
      try
        {
          akasha::tstring s = boost::lexical_cast<akasha::tstring>(str);
          AddText__(s, newLine);
        }
      catch(boost::bad_lexical_cast& e)
        {
         // OutDebug( e.what(), " " );
        }
    }



  //! コンソールコマンドの追加
  BOOL AddCommand(const akasha::tstring& command, CommandFunc& func)
    {
      m_commandMap.insert( CommandMap::value_type( command, func ));
      //boost::swap(func,m_commandMap[ command ]);
      return TRUE;
    }


  //! 再表示
  BOOL refresh()
    {
      BOOL res = SetWindowText((m_text + akasha::tstring(">")).c_str());
      m_lineCountBuffer = GetLineCount();
      //キャレットを最後に移動
      int length = ::GetWindowTextLength(GetHwnd());
      SetSel(length,length,true);

      return res;
    }

  //! 内容の更新
  void Update()
    {

      //ユーザー入力による行数変化チェック
      if (GetLineCount()==m_lineCountBuffer) return;
      m_lineCountBuffer = GetLineCount();

      LPTSTR buf[100];
      GetLine(GetLineCount()-2, (LPTSTR)buf, 100);
      akasha::tstring line((TCHAR*)buf);

      //キャレット以外があるか
      //最初はキャレットか
      if (line.size() > 1 && line.find(">",0)!=0)
        {
          refresh();
          return;
        }

      AddText( line ); //表示

      size_t length2 = line.find(" ",1);
      if (std::string::npos== length2)
        ExecLine( line.substr(1));
      else
        ExecLine(
          line.substr(1,length2-1 ),
          line.substr(length2+1)
          );
    }

  //! カレント行のコマンドを実行
  BOOL ExecLine(const akasha::tstring& command, const akasha::tstring& arg = "")
    {

      CommandMap::iterator i = m_commandMap.find(command);
      if (m_commandMap.end() == i)
        {
          AddText(  command + akasha::tstring(" NotFond."));
          return FALSE;
        }

      //(*i).second( arg );
      (*i).second( );
      return TRUE;
    }

  //! プリクリエイト。オーバーライド
  virtual void PreCreate(CREATESTRUCT& cs){
    cs.cy = 1000;
    cs.cx = 300;

    cs.style =
      WS_VISIBLE | WS_SYSMENU | WS_THICKFRAME |
       WS_HSCROLL | WS_VSCROLL |ES_AUTOHSCROLL | ES_MULTILINE;

    CEdit::PreCreate(cs);
  }

  //!ウィンドメッセージ処理
  virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
      switch(uMsg)
        {
        case WM_CLOSE:          // 閉じるボタンで不可視化
          ShowWindow(SW_HIDE);
          break;
        case WM_KEYUP:
        case WM_KEYDOWN:
          Update();
          break;
        }
      return WndProcDefault(uMsg, wParam, lParam);
    }

  //! コマンド処理
  BOOL OnCommand(WPARAM wParam, LPARAM lParam)
    {
      UNREFERENCED_PARAMETER(wParam);
      UNREFERENCED_PARAMETER(lParam);


      return FALSE;
    }

private:

  BOOL AddText__(const akasha::tstring& str, bool newLine = true)
    {
      m_text += str;
      if (newLine)
        m_text += akasha::tstring("\r\n");


      return refresh();
    }

private:
  akasha::tstring m_text;
  CommandMap m_commandMap;
  int m_lineCountBuffer;

};

*/
#endif /* DEBUGCONSOL_HPP_ */
