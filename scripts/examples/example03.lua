--[[
CXEngine.exe --cwd=[工作目录] --script_path=[lua文件夹相对工作目录]/  --luapath=[相对script_path下的lua文件路径]
示例：
./bin/Debug/CXEngine.exe --cwd=[CXEngine目录的路径]/  --script_path=scripts/examples/ --luapath=example0.lua

example3 创建一个actor

涉及到Actor的枚举值：
ACTOR_TYPE_DEFAULT
ACTOR_TYPE_PLAYER
ACTOR_TYPE_SUMMON
ACTOR_TYPE_NPC

ACTION_IDLE
ACTION_WALK
ACTION_SIT
ACTION_ANGRY
ACTION_SAYHI
ACTION_DANCE
ACTION_SALUTE
ACTION_CLPS
ACTION_CRY
ACTION_BATIDLE
ACTION_ATTACK
ACTION_CAST
ACTION_BEHIT
ACTION_RUNTO
ACTION_RUNBACK
ACTION_DEFEND

ASM_ACTION
ASM_ATTACK_ACTION
ASM_CAST_ACTION
ASM_BEHIT_ACTION
ASM_BECAST_ACTION
ASM_DEAD_FLY_ACTION
ASM_PATH_MOVE_ACTION
		
WEAPON_SPEAR
WEAPON_AXE
WEAPON_SWORD
WEAPON_DBSWORDS
WEAPON_RIBBON
WEAPON_CLAW
WEAPON_FAN
WEAPON_WAND
WEAPON_HAMMER
WEAPON_WHIP
WEAPON_RING
WEAPON_KNIFE
WEAPON_STAFF
WEAPON_JEWEL
WEAPON_BOW

ROLE_FYN
ROLE_YNX
ROLE_XYS
ROLE_JXK
ROLE_HMR
ROLE_GJL
ROLE_JMW
ROLE_HTG
ROLE_WTJ
ROLE_XCE
ROLE_STB
ROLE_LTZ

DIR_N
DIR_S
DIR_W
DIR_E
DIR_NE
DIR_NW
DIR_SE
DIR_SW

RACE_HUMAN  人族
RACE_DEVIL  魔族
RACE_SPIRIT 仙族


SCHOOL_DT = 1
SCHOOL_FC = 2
SCHOOL_HS = 3
SCHOOL_NE = 4
SCHOOL_TG = 5
SCHOOL_LG = 6
SCHOOL_WZ = 7
SCHOOL_PT = 8
SCHOOL_DF = 9
SCHOOL_MW = 10
SCHOOL_ST = 11
SCHOOL_PS = 12
SCHOOL_LB = 13
SCHOOL_WD = 14
SCHOOL_SM = 15


创建一个actor比较复杂，会引入scripts/share里面的一些lua文件
]]

--script_system_dofile 相当于lua里面的dofile，只不过会把传入的是lua文件相对于项目目录的相对路径拼接成磁盘上的绝对路径
script_system_dofile('../share/enums.lua')
script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')
script_system_dofile('../share/content_system.lua')
script_system_dofile('../share/actor_metatable.lua')


local actor
function init()
    content_system_init()           --这个函数在content_system.lua里面定义，会加载各种数据表(tables/*.tsv)，游戏内容基本上是由表来定义的
    actor = lua_create_actor(1)     --lua_create_actor的参数是actor的id，这里就随便填了个1
    actor:ResetASM()                --ResetASM是重新设置actor的动作状态机，目前需要这么手动调用一下

    --actor这个对象很有用，每个actor对象都定义了很多属性，可以通过Get/SetProperty来读写，具体actor的属性定义文件在res\tables\actor_template.tsv中
    actor:SetProperty(PROP_NAME, 'yzxy')    --通过SetProperty来设置玩家的名字
    actor:SetProperty(PROP_ID , 1 )                             --设置ID
    actor:SetProperty(PROP_DIR , DIR_S )                        --设置朝向 
    actor:SetProperty(PROP_RACE , RACE_HUMAN)                   --设置种族 
    actor:SetProperty(PROP_SCHOOL , SCHOOL_DT )                 --设置门派
    actor:SetProperty(PROP_SCHOOL_SKILL_LV_TARGETHIT ,0 )       --设置增加命中的技能等级
    actor:SetProperty(PROP_SCHOOL_SKILL_LV_DAMAGE , 0)          --设置增加伤害的技能等级
    actor:SetProperty(PROP_SCHOOL_SKILL_LV_DEFEND , 0)          --设置增加防御的技能等级
    actor:SetProperty(PROP_SCHOOL_SKILL_LV_SPEED ,0 )           --设置增加速度的技能等级
    actor:SetProperty(PROP_SCHOOL_SKILL_LV_DODGE ,0 )           --设置增加闪避的技能等级
    actor:SetProperty(PROP_SCHOOL_SKILL_LV_SPIRITUAL , 0)       --设置增加灵力的技能等级
    actor:SetProperty(PROP_SCHOOL_SKILL_LV_HP , 0)              --设置增加气血的技能等级
    actor:SetProperty(PROP_SCHOOL_SKILL_LV_MP , 0)              --设置增加魔法的技能等级
    actor:SetProperty(PROP_EQUIP_HP , 0)                        --设置装备增加的魔法值
    actor:SetProperty(PROP_EQUIP_MP , 0)                        --设置装备增加的气血值
    actor:SetProperty(PROP_EQUIP_TARGET , 0)                    --设置装备增加的命中值
    actor:SetProperty(PROP_EQUIP_DAMAGE , 0)                    --设置装备增加的伤害值
    actor:SetProperty(PROP_EQUIP_DEFEND , 0)                    --设置装备增加的防御值
    actor:SetProperty(PROP_EQUIP_SPIRITUAL , 0)                 --设置装备增加的灵力值
    actor:SetProperty(PROP_EQUIP_AGILE , 0)                     --设置装备增加的速度值
    actor:SetProperty(PROP_AUXI_SKILL_LV_HP ,0 )                --设置增加血量的辅助技能等级
    actor:SetProperty(PROP_AUXI_SKILL_LV_MP ,0 )                --设置增加魔法值的辅助技能等级
    -- actor:SetProperty(PROP_SHOW_BOUNDINGBOX , )              --TODO:继续往下注释
    -- actor:SetProperty(PROP_SHOW_AVATAR_INFO , )
    -- actor:SetProperty(PROP_IS_AUTORUN , )
    -- actor:SetProperty(PROP_IS_DEAD , )
    -- actor:SetProperty(PROP_FRAME_SPEED , )
    -- actor:SetProperty(PROP_ACTOR_TYPE , )
    -- actor:SetProperty(PROP_NAME , )
    -- actor:SetProperty(PROP_AVATAR_ID , )
    -- actor:SetProperty(PROP_WEAPON_AVATAR_ID , )
    -- actor:SetProperty(PROP_SCENE_ID , )
    -- actor:SetProperty(PROP_ROLE_ID , )
    -- actor:SetProperty(PROP_WEAPON_ID , )
    -- actor:SetProperty(PROP_WEAPON_TYPE , )
    -- actor:SetProperty(PROP_ACTION_ID , )
    -- actor:SetProperty(PROP_POS , )
    -- actor:SetProperty(PROP_MOVE_TO_POS , )
    -- actor:SetProperty(PROP_DIR_COUNT , )
    -- actor:SetProperty(PROP_IS_MOVE , )
    -- actor:SetProperty(PROP_MOVE_VELOCITY , )
    -- actor:SetProperty(PROP_CAN_MOVE , )
    -- actor:SetProperty(PROP_IS_COMBAT , )
    -- actor:SetProperty(PROP_TARGET_ID , )
    -- actor:SetProperty(PROP_CAST_ID , )
    -- actor:SetProperty(PROP_IS_CALC_MOVE_LIST , )
    -- actor:SetProperty(PROP_SAY_DURATION , )
    -- actor:SetProperty(PROP_IS_LOCAL , )
    -- actor:SetProperty(PROP_COMBAT_POS , )
    -- actor:SetProperty(PROP_COMBAT_DIR , )
    -- actor:SetProperty(PROP_COMBAT_BACKUP_POS , )
    -- actor:SetProperty(PROP_COMBAT_TARGET_POS , )
    -- actor:SetProperty(PROP_COMBAT_ACTING , )
    -- actor:SetProperty(PROP_COMBAT_BATTLE_ID , )
    -- actor:SetProperty(PROP_TEAM_TYPE , )
    -- actor:SetProperty(PROP_TEAM_ID , )
    -- actor:SetProperty(PROP_USING_SKILL , )
    -- actor:SetProperty(PROP_CAST_SKILL_TID , )
    -- actor:SetProperty(PROP_TURN_READY , )
    -- actor:SetProperty(PROP_ASM_ATK_COMBO , )
    -- actor:SetProperty(PROP_ASM_BEHIT_ANIM , )
    -- actor:SetProperty(PROP_ASM_DAMAGE , )
    -- actor:SetProperty(PROP_ASM_HEAL , )
    -- actor:SetProperty(PROP_ASM_BUFF_ANIM , )
    -- actor:SetProperty(PROP_ASM_PLAY_BEHIT , )
    -- actor:SetProperty(PROP_ASM_RUNTO_X , )
    -- actor:SetProperty(PROP_ASM_RUNTO_Y , )
    -- actor:SetProperty(PROP_HP , )
    -- actor:SetProperty(PROP_MP , )
    -- actor:SetProperty(PROP_SP , )
    -- actor:SetProperty(PROP_LV , )
    -- actor:SetProperty(PROP_ADD_PROP_PLAN , )
    -- actor:SetProperty(PROP_EXP , )
    -- actor:SetProperty(PROP_BASE_FORCE , )
    -- actor:SetProperty(PROP_BASE_HEALTH , )
    -- actor:SetProperty(PROP_BASE_STAMINA , )
    -- actor:SetProperty(PROP_BASE_AGILITY , )
    -- actor:SetProperty(PROP_BASE_MAGIC , )
    -- actor:SetProperty(PROP_SUMMON_TAKE_LEVEL , )
    -- actor:SetProperty(PROP_SUMMON_BATTLE_LEVEL , )
    -- actor:SetProperty(PROP_SUMMON_ATK_QUAL , )
    -- actor:SetProperty(PROP_SUMMON_DEF_QUAL , )
    -- actor:SetProperty(PROP_SUMMON_HEALTH_QUAL , )
    -- actor:SetProperty(PROP_SUMMON_MAGIC_QUAL , )
    -- actor:SetProperty(PROP_SUMMON_SPEED_QUAL , )
    -- actor:SetProperty(PROP_SUMMON_DODGE_QUAL , )
    -- actor:SetProperty(PROP_SUMMON_GROW_COEF , )
    -- actor:SetProperty(PROP_IS_USING_SKILL , )
    -- actor:SetProperty(PROP_COMBAT_SKILL_BAN_ATK , )
    -- actor:SetProperty(PROP_COMBAT_SKILL_BAN_SPELL , )
    -- actor:SetProperty(PROP_SUMMON_UIDS , )
    -- actor:SetProperty(PROP_SUMMON_HAS_OWNER , )
    -- actor:SetProperty(PROP_SUMMON_OWNER , )
    -- actor:SetProperty(PROP_COMBAT_POS_ID , )
    -- actor:SetProperty(PROP_DAMAGE_EXTRA , )
    -- actor:SetProperty(PROP_DEFEND_EXTRA , )
    -- actor:SetProperty(PROP_SPEED_EXTRA , )
    -- actor:SetProperty(PROP_SPIRITUAL_EXTRA , )
    -- actor:SetProperty(PROP_TARGETHIT_EXTRA , )
    -- actor:SetProperty(PROP_DODGE_EXTRA , )
    -- actor:SetProperty(PROP_ASSIGN_FORCE , )
    -- actor:SetProperty(PROP_ASSIGN_HEALTH , )
    -- actor:SetProperty(PROP_ASSIGN_STAMINA , )
    -- actor:SetProperty(PROP_ASSIGN_AGILITY , )
    -- actor:SetProperty(PROP_ASSIGN_MAGIC , )
    -- actor:SetProperty(PROP_ATK_PRACTICE_SKILL_LV , )
    -- actor:SetProperty(PROP_ATK_RESISTANCE_SKILL_LV , )
    -- actor:SetProperty(PROP_SPELL_PRACTICE_SKILL_LV , )
    -- actor:SetProperty(PROP_SPELL_RESISTANCE_SKILL_LV , )
    -- actor:SetProperty(PROP_PAL_MATRIX , )
    -- actor:SetProperty(PROP_IS_AUTO_COMMAND , )
    -- actor:SetProperty(PROP_SETTING_BGM , )
    -- actor:SetProperty(PROP_SETTING_WILD_BATTLE , )
    
    actor:SetPos(100, 100)                  --设置玩家的世界坐标
end

function update()
    if actor then
        actor:Update()
        actor:Draw()
    end
end

do
    iw_init(800, 600)
    local ok, ret = pcall(init)
    if not ok then
        print(ret)
    end
    iw_render(update)
    iw_deinit()
end
