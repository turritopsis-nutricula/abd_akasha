--[[
変数名テーブル
VarNames = {varName1, varName2, ...};

エレメントテーブル
Elements = {
  id = {
    string factor1=要素1,
    string factor2=要素2,
    number elementNumber = id,
    number parentNumber = id,
    param = {
      string [attributeName] = value
    }},...}

    クライアントCオブジェクトポインタ
    userdata  Client;


    C_addElement(
      userdata Client,
      {
        number direct = 接続方向,
        number elementType = エレメント種別,
        number parentID = 親のID,
        number context = エレメントコンテキスト,
        number shape = エレメント形状,
        number jointType = ジョイントタイプ,
        number jointAxis = ジョイント軸,
        number mass = 質量,
        number edurance = チップ耐久度,
        number enager = チップエネルギー,
        number airParameter0 = 空力定数1,
        number airParameter1 = 空力定数2,
        number airParameter2 = 空力定数3,
        table mesh = {
          string name; メッシュファイル名,
          table position ={number X,number Y,number Z};
          table rotation = {number X,number Y,number Z};
          table scale = {number X,number Y,number Z};
        };
        table param = パラメータテーブル
        table sign = パラメータ符号テーブル(booleanテーブル)
        [, table 初期運動パラメータ]
      }
    )

    numberで固定値、文字列でモデル変数値として扱う
    パラメータテーブル = {
      name = Name
      color = Color,
      angle = Angle,
      damper = Damper,
      spring = Spring,
      power = Power,
      brake = Break,
      option = Option,
      effect = Effect,
      user1 = user1,
      user2 = user2
    }
    ]]
    --################################################################
    --################################################################

    --本体コード列挙体との同期マップ群################################################################
    --接続方向名のマップ
    --ElementDirect
    directType = {
      "s",                      -- South
      "e",                      -- East
      "n",                      -- North
      "w",                      -- West
      "face",                   -- ForeFace
      "back",                   -- BackFace
    };
    --接続軸名マップ
    --ElementJointAxis
    axisType = {
      "x",
      "y",
      "z",
    };
    --エレメント形状
    --ElementCollisionShape
    shapeType = {
      "panel",
      "disc",
      "shaft"
    };

    --エレメントタイプ
    -- ElementType
    elementType = {
      "core",
      "chip",
      "rudder",
      "trim",
      "rim",
      "wheel",
      "jet"
    };

    --ジョイントタイプ
    --ElementJointType
    jointType = {
      "common",
      "shaft"
    };

        --列挙型対応マップからの値取得
    function getToMap(mapTable, str)
      for i=1, table.getn(mapTable) do
        if (mapTable[i]==str) then
          return i-1;
        end --if
      end --for
    end --getToMap()
    
    --\ 本体コード列挙体との同期マップ群################################################################


    -- エレメント追加ファンク################
    function addElement(data,elementTypeStr,directStr)
      
      local meshTable = {};
      do --meshTable構成
        local f = meshData[ meshNameMap[ elementTypeStr ] ];
        meshTable.name = meshNameMap[ elementTypeStr ]..".x";
        for i, name in pairs{"position", "rotation", "scale"} do
          meshTable[ name ] = {
            X = f[ name ][1],
            Y = f[ name ][2],
            Z = f[ name ][3]
          };
        end --for
      end --/meshTable構成

      local paramTable = {};
      local paramSign = {};
      do --paramTableの構成
        for n,value in pairs(data.param) do
          local sign, v;
          if string.sub(value, 1,1)=="-" then
            sign = true;
            v = string.sub(value, 2);
          else
            sign = false;
            v = value;
          end --if
          paramTable[ string.lower(n) ] = v;
          paramSign[ string.lower(n) ] = sign;
        end --for value
      end --/paramTableの構成

      --outDebug( "addElement:"..elementTypeStr..":parent=>"..data.parentNumber);

      --C側の登録関数呼び出し
      return C_addElement(
        Client,
        {
          direct = getToMap(directType, directStr );
          elementType = getToMap(elementType, elementTypeStr );

          
          jointAxis = axisMap[ elementTypeStr ];
          jointType = jointMap[ elementTypeStr ];
          shape = shapeMap[ elementTypeStr ];
          mass = massMap[ elementTypeStr ];

          airParameter0 = airParame0Map[ elementTypeStr ];
          airParameter1 = airParame1Map[ elementTypeStr ];
          airParameter2 = airParame2Map[ elementTypeStr ];
          
          parentID = data.parentNumber;
          mesh = meshTable;
          param = paramTable;
          sign = paramSign;
        });
    end

    --情報用ツリー構成再帰関数
    function makeElementTree(t,linerDataIndex, linerData)
      if not(linerData[linerDataIndex]) then
        return;
      end
      local Id = linerData[ linerDataIndex ].elementNumber;
      local parentId = linerData[ linerDataIndex ].parentNumber;
      if (t.id == parentId) then
  --      outDebug(linerDataIndex..":hit");
        local elem = {id = Id, parent= t, child = {}};
        makeElementTree( elem,linerDataIndex+1, linerData);
        table.insert(t.child, elem);
        return;
      else
--        outDebug("non:");
        makeElementTree( t.parent, linerDataIndex, linerData);
      end --if
    end --makeElementTree

    --情報ツリーをもらってElementsデータ内を処理
    --func(index, data, tree); non nilを返すとそこで終了
    function treeIterator(t,func)
      if (Elements[ t.id ]) then
        if ( func( t.id, Elements[ t.id ], t) ) then return true; end;
      end
      for i,c in pairs(t.child) do
        if ( treeIterator(c,func) ) then return true; end;
      end --for
      return nil;
    end -- treeIterator
    
    --エントリポイント################################################################
    function main()
      local parentId = -1;
      -- {id=-1, child={ {id=0, child={...}},...} }な感じのツリー
      local infoTree = {id=-1,child={}};
      makeElementTree(infoTree, 0,Elements);
      
      for index,data in pairs(Elements) do
        local elementTypeStr = "";
        local directStr = "";
        if string.len(data.factor2) ~= 0 then
          directStr = string.lower(data.factor1);
          elementTypeStr =  string.lower(data.factor2);
        else
          elementTypeStr = string.lower(data.factor1);
        end -- data.factor2
--        outDebug("index->"..index..": "..data.elementNumber..data.factor1..":"..data.factor2);

        --rimを挟みこむ
        if elementTypeStr=="wheel" then
          local rimLevel = data.parentNumber;
          local n = addElement(data,"rim", directStr);
          --直下の回転体は
          data.elementNumber = data.elementNumber+1;
          data.parentNumber = n;
          data.param.angle = "0";
--          outDebug("start->"..index..": end=>"..table.getn(Elements));
          
          --以下のElementsデータを書き換える
          for j=index+1, table.getn(Elements) do
            local  e = Elements[ j ];
            e.elementNumber = e.elementNumber + 1;
--            outDebug("call rim. parent=>"..e.parentNumber);
            if e.parentNumber > rimLevel then
--              outDebug( e.factor1..":"..e.factor2.."  "..e.parentNumber.."->"..e.parentNumber+1);
              e.parentNumber = e.parentNumber + 1;
            end
          end
          directStr = "face";   -- wheelは強制的に表面接続
        end --if elementType=="wheel"

        --追加
        addElement(data,elementTypeStr, directStr);
        
      end -- for Elements

      return 1;
    end --/main

    
    --設定マップ################################################################
    shapeMap = {
      core  = getToMap(shapeType, "panel");
      chip  = getToMap(shapeType, "panel");
      rudder = getToMap(shapeType, "panel");
      trim = getToMap(shapeType, "panel");
      rim   = getToMap(shapeType, "shaft");
      wheel = getToMap(shapeType, "disc");
      jet = getToMap(shapeType, "panel");
    };

    --可動軸
    axisMap = {
      core = getToMap(axisType, "x");
      chip = getToMap(axisType, "x");
      rudder=getToMap(axisType, "y");
      trim = getToMap(axisType, "z");
      rim = getToMap(axisType, "x");
      wheel = getToMap(axisType, "z");
      jet = getToMap(axisType, "x");
    };

    --ジョイントタイプ
    jointMap = {
      core = getToMap(jointType, "common");
      chip = getToMap(jointType, "common");
      rudder= getToMap(jointType, "common");
      trim = getToMap(jointType, "common");
      rim = getToMap(jointType, "common");
      wheel = getToMap(jointType, "shaft");
      jet = getToMap(jointType, "common");
    };

    --重量
    massMap = {
      core = 25.0;
      chip = 25.0;
      rudder = 25.0;
      trim = 25.0;
      rim = 25.0;
      wheel = 25.0;
      jet = 25.0;
    };

    --空力定数1,
    airParame0Map ={
      core = 0.8;
      chip = 0.8;
      rudder = 0.8;
      trim = 0.8;
      rim = 0.01;
      wheel = 0.1;
      jet = 0.01;
    };
    --空力定数2
    airParame1Map = {
      core = 0;
      chip = 0;
      rudder = 0;
      trim = 0;
      rim = 0.2;
      wheel = 0;
      jet = 0.01;
    };
    --空力定数3
    airParame2Map = {
      core = 1;
      chip = 1;
      rudder = 1;
      trim = 1;
      rim = 0.2;
      wheel = 0.3;
      jet = 0.01;
    };

    --エレメント名とメッシュファイルの対応
    meshNameMap = {
      core = "chip";
      chip = "chip";
      rudder= "chip";
      trim = "chip";
      rim = "rim";
      wheel = "wheel";
      jet = "jet";
    };
    
        
    --メッシュファイルのデータ
    meshData = {};
    meshData["chip"] = {
      position = {0,0,0};
      rotation = {0,0,0};
      scale = {1, 1, 1};
    };
    meshData["wheel"] = {
      position = {0,0,0};
      rotation = {0,0,0};
      scale = {1, 1, 1};
    };
    meshData["rim"] = {
      position = {0,0,0};
      rotation = {0,0,0};
      scale = {0.4,0.4,0.4};
    };
    meshData["jet"] = {
      position = {0,0,0};
      rotation = {0,0,0};
      scale = {1,1,1};
    };
    --設定マップ################################################################

