#ifndef TEMPLATE_VTBL_EX__HPP
#define TEMPLATE_VTBL_EX__HPP


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
#include <boost/preprocessor/facilities/is_1.hpp>

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

#if !defined(CONCEPT_ARRAY)
BOOST_STATIC_ASSERT( 0 && "CONCEPT_ARRAY not found" );
#endif

//��`�t���O���m�F
#ifndef TEMPLATE_VTBL_EX_LOADED

//���X�g�ɕϊ�
#define VTBL_LIST BOOST_PP_TUPLE_TO_LIST( BOOST_PP_ARRAY_SIZE(CONCEPT_ARRAY), BOOST_PP_ARRAY_DATA(CONCEPT_ARRAY))

//�����W�J
#define EXCUTES_ARGS_OPEN(argArray)  BOOST_PP_TUPLE_REM( BOOST_PP_ARRAY_SIZE(argArray) ) BOOST_PP_ARRAY_DATA(argArray)
#define EXCUTES_ARGS(argArray) BOOST_PP_EXPR_IF( BOOST_PP_ARRAY_SIZE(argArray),\
                                            EXCUTES_ARGS_OPEN(argArray))



//const�t���O�W�J
#define VTBL_LIST_CONST_FLAG(elem) BOOST_PP_TUPLE_ELEM(4,0,elem)
//�֐����W�J
#define VTBL_LIST_NAME(elem) BOOST_PP_TUPLE_ELEM(4,1,elem)
//�߂�l�^�W�J
#define VTBL_LIST_RETURN(elem) BOOST_PP_TUPLE_ELEM(4,2,elem)
//����Array�W�J
#define VTBL_LIST_ARGS_ARRAY(elem) BOOST_PP_TUPLE_ELEM(4,3,elem)
//����Array�̌�
#define VTBL_LIST_ARGS_ARRAY_SIZE(elem) BOOST_PP_ARRAY_SIZE(VTBL_LIST_ARGS_ARRAY(elem))
//�����^�񋓓W�J
#define VTBL_LIST_ARGS(elem) EXCUTES_ARGS(VTBL_LIST_ARGS_ARRAY(elem))
//������1�ȏ�̎��R���}�ɓW�J
#define VTBL_LIST_COMMA(elem) BOOST_PP_COMMA_IF( VTBL_LIST_ARGS_ARRAY_SIZE(elem) )
//const�t���O��1�ȊO�Ȃ�ΓW�J����
#define VTBL_LIST_CONST(elem) BOOST_PP_EXPR_IIF(BOOST_PP_NOT(BOOST_PP_IS_1( VTBL_LIST_CONST_FLAG(elem))), const)
//const�t���O��1�ȊO�Ȃ�_c�A1�Ȃ��_nc�ɓW�J
#define VTBL_LIST_CONST_FLAGNAME(elem) BOOST_PP_IIF(BOOST_PP_NOT(BOOST_PP_IS_1( VTBL_LIST_CONST_FLAG(elem))), _c, _nc)
//const�t���O�t���ł����_c�A�����łȂ����_nc������ɕt���֐����ɓW�J
#define VTBL_LIST_NAME_WITH_FLAG(elem) BOOST_PP_CAT(VTBL_LIST_NAME(elem), VTBL_LIST_CONST_FLAGNAME(elem))

//�֐��������X�g��W�J
#define FUNCTION_ARGS_I(z,n,elem) BOOST_PP_ARRAY_ELEM(n, VTBL_LIST_ARGS_ARRAY(elem)) a##n \
  BOOST_PP_COMMA_IF( BOOST_PP_LESS(n, BOOST_PP_SUB( VTBL_LIST_ARGS_ARRAY_SIZE(elem) ,1)))
#define VTBL_LIST_FUNCTION_ARGS(elem) BOOST_PP_REPEAT( VTBL_LIST_ARGS_ARRAY_SIZE(elem), FUNCTION_ARGS_I, elem)

//// VTBL Struct
//VTBL�W�J
#define STRUCT_I(r, data,elem) \
  VTBL_LIST_RETURN(elem) (*VTBL_LIST_NAME_WITH_FLAG(elem))(void VTBL_LIST_CONST(elem)* VTBL_LIST_COMMA(elem) VTBL_LIST_ARGS(elem));
#define VTBL_STRUCT { BOOST_PP_LIST_FOR_EACH(STRUCT_I, data, VTBL_LIST) }

//////Inializer Struct �錾
//��������`�p
#define EXCUTES(r,data, elem) \
static VTBL_LIST_RETURN(elem) VTBL_LIST_NAME_WITH_FLAG(elem)(void VTBL_LIST_CONST(elem)* this_ VTBL_LIST_COMMA(elem) VTBL_LIST_FUNCTION_ARGS(elem)){\
  return static_cast<data VTBL_LIST_CONST(elem)*>(this_)->VTBL_LIST_NAME(elem)( BOOST_PP_ENUM_PARAMS( VTBL_LIST_ARGS_ARRAY_SIZE(elem), a) ); \
}
#define VTBL_INITALIZER(templateName, VtblType, VtblValue) { static VtblType VtblValue; BOOST_PP_LIST_FOR_EACH(EXCUTES, templateName, VTBL_LIST) }

//////VTBL��InializerStruct�����錾
#define VTBL_STRUCT_AND_INIALIZER( vtblType, vtblInitType, Template, vtblValue) \
  struct vtblType VTBL_STRUCT; \
  template<typename Template> struct vtblInitType VTBL_INITALIZER(Template, vtblType, vtblValue)

//////VTBL��`
//static��`
#define INIALIZES_I(r, data,i, elem) \
  BOOST_PP_COMMA_IF( BOOST_PP_NOT_EQUAL(0,i)) &data::VTBL_LIST_NAME_WITH_FLAG(elem)
#define VTBL_INIT( ClassName ) { BOOST_PP_LIST_FOR_EACH_I(INIALIZES_I, ClassName, VTBL_LIST) }
#define VTBL_INIT_DEFINE( masterType, vtblType, vtblInitType, vtblValue) \
  template<typename TempType> masterType ## :: ## vtblType \
    masterType ## :: ## vtblInitType ## < TempType > ## :: ## vtblValue = \
      VTBL_INIT( masterType ## :: ## vtblInitType ## < TempType > )

//��`�t���O�����Ă�
#define TEMPLATE_VTBL_EX_LOADED

#endif //TEMPLATE_VTBL_EX_LOADED

//////////////////////////////////////////////////////////////////
//�g����?
/*
  //1.VTBL_ARRAY���`����BboostPreprosser��Array���f�����Q��
  // (NonConstFlag, FunctionName, RetureType, ArgTypeArray)

   #define VTBL_ARRAY (3, ( \
                         (0,quack, void, (0, BOOST_PP_NIL)),\
                         (0,bark, int, (2,(int, int))) \
                         (1,eat,  int, (1, (const long))) \
                         ))

////////////////////////////////////////////////////////////////
  //2. Duck�N���X��VTBL_ARRAY�𖞂����悤��`�A����
struct Duck{
  void quack() const{ cout << "gaaa" << endl; }
  int bark(int i,int j) const{ cout << "gagaga" << endl;  return i; }
  int eat(const long l){ return l; }
};

struct Foo{
  int x;
  void quack() const{ cout << "...." << endl; }
  int bark(int i,int j) const{ cout << "fufufu" << endl; return i*i;}
  int eat(const long l){ x += l; return x;}
};

////////////////////////////////////////////////////////////////
//3, ���̃t�@�C��(TemplateVTBL.hpp)���C���N���[�h
  #include "TemplateVTBL_Ex_MacrLoad.hpp"

////////////////////////////////////////////////////////////////
//4, ���b�p�[�N���X���`

struct Ducky
{
   VTBL_STRUCT_AND_INIALIZER( VTBL, VTBLInitalizer, T, vtbl_);


  //�ȉ��ɓW�J�����
  //  struct VTBL
  // {
  // void (*quack_c)(void const *);
  // int (*bark_c)(void const *, int);
  // int (*eat_nc)(void *, const long);
  //};
  //template<typename T>
  //struct VTBLInitalizer
  //{
  // static VTBL vtbl_;
  //  static void quack_c(void const* this_)  {
  //       static_cast<T const *>(this_)->func();  }
  //  static int bark_c(void const* this_, int a0){
  //       static_cast<T const *>(this_)->bark(a0); }
  //  static int eat_nc(void* this_, const long a0){
  //       static_cast<T *>(this_)->eat(a0); }
  // };


  //�R���X�g���N�^
  template<typename T>
  Ducky(T& duckey)
       : this_(&duckey),
         vptr_(&VTBLInitalizer<T>::vtbl_)
    {}

  void quack() const
    {
      vptr_->quack_c(this_);
    }

  int bark(int i) const
  {
      return vptr_->bark_c(this_, i);
   }

  int eat(const long l)
  {
     return vptr_->eat_nc(this_, l);
  }

  void* this_;
  VTBL* vptr_;
};

VTBL_INIT_DEFINE( Ducky, VTBL, VTBLInitalizer, vtbl_ );
 //�ȉ��ɓW�J
//template<typename TempType>
//Ducky::VTBL Ducky::VTBLInitalizer<T>::vtbl_ = Ducky::VTBLInitalizer<T>
  // {
  //  &Ducky::VTBLInitalizer<TempType>::quack_c,
  //  &Ducky::VTBLInitalizer<TempType>::bark_c,
  //  &Ducky::VTBLInitalizer<TempType>::eat_nc,
  //};


  ////////////////////////////////////////////////////////////////
  //5. �}�N���̂��|���w�b�_���C���N���[�h
  #include "TemplateVTBL_EX_MacroClear.hpp"

 ////////////////////////////////////////////////////////////////
  //6.�g��
 int func(Ducky ducky){
  // ducky �͎Q�ƂƂ��ċ@�\
  ducky.quack();
  return  ducky.eat( ducky.bark() );
}

int main(){
  Duck duck;
  Foo foo;
  func(duck);
  func(foo);
  return 0;
}

*/


