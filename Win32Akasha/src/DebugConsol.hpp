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
//! �f�o�b�N�R���\�[��

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

  //! �R���\�[���ɕ������ǉ�
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



  //! �R���\�[���R�}���h�̒ǉ�
  BOOL AddCommand(const akasha::tstring& command, CommandFunc& func)
    {
      m_commandMap.insert( CommandMap::value_type( command, func ));
      //boost::swap(func,m_commandMap[ command ]);
      return TRUE;
    }


  //! �ĕ\��
  BOOL refresh()
    {
      BOOL res = SetWindowText((m_text + akasha::tstring(">")).c_str());
      m_lineCountBuffer = GetLineCount();
      //�L�����b�g���Ō�Ɉړ�
      int length = ::GetWindowTextLength(GetHwnd());
      SetSel(length,length,true);

      return res;
    }

  //! ���e�̍X�V
  void Update()
    {

      //���[�U�[���͂ɂ��s���ω��`�F�b�N
      if (GetLineCount()==m_lineCountBuffer) return;
      m_lineCountBuffer = GetLineCount();

      LPTSTR buf[100];
      GetLine(GetLineCount()-2, (LPTSTR)buf, 100);
      akasha::tstring line((TCHAR*)buf);

      //�L�����b�g�ȊO�����邩
      //�ŏ��̓L�����b�g��
      if (line.size() > 1 && line.find(">",0)!=0)
        {
          refresh();
          return;
        }

      AddText( line ); //�\��

      size_t length2 = line.find(" ",1);
      if (std::string::npos== length2)
        ExecLine( line.substr(1));
      else
        ExecLine(
          line.substr(1,length2-1 ),
          line.substr(length2+1)
          );
    }

  //! �J�����g�s�̃R�}���h�����s
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

  //! �v���N���G�C�g�B�I�[�o�[���C�h
  virtual void PreCreate(CREATESTRUCT& cs){
    cs.cy = 1000;
    cs.cx = 300;

    cs.style =
      WS_VISIBLE | WS_SYSMENU | WS_THICKFRAME |
       WS_HSCROLL | WS_VSCROLL |ES_AUTOHSCROLL | ES_MULTILINE;

    CEdit::PreCreate(cs);
  }

  //!�E�B���h���b�Z�[�W����
  virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
      switch(uMsg)
        {
        case WM_CLOSE:          // ����{�^���ŕs����
          ShowWindow(SW_HIDE);
          break;
        case WM_KEYUP:
        case WM_KEYDOWN:
          Update();
          break;
        }
      return WndProcDefault(uMsg, wParam, lParam);
    }

  //! �R�}���h����
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
