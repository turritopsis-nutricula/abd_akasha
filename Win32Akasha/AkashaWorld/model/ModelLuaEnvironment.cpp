#include "ModelLuaEnvironment.hpp"

#include "ModelLuaConstract.hpp"

using akasha::lua::ModelLuaEnvironment;

bool ModelLuaEnvironment::setup(
		std::string const& luaCode,
		boost::shared_ptr<model::ModelObserver> const& statusBuffer,
		boost::filesystem::path const& modelFilePath,
		boost::function<void (bool)>&& simulationContext)
{
	typedef std::string StringType;

	m_simulationContext = std::move(simulationContext);

	boost::fill(m_inputDown,  false);
	boost::fill(m_inputUp,    false);
	boost::fill(m_inputBuffer,false);

	m_lua = boost::in_place();
	lua_State* l;

	try
	{
		l = m_lua->open();
		luaL_openlibs(l);
	} catch (...) {
		throw std::runtime_error(
				"ModelLuaEnviroment setup()  setupError");
	}

	{ //ロードテーブル初期化

		StringType pathTable("./?.lua;./?;");
		auto modelDir = modelFilePath.branch_path().generic_string();
		pathTable += modelDir;
		pathTable += "/?.lua;";
		pathTable += modelDir;
		pathTable += "/?;";

		boost::optional<StringType> package_path = luabind::object_cast<boost::optional<StringType>>(
				luabind::globals(l)[ "package"]["path"]
				);
		if (package_path)
			luabind::globals(l)[ "package" ]["path"] =  pathTable+(*package_path);
		else
			luabind::globals(l)[ "package" ]["path"] = pathTable;
	}


	//モデルluaの構築
	lua::binded::ConstractModelLuaEnvironment(
			l,
			statusBuffer,
			boost::ref(m_irrEx),
			boost::bind(&ModelLuaEnvironment::registOutString, this, _1, _2),
			boost::ref(*this)
			);



	if (luaL_dostring(l, luaCode.c_str() ) == 0) {
		//Luaコード実行成功
		return true;
	}
	else {
		return false;
	}
}


//OnInit Call
void ModelLuaEnvironment::onInit( model::value::ModelValueBuffer& values)
{
	if ( existLua() )
	{
		try
		{
			resetInputStatus();
			luabind::call_function<void>( m_lua->getState(), "OnInit" );
		}
		catch( luabind::error const& e)
		{
			std::cout << e.what() << std::endl;
		}

		readModelValueToLua(values);
	}
}

//ModelLua OnFrame
void ModelLuaEnvironment::onFrame(model::value::ModelValueBuffer& values)
{
	if ( existLua() )
	{
		//Luaにモデル変数書き出し
		writeModelValueToLua(values);
		resetInputStatus();

		//関数メインを呼び出し
		try
		{
			luabind::call_function<void>( m_lua->getState(), "main" );
		}
		catch ( luabind::error const& e )
		{
			luabind::object errorMsg(
					luabind::from_stack( e.state(), -1 ) );
			std::stringstream ss;
			ss << "Error:" << errorMsg << std::endl;
		}
		drawOutMessage();

		//Luaからモデル読み出し
		readModelValueToLua( values );
	}

}

//ModelValues write
void ModelLuaEnvironment::writeModelValueToLua( model::value::ModelValueBuffer& values)
{
	for(auto const& v: values)
	{
		luabind::globals( m_lua->getState() )[v.getName().c_str()] =
			v.getVolum();
	}
}
//ModelValues read
void ModelLuaEnvironment::readModelValueToLua( model::value::ModelValueBuffer& values)
{
	for(auto& v : values)
	{
		auto value =
			luabind::object_cast<boost::optional<float>>(
					luabind::globals( m_lua->getState() )[v.getName().c_str()] );
		if ( value )
			v.setVolum( value.get() );
	}
}



