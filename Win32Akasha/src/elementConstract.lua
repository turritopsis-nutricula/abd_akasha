--[[
�ϐ����e�[�u��
VarNames = {varName1, varName2, ...};

�G�������g�e�[�u��
Elements = {
  id = {
    string factor1=�v�f1,
    string factor2=�v�f2,
    number elementNumber = id,
    number parentNumber = id,
    param = {
      string [attributeName] = value
    }},...}

    �N���C�A���gC�I�u�W�F�N�g�|�C���^
    userdata  Client;


    C_addElement(
      userdata Client,
      {
        number direct = �ڑ�����,
        number elementType = �G�������g���,
        number parentID = �e��ID,
        number context = �G�������g�R���e�L�X�g,
        number shape = �G�������g�`��,
        number jointType = �W���C���g�^�C�v,
        number jointAxis = �W���C���g��,
        number mass = ����,
        number edurance = �`�b�v�ϋv�x,
        number enager = �`�b�v�G�l���M�[,
        number airParameter0 = ��͒萔1,
        number airParameter1 = ��͒萔2,
        number airParameter2 = ��͒萔3,
        table mesh = {
          string name; ���b�V���t�@�C����,
          table position ={number X,number Y,number Z};
          table rotation = {number X,number Y,number Z};
          table scale = {number X,number Y,number Z};
        };
        table param = �p�����[�^�e�[�u��
        table sign = �p�����[�^�����e�[�u��(boolean�e�[�u��)
        [, table �����^���p�����[�^]
      }
    )

    number�ŌŒ�l�A������Ń��f���ϐ��l�Ƃ��Ĉ���
    �p�����[�^�e�[�u�� = {
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

    --�{�̃R�[�h�񋓑̂Ƃ̓����}�b�v�Q################################################################
    --�ڑ��������̃}�b�v
    --ElementDirect
    directType = {
      "s",                      -- South
      "e",                      -- East
      "n",                      -- North
      "w",                      -- West
      "face",                   -- ForeFace
      "back",                   -- BackFace
    };
    --�ڑ������}�b�v
    --ElementJointAxis
    axisType = {
      "x",
      "y",
      "z",
    };
    --�G�������g�`��
    --ElementCollisionShape
    shapeType = {
      "panel",
      "disc",
      "shaft"
    };

    --�G�������g�^�C�v
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

    --�W���C���g�^�C�v
    --ElementJointType
    jointType = {
      "common",
      "shaft"
    };

        --�񋓌^�Ή��}�b�v����̒l�擾
    function getToMap(mapTable, str)
      for i=1, table.getn(mapTable) do
        if (mapTable[i]==str) then
          return i-1;
        end --if
      end --for
    end --getToMap()
    
    --\ �{�̃R�[�h�񋓑̂Ƃ̓����}�b�v�Q################################################################


    -- �G�������g�ǉ��t�@���N################
    function addElement(data,elementTypeStr,directStr)
      
      local meshTable = {};
      do --meshTable�\��
        local f = meshData[ meshNameMap[ elementTypeStr ] ];
        meshTable.name = meshNameMap[ elementTypeStr ]..".x";
        for i, name in pairs{"position", "rotation", "scale"} do
          meshTable[ name ] = {
            X = f[ name ][1],
            Y = f[ name ][2],
            Z = f[ name ][3]
          };
        end --for
      end --/meshTable�\��

      local paramTable = {};
      local paramSign = {};
      do --paramTable�̍\��
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
      end --/paramTable�̍\��

      --outDebug( "addElement:"..elementTypeStr..":parent=>"..data.parentNumber);

      --C���̓o�^�֐��Ăяo��
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

    --���p�c���[�\���ċA�֐�
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

    --���c���[���������Elements�f�[�^��������
    --func(index, data, tree); non nil��Ԃ��Ƃ����ŏI��
    function treeIterator(t,func)
      if (Elements[ t.id ]) then
        if ( func( t.id, Elements[ t.id ], t) ) then return true; end;
      end
      for i,c in pairs(t.child) do
        if ( treeIterator(c,func) ) then return true; end;
      end --for
      return nil;
    end -- treeIterator
    
    --�G���g���|�C���g################################################################
    function main()
      local parentId = -1;
      -- {id=-1, child={ {id=0, child={...}},...} }�Ȋ����̃c���[
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

        --rim�����݂���
        if elementTypeStr=="wheel" then
          local rimLevel = data.parentNumber;
          local n = addElement(data,"rim", directStr);
          --�����̉�]�̂�
          data.elementNumber = data.elementNumber+1;
          data.parentNumber = n;
          data.param.angle = "0";
--          outDebug("start->"..index..": end=>"..table.getn(Elements));
          
          --�ȉ���Elements�f�[�^������������
          for j=index+1, table.getn(Elements) do
            local  e = Elements[ j ];
            e.elementNumber = e.elementNumber + 1;
--            outDebug("call rim. parent=>"..e.parentNumber);
            if e.parentNumber > rimLevel then
--              outDebug( e.factor1..":"..e.factor2.."  "..e.parentNumber.."->"..e.parentNumber+1);
              e.parentNumber = e.parentNumber + 1;
            end
          end
          directStr = "face";   -- wheel�͋����I�ɕ\�ʐڑ�
        end --if elementType=="wheel"

        --�ǉ�
        addElement(data,elementTypeStr, directStr);
        
      end -- for Elements

      return 1;
    end --/main

    
    --�ݒ�}�b�v################################################################
    shapeMap = {
      core  = getToMap(shapeType, "panel");
      chip  = getToMap(shapeType, "panel");
      rudder = getToMap(shapeType, "panel");
      trim = getToMap(shapeType, "panel");
      rim   = getToMap(shapeType, "shaft");
      wheel = getToMap(shapeType, "disc");
      jet = getToMap(shapeType, "panel");
    };

    --����
    axisMap = {
      core = getToMap(axisType, "x");
      chip = getToMap(axisType, "x");
      rudder=getToMap(axisType, "y");
      trim = getToMap(axisType, "z");
      rim = getToMap(axisType, "x");
      wheel = getToMap(axisType, "z");
      jet = getToMap(axisType, "x");
    };

    --�W���C���g�^�C�v
    jointMap = {
      core = getToMap(jointType, "common");
      chip = getToMap(jointType, "common");
      rudder= getToMap(jointType, "common");
      trim = getToMap(jointType, "common");
      rim = getToMap(jointType, "common");
      wheel = getToMap(jointType, "shaft");
      jet = getToMap(jointType, "common");
    };

    --�d��
    massMap = {
      core = 25.0;
      chip = 25.0;
      rudder = 25.0;
      trim = 25.0;
      rim = 25.0;
      wheel = 25.0;
      jet = 25.0;
    };

    --��͒萔1,
    airParame0Map ={
      core = 0.8;
      chip = 0.8;
      rudder = 0.8;
      trim = 0.8;
      rim = 0.01;
      wheel = 0.1;
      jet = 0.01;
    };
    --��͒萔2
    airParame1Map = {
      core = 0;
      chip = 0;
      rudder = 0;
      trim = 0;
      rim = 0.2;
      wheel = 0;
      jet = 0.01;
    };
    --��͒萔3
    airParame2Map = {
      core = 1;
      chip = 1;
      rudder = 1;
      trim = 1;
      rim = 0.2;
      wheel = 0.3;
      jet = 0.01;
    };

    --�G�������g���ƃ��b�V���t�@�C���̑Ή�
    meshNameMap = {
      core = "chip";
      chip = "chip";
      rudder= "chip";
      trim = "chip";
      rim = "rim";
      wheel = "wheel";
      jet = "jet";
    };
    
        
    --���b�V���t�@�C���̃f�[�^
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
    --�ݒ�}�b�v################################################################

