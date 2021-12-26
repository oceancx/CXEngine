## 枚举值

系统自定义枚举值（可以直接使用，不需要script_system_dofile('xxx')）

client\src\resource_manager.cpp:

ADDONWDF
ATOMWDF
CHATWDF
FIREWORKWDF
GOODSWDF
ITEMWDF
MAGICWDF
MAPANIWDF
MHIMAGEWDF
MISCWDF
MUSICWDF
SCENEWDF
SHAPEWDF
SHAPEWD1
SHAPEWD2
SHAPEWD3
SHAPEWD4
SHAPEWD5
SHAPEWD6
SHAPEWD7
SMAPWDF
SOUNDWDF
STOCKWDF
WADDONWDF
WZIFEWDF
WZIFEWD1


common\src\actor\actor_enum.cpp:

PROP_ID
PROP_DIR
PROP_RACE
PROP_SCHOOL
PROP_SCHOOL_SKILL_LV_TARGETHIT
PROP_SCHOOL_SKILL_LV_DAMAGE
PROP_SCHOOL_SKILL_LV_DEFEND
PROP_SCHOOL_SKILL_LV_SPEED
PROP_SCHOOL_SKILL_LV_DODGE
PROP_SCHOOL_SKILL_LV_SPIRITUAL
PROP_SCHOOL_SKILL_LV_HP
PROP_SCHOOL_SKILL_LV_MP
PROP_EQUIP_HP
PROP_EQUIP_MP
PROP_EQUIP_TARGET
PROP_EQUIP_DAMAGE
PROP_EQUIP_DEFEND
PROP_EQUIP_SPIRITUAL
PROP_EQUIP_AGILE
PROP_AUXI_SKILL_LV_HP
PROP_AUXI_SKILL_LV_MP
PROP_SHOW_BOUNDINGBOX
PROP_SHOW_AVATAR_INFO
PROP_IS_AUTORUN
PROP_IS_DEAD
PROP_FRAME_SPEED
PROP_ACTOR_TYPE
PROP_NAME
PROP_AVATAR_ID
PROP_WEAPON_AVATAR_ID
PROP_SCENE_ID
PROP_ROLE_ID
PROP_WEAPON_ID
PROP_WEAPON_TYPE
PROP_ACTION_ID
PROP_POS
PROP_MOVE_TO_POS
PROP_DIR_COUNT
PROP_IS_MOVE
PROP_MOVE_VELOCITY
PROP_CAN_MOVE
PROP_IS_COMBAT
PROP_TARGET_ID
PROP_CAST_ID
PROP_IS_CALC_MOVE_LIST
PROP_SAY_DURATION
PROP_IS_LOCAL
PROP_COMBAT_POS
PROP_COMBAT_DIR
PROP_COMBAT_BACKUP_POS
PROP_COMBAT_TARGET_POS
PROP_COMBAT_ACTING
PROP_COMBAT_BATTLE_ID
PROP_TEAM_TYPE
PROP_TEAM_ID
PROP_USING_SKILL
PROP_CAST_SKILL_TID
PROP_TURN_READY
PROP_ASM_ATK_COMBO
PROP_ASM_BEHIT_ANIM
PROP_ASM_DAMAGE
PROP_ASM_HEAL
PROP_ASM_BUFF_ANIM
PROP_ASM_PLAY_BEHIT
PROP_ASM_RUNTO_X
PROP_ASM_RUNTO_Y
PROP_HP
PROP_MP
PROP_SP
PROP_LV
PROP_ADD_PROP_PLAN
PROP_EXP
PROP_BASE_FORCE
PROP_BASE_HEALTH
PROP_BASE_STAMINA
PROP_BASE_AGILITY
PROP_BASE_MAGIC
PROP_SUMMON_TAKE_LEVEL
PROP_SUMMON_BATTLE_LEVEL
PROP_SUMMON_ATK_QUAL
PROP_SUMMON_DEF_QUAL
PROP_SUMMON_HEALTH_QUAL
PROP_SUMMON_MAGIC_QUAL
PROP_SUMMON_SPEED_QUAL
PROP_SUMMON_DODGE_QUAL
PROP_SUMMON_GROW_COEF
PROP_IS_USING_SKILL
PROP_COMBAT_SKILL_BAN_ATK
PROP_COMBAT_SKILL_BAN_SPELL
PROP_SUMMON_UIDS
PROP_SUMMON_HAS_OWNER
PROP_SUMMON_OWNER
PROP_COMBAT_POS_ID
PROP_DAMAGE_EXTRA
PROP_DEFEND_EXTRA
PROP_SPEED_EXTRA
PROP_SPIRITUAL_EXTRA
PROP_TARGETHIT_EXTRA
PROP_DODGE_EXTRA
PROP_ASSIGN_FORCE
PROP_ASSIGN_HEALTH
PROP_ASSIGN_STAMINA
PROP_ASSIGN_AGILITY
PROP_ASSIGN_MAGIC
PROP_ATK_PRACTICE_SKILL_LV
PROP_ATK_RESISTANCE_SKILL_LV
PROP_SPELL_PRACTICE_SKILL_LV
PROP_SPELL_RESISTANCE_SKILL_LV
PROP_PAL_MATRIX
PROP_IS_AUTO_COMMAND
PROP_SETTING_BGM
PROP_SETTING_WILD_BATTLE
PROP_COUNT


PROP_TYPE_BOOL
PROP_TYPE_FLOAT
PROP_TYPE_INT
PROP_TYPE_STR
PROP_TYPE_UINT64
PROP_TYPE_VEC2

common\src\actor\actor.cpp:

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

ACTOR_EV_ON_CLICK
ACTOR_EV_ON_HOVER
 
common\src\imgui\cximgui_enums.inl:
此处不列，见cximgui_enums.inl即可

scripts\client\actor_metatable.lua:

AnimationMT:DrawAvatarInfo()
AnimationMT:DrawBoundingBox()

ActorMT:DrawAvatarInfo()
ActorMT:DrawBoundingBox()
ActorMT:GetAvatarRect()

ActorMT:PlaySound(action)
ActorMT:GetSceneBGMName()
ActorMT:GetAtkKeyFrame()


scripts\client\module\summon.lua:

ActorMT:GetAtkKeyFrame()
ActorMT:GetAtkKeyFrame()
  
scripts\client\module\team.lua:
TeamMT:new(o)
TeamMT:GetID()
TeamMT:AddMember(actor)
TeamMT:RemoveMember(actor)
TeamMT:SetLeader(actor)
TeamMT:GetMembers()
TeamMT:GetLeader()
TeamMT:new(o)
TeamMT:GetID()
TeamMT:AddMember(actor)
TeamMT:RemoveMember(actor)
TeamMT:HasMember(actor)
TeamMT:SetLeader(actor)
TeamMT:GetMembers()
TeamMT:GetLeader()
TeamMT:Serialize()

ActorMT:HasTeam()
ActorMT:GetTeam()
ActorMT:IsTeamLeader()
ActorMT:CreateTeam()
ActorMT:DismissTeam()
ActorMT:AddTeamMember(actor)
ActorMT:OrganizeTeam(actor)
ActorMT:RemoveTeamMember(actor)
ActorMT:LeaveTeam()
 
scripts\combat\combat_system.lua:
BattleMT:new(o)
BattleMT:Serialize()
BattleMT:Deserialize(info)
BattleMT:FindActor(actor_id)
BattleMT:AddActor(actor, team_type, pos_i)
BattleMT:RemoveActor(actor)
BattleMT:GetNotReadyActors()
BattleMT:GetTurn()
BattleMT:AddCommand(actor, cmd)
BattleMT:RandomSelectEnemy(self_actor)
BattleMT:AutoCommand(actor)
BattleMT:PrepareBattle()
BattleMT:StartBattle()
BattleMT:CheckAllIdle()
BattleMT:InBattle(__actor__)
BattleMT:EndBattle()
BattleMT:NextTurn()
BattleMT:CheckStandBy()
BattleMT:CheckEnd()

scripts\combat\client\combat_system.lua:
ActorMT:GetBattle()

scripts\combat\server\combat_system.lua:
ActorMT:GetBattle()

scripts\server\actor_system.lua:
ActorMT:AddSummon(summon)
ActorMT:GetSummon()
ActorMT:GetSummons()
ActorMT:HasTeam()
ActorMT:GetTeam()
ActorMT:IsTeamLeader()
ActorMT:CreateTeam()
ActorMT:DismissTeam()

scripts\share\actor_metatable.lua:

ActorMT:GetPos()
ActorMT:SetPos(x,y)
ActorMT:SetCombatPos(x,y)
ActorMT:IsCombat()
ActorMT:GetY()
ActorMT:GetX()
ActorMT:StopMove()
ActorMT:FaceTo(target)
ActorMT:SetProperties(props)
ActorMT:ApplySummonQual(name)
ActorMT:GetProperties()
ActorMT:GetName()
ActorMT:CalcSchoolSkillTargethit()
ActorMT:CalcSchoolSkillDamage()
ActorMT:CalcSchoolSkillDefend()
ActorMT:CalcSchoolSkillSpeed()
ActorMT:CalcSchoolSkillDodge()
ActorMT:CalcSchoolSkillSpiritual()
ActorMT:CalcSchoolSkillHP(base_hp)
ActorMT:CalcSchoolSkillMP(base_mp)
ActorMT:GetMaxHP()
ActorMT:GetMaxMP()
ActorMT:GetHP()
ActorMT:GetMP()
ActorMT:ModifyHP(delta)
ActorMT:ModifyMP(delta)
ActorMT:CalcTargetHit()
ActorMT:CalcAttack()
ActorMT:CalcDefend()
ActorMT:CalcSpeed()
ActorMT:CalcSpiritual()
ActorMT:CalcDodge()
ActorMT:IsDead()
ActorMT:GetRaceName()
ActorMT:GetSceneName()
ActorMT:GetSchoolName()
ActorMT:GetInitProp()
ActorMT:GetRemainPropPoints()
ActorMT:SetPropsByPlan(plan)
ActorMT:GetInitHealthProp()
ActorMT:GetInitMagicProp()
ActorMT:GetInitForceProp()
ActorMT:GetInitStaminaProp()
ActorMT:GetInitAgilityProp()
ActorMT:GetHealthProp()
ActorMT:GetMagicProp()
ActorMT:GetForceProp()
ActorMT:GetStaminaProp()
ActorMT:GetAgilityProp()
ActorMT:ClearAssignPoints()
ActorMT:UpdatePropPtsByPlan()
ActorMT:GetAttackDamage(target, is_critical, is_combo, combo_coef, actor_type_coef)
ActorMT:GetSpellDamage(target)
ActorMT:SetGlobalStandardEquip(lv, is_enforce)
ActorMT:IsPlayer()
ActorMT:IsNPC()
ActorMT:IsSummon()
ActorMT:GetSummonOwner()
ActorMT:SetSummonOwner(owner)
ActorMT:RemoveSummonOwner()
ActorMT:GetSceneBGMName()
ActorMT:GetRandomSkillID(type)
ActorMT:Variation()


audio_manager_play
cxlog_info
input_manager_get_mouse_pos
net_manager_update
ui_renderer_add_to_draw
npc_dialog_show
vfs_list_files
action_calc_run_to_pos
actor_manager_set_local_player
astar_get_angle_use_box_xy
timer_manager_update
timer_manager_add_timer
utils_file_write
luadbg_listen
debugger_send_message
game_server_update
input_manager_init
input_manager_deinit
net_manager_init
net_manager_deinit
net_manager_reconnect
net_send_message_in_c)
resource_manager_init
resource_manager_update
resource_manager_deinit
sprite_renderer_init
text_renderer_init
text_renderer_draw_text
window_system_set_floating
window_system_get_dt
window_system_get_fps
game_get_width
game_get_height
iw_deinit
iw_should_close
iw_set_font
iw_init
animation_manager_update
animation_manager_draw
animation_manager_clear
audio_manager_stop
audio_manager_clear
audio_manager_toggle_bgm
audio_manager_is_bgm_on
ui_renderer_clear
npc_dialog_is_show
npc_dialog_set_xy
ui_renderer_update
ui_renderer_draw
debug_get_drawcall
debug_get_texture_count
debug_get_sprites_count
scene_manager_init
scene_manager_update
scene_manager_draw
scene_manager_deinit
scene_manager_add_scene
scene_manager_add_custom_scene
scene_is_combat
scene_set_map
game_map_reset_map_offset
scene_manager_switch_scene_by_id
scene_manager_switch_scene_by_name
scene_manager_get_current_scene_id
scene_manager_get_current_scene_name
scene_manager_sync_draw_cbx
fs_get_tsv_path
log_print
process_is_server
time_now
time_now_nano
action_get_name
actor_manager_update
actor_manager_draw
actor_manager_set_scene
astar_get_dir_use_int
astar_get_dir
astar_get_dir4
math_get_reverse_dir
math_clamp
math_dir_8_to_4
math_next_dir4
action_system_get_action_size
action_system_get_action_name
timer_manager_init
timer_manager_deinit
timer_manager_remove_timer
timer_manager_pause_timer
timer_manager_end_timer
iw_init
iw_deinit
iw_should_close
iw_begin_render
iw_end_render
iw_set_font
fn) lua_register_function(L, f
enum_reset
enum_next
command_arg_check
command_arg_get
command_arg_opt_str
command_arg_opt_int
io_service_context_init
at_exit_manager_init
utils_resave_tsv_file
luadbg_set_line_ending_in_c
luadbg_get_line_ending_in_c
luadbg_stop
debugger_sleep
debugger_fetch_message
debugger_is_connected
luadbg_get_line_ending_in_c
get_line_ending_in_c
set_line_ending_in_c
start_stdio_read_thread
get_line_ending_in_c
set_line_ending_in_c
sleepms
game_server_start
game_server_stop


luaL_Reg MT_BASE_SPRITE[] = {
	{ "Update",base_sprite_update },
{ "Draw",base_sprite_draw },
{ "SetPos", base_sprite_set_pos },
{ "GetPos", base_sprite_get_pos },
{ "SetDir",base_sprite_set_dir },
{ "GetDir",base_sprite_get_dir },
{ "SetFrameInterval",base_sprite_set_frame_interval },
{ "GetFrameInterval",base_sprite_get_frame_interval },
{ "GetWidth", base_sprite_get_width },
{ "GetHeight", base_sprite_get_height },
{ "SetWidth", base_sprite_set_width },
{ "SetHeight", base_sprite_set_height },
{ "GetKeyX", base_sprite_get_KeyX },
{ "GetKeyY", base_sprite_get_KeyY },
{ "GetFrameKeyX", base_sprite_get_frame_KeyX },
{ "GetFrameKeyY", base_sprite_get_frame_KeyY },
{ "GetFrameHeight", base_sprite_get_frame_height },
{ "GetFrameWidth", base_sprite_get_frame_width },
{ "GetPlayTime", base_sprite_get_play_time },
{ "GetDirCnt", base_sprite_get_dir_cnt },
{ "GetTotalFrames", base_sprite_get_total_frames },
{ "GetCurrentFrame", base_sprite_get_current_frame },
{ "SetCurrentFrame", base_sprite_set_current_frame },
{ "GetGroupFrameCount", base_sprite_get_group_frame_count },
{ "GetGroupFrameTime", base_sprite_get_group_frame_time},
{ "GetGroupCount", base_sprite_get_group_count },
{ "EnableDrag", base_sprite_enable_drag },
{ "Export", base_sprite_export },
{ "ExportWas", base_sprite_export_was },
{ "Destroy", base_sprite_destroy },
{ NULL, NULL }
};


luaL_Reg MT_ANIMATION[] = {
{ "Pause",animation_pause },
{ "Stop",animation_stop},
{ "Play",animation_start },
{ "LockFrame",animation_lock_frame },
{ "UnLockFrame",animation_unlock_frame },
{ "SetLoop",animation_set_loop },
{ "GetVisible", animation_get_visible },
{ "SetVisible", animation_set_visible },
{ "Translate", animation_translate },
{ "AddCallback", animation_add_callback},
{ "AddStopCallback", animation_add_stop_callback},
{ "AddFrameCallback", animation_add_frame_callback},
{ "AddStartCallback", animation_add_start_callback},
{ "AddLoopCallback", animation_add_loop_callback},
{ "AddUpdateCallback", animation_add_update_callback},
{ "RemoveUpdateCallback", animation_remove_update_callback},
{ "RemoveLoopCallback", animation_remove_loop_callback},
{ "GetKeyFrame", animation_get_key_frame},
{ "Reset", animation_reset},
{ "SetOffsetX", animation_set_offset_x},
{ "SetOffsetY", animation_set_offset_y},
{ "IsGroupEndUpdate", animation_is_group_end_update},
{ "IsFrameUpdate", animation_is_frame_update},
{ "SetRotation", animation_set_rotation},
//{ "__gc", animation_destroy},
{ "__index", animation_index},
{ "__newindex", animation_newindex},
{ NULL, NULL }
};

luaL_Reg MT_BEAT_NUMBER[] = {
	{ "Update",beat_number_update },
	{ "Draw",beat_number_draw },
	{ "Beat",beat_number_beat },
	{ "SetPos",beat_number_set_pos},
	{ "SetNumber",beat_number_set_number },
};

luaL_Reg mt_actor[] = {
	{ "Destroy",actor_destroy },
{ "Update",actor_update },
{ "Draw",actor_draw },
{ "GetWidth", actor_get_width },
{ "GetHeight", actor_get_height },
{ "GetID", actor_get_id },
{ "SetDir", actor_set_dir },
{ "GetDir", actor_get_dir },
{ "ReverseDir", actor_reverse_dir },
{ "SetLocal", actor_set_local },
{ "IsLocal", actor_is_local },
{ "SetActionID", actor_set_action_id },
{ "GetActionID", actor_get_action_id },
{ "TranslateX", actor_translate_x },
{ "TranslateY", actor_translate_y },
{ "MoveTo", actor_move_to },
{ "IsMoving", actor_is_moving},
{ "Say", actor_say },
{ "ShowDialog", actor_show_dialog},
{ "ClearFrames", actor_clear_frames },
{ "SetTarget", actor_set_target},
{ "GetTarget", actor_get_target},
{ "PlayCast", actor_play_cast },
{ "SetTimeInterval", actor_set_time_interval },
{ "GetAvatar", actor_get_avatar },
{ "GetWeapon", actor_get_weapon },
{ "ChangePalMatrix", actor_change_pal_matrix},
{ "GetPalMatrix", actor_get_pal_matrix},
{ "ClearPalMatrix", actor_clear_pal_matrix},
{ "RegProperty", actor_reg_prop},
{ "GetProperty", actor_get_prop},
{ "SetProperty", actor_set_prop},
{ "ResetASM", actor_reset_asm},
{"RestoreAction", actor_restore_action},
{"ASMSetAction", actor_asm_set_action},
{"PlayBeHit", actor_play_be_hit},
{"MoveOnScreenWithDuration",actor_move_on_screen_with_duration},
{"GetMoveDestAngle",actor_get_move_dest_angle},
{"GetDirByDegree",actor_get_dir_by_degree},
{"PushAction",actor_push_action},
{"ClearAction",actor_clear_action},
{"MoveActionToBack",actor_move_action_to_back},
{"AddFrontAnim",actor_add_front_anim},
{"RemoveFrontAnim",actor_remove_front_anim},
{"AddBackAnim",actor_add_back_anim},
{"RemoveBackAnim",actor_remove_back_anim},
{ "ClearBackAnim", actor_clear_back_anim},
{ "ClearFrontAnim", actor_clear_front_anim},
{"ShowBeatNumber",actor_show_beatnumber},
{"GetAttackVec",actor_get_attack_vec},
{ "IsDirty",actor_is_dirty},
{ "ClearDirty",actor_clear_dirty},
{ "GetDirtyProps",actor_get_dirty_props},
{ NULL, NULL }
};


luaL_Reg mt_tcp_connection_reg[] = {
	{ "tohostport",tcp_connection_to_host_port},
	{ "connected",tcp_connection_connected},
	{ "send",tcp_connection_send },
	{ "Send",tcp_connection_Send },
	{ NULL, NULL }
};

luaL_Reg mt_buffer_reg[] = {
	{ "readable_size",buffer_readablesize },
	{ "Preview",buffer_preview },
	{ "Consume",buffer_consume },
	{ "PrependInt",buffer_prependint},
	{ "PrependString",buffer_prependstring },
	{ "WriteString",buffer_writestring },
	{ "WriteFloat",buffer_writefloat },
	{ "WriteInt",buffer_writeint },
	{ "WriteByte",buffer_writebyte },
	{ "WriteInt64",buffer_writeint64 },
	{ "ReadAsString",buffer_readstring },
	{ "ReadAllAsString",buffer_readallstring },
	
	{ "ReadAsFloat",buffer_readfloat },
	{ "ReadAsInt",buffer_readint },
	{ "PeekAsInt",buffer_peekint},
	{ "PeekAsFloat",buffer_peekfloat },
	{ "PeekAsString",buffer_peekstring },

	//{ "__gc",buffer_gc },
	{ "Destroy",buffer_gc },
	{ NULL, NULL }
};


luaL_Reg MT_EZ_EVENT_LOOP[] = {
	{ "Run",ez_event_loop_run },
	{ "Quit",ez_event_loop_quit },
	{ "QueueTask",ez_event_loop_queue_task },
	{ "RunTask",ez_event_loop_run_task },
	//{ "RunTaskAt",ez_event_loop_run_task_at },
	{ "RunTaskAfter",ez_event_loop_run_task_after },
	{ "RunTaskEvery",ez_event_loop_run_task_every },
	{ "CancelTimedTask",ez_event_loop_run_task_cancel_timed_task },
	{ "BelongsToCurrentThread",ez_event_loop_belongs_to_current_thread },
	{ "RegisterNotifier",ez_event_loop_register_notifier },
	{ "UnregisterNotifier",ez_event_loop_unregister_notifier },
	{ "Wakeup",ez_event_loop_wakeup },
	{ NULL, NULL}
};



static luaL_Reg MT_EZ_TCP_SERVER[] = {
	{ "Start",ez_tcp_server_start },
	{ "ip_port",ez_tcp_server_ip_port },
	{ "name",ez_tcp_server_name},
	{ "set_on_connection",ez_tcp_server_set_on_connection},
	{ "set_on_message",ez_tcp_server_set_on_message},
	{ NULL, NULL}
};



static luaL_Reg MT_EZ_TCP_CLIENT[] = {
	{ "IsConnected",ez_tcp_client_is_connected},
	{ "Connect",ez_tcp_client_connect},
	{ "Cancel",ez_tcp_client_cancel},
	{ "Disconnect",ez_tcp_client_disconnect},
	{ "set_auto_reconnect",ez_tcp_client_set_auto_reconnect},
	{ "Cancel",ez_tcp_client_cancel},
	{ "name",ez_tcp_client_name},
	{ "connection",ez_tcp_client_connection},
	{ "set_on_connection",ez_tcp_client_set_on_connection},
	{ "set_on_message",ez_tcp_client_set_on_message},
	{ NULL, NULL}
};



luaL_Reg MT_NET_THREAD_QUEUE[] = {
	{ "push_back",net_thread_queue_push_back },
	{ "pop_front",net_thread_queue_pop_front },
	{ "front_as_string",net_thread_queue_front_as_string},
	{ "front",net_thread_queue_front },
	{ "size",net_thread_queue_size },
	{ "empty",net_thread_queue_empty },
	{ "clear",net_thread_queue_clear },
	{ NULL, NULL }
};

