
set(ExeName "Yururu")
# Headers {{{
set(Headers
	AkashaApplication/AkashaApplication.h
	AkashaApplication/Instantiater.hpp
	AkashaApplication/SoundManager.hpp
	AkashaApplication/TemplateVTBL_Ex_MacroClear.hpp
	AkashaApplication/TemplateVTBL_Ex_MacroLoad.hpp
	AkashaApplication/TemplateVTBL_MacroClear.hpp
	AkashaApplication/TemplateVTBL_MacroLoad.hpp
	AkashaApplication/lua/LuaEnvironment.hpp
	AkashaApplication/lua/luabindExConverter.hpp
	AkashaApplication/net/FrontEnd.hpp
	AkashaApplication/net/Network.hpp
	AkashaApplication/network/CommunicateData.hpp
	AkashaApplication/network/UDP_connection.hpp
	AkashaApplication/network/WorldData.hpp
	AkashaApplication/network/client/Client.h
	AkashaApplication/network/connection.hpp
	AkashaApplication/network/server/STUN_Observer.hpp
	AkashaApplication/network/server/Server.h
	AkashaApplication/network/utily.hpp
	AkashaApplication/resourceEntity.h
	AkashaSystem/AkashaLogger.hpp
	AkashaSystem/AkashaSystem.hpp
	AkashaSystem/InputInterface.hpp
	AkashaSystem/SystemConstant.hpp
	AkashaWorld/AkashaWorld.h
	AkashaWorld/BulletExtention.h
	AkashaWorld/BulletHfFluid/btHfFluid.h
	AkashaWorld/BulletHfFluid/btHfFluidBuoyantShapeCollisionAlgorithm.h
	AkashaWorld/BulletHfFluid/btHfFluidBuoyantConvexShape.h
	AkashaWorld/BulletHfFluid/btHfFluidBuoyantConcaveCollisionAlgorithm.h
	AkashaWorld/BulletHfFluid/btHfFluidBuoyantConvexCollisionAlgorithmBase.hpp
	AkashaWorld/BulletHfFluid/btHfFluidCollisionShape.h
	AkashaWorld/BulletHfFluid/btHfFluidRigidCollisionAlgorithm.h
	AkashaWorld/BulletHfFluid/btHfFluidRigidCollisionConfiguration.h
	AkashaWorld/BulletHfFluid/btHfFluidRigidDynamicsWorld.h
	AkashaWorld/BulletWorldManager.h
	AkashaWorld/IrrExtention.h
	AkashaWorld/LuaUtility.h
	AkashaWorld/PlayerView.hpp
	AkashaWorld/Receptor/LandReceptor.hpp
	AkashaWorld/Receptor/Receptors.hpp
	AkashaWorld/RenderType.hpp
	AkashaWorld/Toy/Toy.hpp
	AkashaWorld/WorldCommand.hpp
	AkashaWorld/akashaWorldCommon.h
	AkashaWorld/akashaWorldType.h
	AkashaWorld/avatar/AvatarBase.hpp
	AkashaWorld/btIrrDebugDraw.h
	AkashaWorld/bullet/ConstraintHelper.hpp
	AkashaWorld/bullet/ConstraintWrapper.hpp
	AkashaWorld/bullet/FractureDynamics.hpp
	AkashaWorld/bullet/HfFluidDynamics.hpp
	AkashaWorld/bullet/btFractureBody.hpp
	AkashaWorld/bullet/btFractureDynamicsWorld.hpp
	AkashaWorld/bullet/btFractureWelding.hpp
	AkashaWorld/bullet/btProfileDump.hpp
	AkashaWorld/convert/ConvertFunction.hpp
	AkashaWorld/convert/ConvertImpl_Bullet_Irrlicht.hpp
	AkashaWorld/convert/ConvertImpl_Bullet_X3D.hpp
	AkashaWorld/convert/ConvertImpl_narowchar_widechar.hpp
	AkashaWorld/convert/ConvertMPL.hpp
	AkashaWorld/convert/ConvertMeshBuffer.hpp
	AkashaWorld/convert/Convert_Irrlicht_X3D.hpp
	AkashaWorld/detection/DetectionWorld.hpp
	AkashaWorld/detection/ObjectCapture.hpp
	AkashaWorld/detection/ShotBase.hpp
	AkashaWorld/detection/figuer/BeamFiguer.hpp
	AkashaWorld/entity/EntityReceptor.hpp
	AAkashaWorld/environment/EnvironmentLand.hpp
	AkashaWorld/environment/LandWater.hpp
	AkashaWorld/environment/Landform.hpp
	AkashaWorld/model/ElementModel.hpp
	AkashaWorld/model/EnergyLinkGraph.hpp
	AkashaWorld/model/ModelConnectGraph.hpp
	AkashaWorld/model/ModelControlKey.hpp
	AkashaWorld/model/ModelLuaConstract.hpp
	AkashaWorld/model/ModelLuaEnvironment.hpp
	AkashaWorld/model/ModelValue.hpp
	AkashaWorld/model/ValueManifold.hpp
	AkashaWorld/model/builder/ControlKeyBuilder.hpp
	AkashaWorld/model/builder/ElementChunkBuilder.hpp
	AkashaWorld/model/builder/ElementDataBuilder.hpp
	AkashaWorld/model/builder/UserValueBuilder.hpp
	AkashaWorld/model/builder/ValueManifoldBuilder.hpp
	AkashaWorld/model/element/ConnectType.hpp
	AkashaWorld/model/element/ElementData.hpp
	AkashaWorld/model/element/energy/EnergyPort.hpp
	AkashaWorld/model/element/energy/GearAssemblyBuilder.hpp
	AkashaWorld/model/element/energy/Harnnes.hpp
	AkashaWorld/model/element/energy/LinkType.hpp
	AkashaWorld/model/element/energy/TractionLink.hpp
	AkashaWorld/model/element/figuer/BasicElementFiguer.hpp
	AkashaWorld/model/element_v2/CreateInfo.hpp
	AkashaWorld/model/element_v2/ElementBuffer.hpp
	AkashaWorld/model/element_v2/ElementConcept.hpp
	AkashaWorld/model/element_v2/behavior/PhysicsBehavior.hpp
	AkashaWorld/model/element_v2/behavior/FlexibleJointBehavior.hpp
	AkashaWorld/model/element_v2/behavior/TractionJointBehavior.hpp
	AkashaWorld/model/element_v2/detail/ElementBase.hpp
	AkashaWorld/model/element_v2/detail/ElementParameter.hpp
	AkashaWorld/model/element_v2/event/event.hpp
	AkashaWorld/model/element_v2/type_fwd.hpp
	AkashaWorld/model/fluidDynamics/fluidDynamics.hpp
	AkashaWorld/model/loader/RCFileLoader.hpp
	AkashaWorld/model/loader/define/FusionAdapt.hpp
	AkashaWorld/model/loader/define/ModelDefineStruct.hpp
	AkashaWorld/model/loader/parser/BlockSplitGrammar.hpp
	AkashaWorld/model/loader/parser/BodyBlockGrammar.hpp
	AkashaWorld/model/loader/parser/ExpressionGrammar.hpp
	AkashaWorld/model/loader/parser/KeyBlockGrammar.hpp
	AkashaWorld/model/loader/parser/ValBlockGrammar.hpp
	AkashaWorld/model/loader/parser/annotation.hpp
	AkashaWorld/model/loader/parser/commentSkipper.hpp
	AkashaWorld/other/OtherElementModel.hpp
	Win32++/include/controls.h
	Win32++/include/cstring.h
	Win32++/include/default_resource.h
	Win32++/include/dialog.h
	Win32++/include/docking.h
	Win32++/include/file.h
	Win32++/include/frame.h
	Win32++/include/gdi.h
	Win32++/include/listview.h
	Win32++/include/mdi.h
	Win32++/include/menu.h
	Win32++/include/propertysheet.h
	Win32++/include/rebar.h
	Win32++/include/ribbon.h
	Win32++/include/shared_ptr.h
	Win32++/include/socket.h
	Win32++/include/statusbar.h
	Win32++/include/stdcontrols.h
	Win32++/include/tab.h
	Win32++/include/taskdialog.h
	Win32++/include/thread.h
	Win32++/include/toolbar.h
	Win32++/include/treeview.h
	Win32++/include/wceframe.h
	Win32++/include/wcestddef.h
	Win32++/include/webbrowser.h
	Win32++/include/wincore.h
	Win32++/include/winutils.h
	src/AkashaFrames.h
	src/AkashaMainWindow.hpp
	src/DebugConsol.hpp
	src/DebugWindow/DebugWindow.hpp
	src/DirectInput.h
	src/InputBridge/InputBridge.hpp
	src/LogWindow/LogList.hpp
	src/LogWindow/LogWindow.hpp
	src/Network/ChatListView.hpp
	src/Network/NetworkDialog.hpp
	src/Network/SelectorDialog.hpp
	src/default_resource.h
	src/res/resource.h
	src/res/resource.rc
	)
#}}}

set (implList
	AkashaWorld/model/element_v2/ChipElement.ipp
	AkashaWorld/model/element_v2/CoreElement.ipp
	AkashaWorld/model/element_v2/RimElement.ipp
	AkashaWorld/model/element_v2/RudderElement.ipp
	AkashaWorld/model/element_v2/TrimElement.ipp
	AkashaWorld/model/element_v2/WheelElement.ipp
	)

set (
	SourceList
	AkashaSystem/AkashaSystem.cpp
	AkashaSystem/SystemConstant.cpp
	AkashaApplication/AkashaApplication.cpp
	AkashaApplication/net/Network.cpp
	AkashaWorld/AkashaWorld.cpp
	AkashaWorld/AkashaWorld_1.cpp
# 	AkashaWorld/BulletHfFluid/btHfFluid.cpp
# 	AkashaWorld/BulletHfFluid/btHfFluidBuoyantConcaveCollisionAlgorithm.cpp
# 	AkashaWorld/BulletHfFluid/btHfFluidBuoyantConvexShape.cpp
# 	AkashaWorld/BulletHfFluid/btHfFluidBuoyantShapeCollisionAlgorithm.cpp
# 	AkashaWorld/BulletHfFluid/btHfFluidCollisionShape.cpp
# 	AkashaWorld/BulletHfFluid/btHfFluidRigidCollisionAlgorithm.cpp
# 	AkashaWorld/BulletHfFluid/btHfFluidRigidCollisionConfiguration.cpp
# 	AkashaWorld/BulletHfFluid/btHfFluidRigidDynamicsWorld.cpp
	AkashaWorld/PlayerView.cpp
	AkashaWorld/btIrrDebugDraw.cpp
	AkashaWorld/bullet/fluidDynamics/btHfFluidRigidCollisionConfiguration.cpp
	AkashaWorld/bullet/fluidDynamics/buoyant/BuoyantAuthoizer.cpp
	AkashaWorld/bullet/fluidDynamics/fluid/btHfFluid.cpp
	AkashaWorld/bullet/fluidDynamics/fluid/btHfFluidCollisionShape.cpp
	AkashaWorld/bullet/fluidDynamics/algorithm/btFluidBodyCollisionAlgorithm.cpp
	AkashaWorld/bullet/PhysicsWorld.cpp
	AkashaWorld/effect/SmorkEffect.cpp
	AkashaWorld/model/element_v2/behavior/FlexibleJointBehavior.cpp
	AkashaWorld/detection/figuer/BeamFiguer.cpp
	AkashaWorld/environment/EnvironmentLand.cpp
	AkashaWorld/model/ElementModel.cpp
	AkashaWorld/model/ModelConnectGraph.cpp
	AkashaWorld/model/EnergyLinkGraph.cpp
	AkashaWorld/model/builder/ElementChunkBuilder.cpp
	AkashaWorld/model/builder/BtShapeBuilder.cpp
	AkashaWorld/model/element/figuer/BasicElementFiguer.cpp
	AkashaWorld/model/element_v2/ElementBuffer.cpp
	AkashaWorld/model/element_v2/behavior/PhysicsBehavior.cpp
	AkashaWorld/model/element_v2/behavior/TractionJointBehavior.cpp
	AkashaWorld/model/element_v2/detail/ElementParameter.cpp
	AkashaWorld/model/loader/RCFileLoader.cpp
	AkashaWorld/model/BridgeLink/BridgeLink.cpp
	AkashaWorld/model/ModelLuaEnvironment.cpp
	src/Win32AkashaApplication.cpp
	src/main.cpp
	)
#Add Resource File
set (
	SourceList
	${SourceList}
	src/res/resource.rc
	)

# buildConfig
configure_file(
	${PROJECT_SOURCE_DIR}/Win32Akasha/BuildConfig.hpp.in
	${PROJECT_SOURCE_DIR}/Win32Akasha/BuildConfig.hpp
	)

# include directory
set(BulletVersion 283)
include_directories(
	${WorksDirectory}/include/irrlicht
# 	${WorksDirectory}/include/Riceball
	${WorksDirectory}/bullet_${BulletVersion}
	${WorksDirectory}/include/luabind_091a
	${WorksDirectory}/include/luajit_202
	)

include_directories(d:/develop/irrlicht-1.8.1/source/Irrlicht)
include_directories(
# 	${PROJECT_SOURCE_DIR}/foreplay
	${PROJECT_SOURCE_DIR}/Win32Akasha/Win32++/include
	)

link_directories(${PROJECT_SOURCE_DIR}/foreplay/bin/Debug)

add_executable(${ExeName} ${SourceList})

#bullet lib prefix
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(BulletLibPrefix ${BulletVersion}_sd)
else()
	set(BulletLibPrefix ${BulletVersion}_s)
endif()
# library
set( AddLibraryes )
if(USE_STATIC_LIBLARY)
	set( AddLibraryes
		dinput8
		dxguid
		dxerr9
		dxerr8
		)
endif()

target_link_libraries(
	${ExeName}
	BulletDynamics_${BulletLibPrefix}
	BulletCollision_${BulletLibPrefix}
	LinearMath_${BulletLibPrefix}
	ConvexDecomposition_${BulletLibPrefix}
	${LibNainaiName} # Nainai
	${LibWispName} # Wisp
	${LibIrrName} # Irrlicht
	${LibForeplayName} # foreplay
	luabind
	luajit
	${AddLibraryes}
	comctl32
	winmm
	opengl32
	gdi32
	comdlg32
	Ole32
	ws2_32
	mswsock
	)

set(link_boost_list
	filesystem
	coroutine
	context
	wserialization
	serialization
	date_time
	thread
	atomic
	timer
	chrono
	system)

link_boost(${ExeName} "${link_boost_list}")
