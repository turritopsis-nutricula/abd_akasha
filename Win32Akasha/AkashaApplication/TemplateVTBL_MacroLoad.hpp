#ifndef TEMPLATE_VTBL__HPP
#define TEMPLATE_VTBL__HPP


#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/list/adt.hpp>
#include <boost/preprocessor/comparison/less.hpp>
#include <boost/preprocessor/comparison/not_equal.hpp>

#include <boost/preprocessor/facilities/is_empty.hpp>

#include <boost/preprocessor/control/if.hpp>
//#include <boost/preprocessor/control/iif.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/control/expr_if.hpp>
//#include <boost/preprocessor/control/expr_iif.hpp>

#include <boost/preprocessor/repetition/enum_params.hpp>

#include <boost/preprocessor/array/size.hpp>
#include <boost/preprocessor/array/data.hpp>
#include <boost/preprocessor/array/elem.hpp>

#include <boost/preprocessor/tuple/rem.hpp>
#include <boost/preprocessor/tuple/to_list.hpp>

#include <boost/preprocessor/list/append.hpp>
#include  <boost/preprocessor/list/for_each.hpp>
#include <boost/preprocessor/list/for_each_i.hpp>

#include <boost/preprocessor/repetition/repeat.hpp>

#include <boost/static_assert.hpp>
#include <boost/preprocessor/debug/assert.hpp>
#endif //TEMPLATE_VTBL__HPP

#if !defined(VTBL_ARRAY)
BOOST_STATIC_ASSERT( 0 && "VTBL_ARRAY not found" );
#endif

//��`�t���O���m�F
#ifndef TEMPLATE_VTBL_LOADED

//���X�g�ɕϊ�
#define VTBL_LIST BOOST_PP_TUPLE_TO_LIST( BOOST_PP_ARRAY_SIZE(VTBL_ARRAY), BOOST_PP_ARRAY_DATA(VTBL_ARRAY))

//�����W�J
#define EXCUTES_ARGS_OPEN(argArray)  BOOST_PP_TUPLE_REM( BOOST_PP_ARRAY_SIZE(argArray) ) BOOST_PP_ARRAY_DATA(argArray)
#define EXCUTES_ARGS(argArray) BOOST_PP_EXPR_IF( BOOST_PP_ARRAY_SIZE(argArray),\
                                            EXCUTES_ARGS_OPEN(argArray))



//�֐����W�J
#define VTBL_LIST_NAME(elem) BOOST_PP_TUPLE_ELEM(3,0,elem)
//�߂�l�^�W�J
#define VTBL_LIST_RETURN(elem) BOOST_PP_TUPLE_ELEM(3,1,elem)
//����Array�W�J
#define VTBL_LIST_ARGS_ARRAY(elem) BOOST_PP_TUPLE_ELEM(3,2,elem)
//����Array�̌�
#define VTBL_LIST_ARGS_ARRAY_SIZE(elem) BOOST_PP_ARRAY_SIZE(VTBL_LIST_ARGS_ARRAY(elem))
//�����^�񋓓W�J
#define VTBL_LIST_ARGS(elem) EXCUTES_ARGS(VTBL_LIST_ARGS_ARRAY(elem))
//������1�ȏ�̎��R���}�ɓW�J
#define VTBL_LIST_COMMA(elem) BOOST_PP_COMMA_IF( VTBL_LIST_ARGS_ARRAY_SIZE(elem) )

//�֐��������X�g��W�J
#define FUNCTION_ARGS_I(z,n,elem) BOOST_PP_ARRAY_ELEM(n, VTBL_LIST_ARGS_ARRAY(elem)) a##n \
  BOOST_PP_COMMA_IF( BOOST_PP_LESS(n, BOOST_PP_SUB( VTBL_LIST_ARGS_ARRAY_SIZE(elem) ,1)))
#define VTBL_LIST_FUNCTION_ARGS(elem) BOOST_PP_REPEAT( VTBL_LIST_ARGS_ARRAY_SIZE(elem), FUNCTION_ARGS_I, elem)

//VTBL�W�J
#define STRUCT_I(r, data,elem) \
  VTBL_LIST_RETURN(elem) (*VTBL_LIST_NAME(elem))(void const * VTBL_LIST_COMMA(elem) VTBL_LIST_ARGS(elem));
#define VTBL_STRUCT { BOOST_PP_LIST_FOR_EACH(STRUCT_I, data, VTBL_LIST) }

//��������`�p
#define EXCUTES(r,data, elem) \
static VTBL_LIST_RETURN(elem) VTBL_LIST_NAME(elem)(void const* this_ VTBL_LIST_COMMA(elem) VTBL_LIST_FUNCTION_ARGS(elem)){\
  return static_cast<data const *>(this_)->VTBL_LIST_NAME(elem)( BOOST_PP_ENUM_PARAMS( VTBL_LIST_ARGS_ARRAY_SIZE(elem), a) ); \
}
#define VTBL_INITALIZER(templateName, VtblType, VtblValue) { static VtblType VtblValue; BOOST_PP_LIST_FOR_EACH(EXCUTES, templateName, VTBL_LIST) }

//static��`
#define INIALIZES_I(r, data,i, elem) \
  BOOST_PP_COMMA_IF( BOOST_PP_NOT_EQUAL(0,i)) &data::VTBL_LIST_NAME(elem)
#define VTBL_INIT( ClassName ) { BOOST_PP_LIST_FOR_EACH_I(INIALIZES_I, ClassName, VTBL_LIST) }


//��`�t���O�����Ă�
#define TEMPLATE_VTBL_LOADED

#endif

//////////////////////////////////////////////////////////////////
//�g����?
/*
  //1.VTBL_ARRAY���`����BboostPreprosser��Array���f�����Q��
  *
   #define VTBL_ARRAY (2, ( \
                         (quack, void, (0, BOOST_PP_NIL)),\
                         (bark, int, (1,(int))) \
                         ))

////////////////////////////////////////////////////////////////
  //2. Duck�N���X��VTBL_ARRAY�𖞂����悤��`�A����
struct Duck{
  void quack() const{ cout << "gaaa" << endl; }
  int bark(int i) const{ cout << "gagaga" << endl;  return i; }
};

struct Foo{
  void quack() const{ cout << "...." << endl; }
  int bark(int i) const{ cout << "fufufu" << endl; return i*i;}
};

////////////////////////////////////////////////////////////////
//3, ���̃t�@�C��(TemplateVTBL.hpp)���C���N���[�h
  #include "TemplateVTBL_MacroLoad.hpp"

////////////////////////////////////////////////////////////////
//4, ���b�p�[�N���X���`

struct Ducky
{
    struct VTBL VTBL_STRUCT;

  //�ȉ��ɓW�J�����
   //{
   // void (*quack)(void const *);
   // int (*bark)(void const *, int);
  //};



  template<typename T>
  struct VTBLInitalizer VTBL_INITALIZER(T, VTBL, vtbl_);

  //�ȉ��ɓW�J
  //  {
  // static VTBL vtbl_;
  //  static void quack(void const* this_)  {
  //       static_cast<T const *>(this_)->func();  }
  //    static int bark(void const* this_, int a0){
  //       static_cast<T const *>(this_)->bark(a0); }
  // };


  //�R���X�g���N�^
  template<typename T>
  Ducky(T& duckey)
       : this_(&duckey),
         vptr_(&VTBLInitalizer<T>::vtbl_)
    {}

  void quack() const
    {
      vptr_->quack(this_);
    }

  int bark(int i) const
  {
      return vptr_->bark(this_, i);
   }

  void* this_;
  VTBL* vptr_;
};

template<typename T>
Ducky::VTBL Ducky::VTBLInitalizer<T>::vtbl_ =  VTBL_INIT( Ducky::VTBLInitalizer<T> );

  //�ȉ��ɓW�J
// {
  //&Ducky::VTBLInitalizer<T>::quack,
  //&Ducky::VTBLInitalizer<T>::bark
//};


  ////////////////////////////////////////////////////////////////
  //5. �}�N���̂��|���w�b�_���C���N���[�h
  #include "TemplateVTBL_MacoroClear.hpp"

 ////////////////////////////////////////////////////////////////
  //6.�g��
 void func(Ducky ducky){
  // ducky �͎Q�ƂƂ��ċ@�\
  ducky.quack();
  ducky.bark();
}

int main(){
  Duck duck;
  Foo foo;
  func(duck);
  func(foo);
  return 0;
}

*/


