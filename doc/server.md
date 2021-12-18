战斗系统

战斗对象 

Battle对象：

battle关联battle里的actor
还有回合流程控制

进行回合控制，里面包含了actors， actor分为player，npc，summon
player， player_summon的指令由客户端上发
npc的指令战斗系统根据npc的战斗指令状态机自行生成

服务端战斗指令处理流程：

先根据actor的速度来计算出战斗指令的执行顺序

根据顺序遍历每个指令，计算指令产生的hp，mp变化以及buff状态变化，同时计算出客户端的表现效果

下发客户端表现需要的数据

客户端做表现

战斗中的特殊情况：

NPC可以被击飞，击飞后的npc被销毁，移出战斗系统

玩家逃跑，不是唯一玩家逃跑，逃跑玩家被移出战斗系统，逃跑玩家脱战，战斗系统继续

唯一玩家逃跑，战斗结束

玩家BB被击飞，移出战斗系统。
玩家BB被召唤，加入战斗系统。

玩家战斗执行发送，既包含自己的，也包含所携带BB的

处理战斗指令的时候，就不必关心战斗指令的来源了。

战斗指令处理的时候可能出现的情况：

actor被移出，移入战斗系统。actor被击飞 or 死亡

战斗指令 cmd
战斗指令表现 skill
战斗指令效果 actor属性计算

作用对象：  己方 对方
物理攻击
	剑荡四方 
法术攻击 龙卷雨击
	封印
		催眠符
	辅助
		加血
暗器
特技
道具

技能表现：

技能效果：

buff表现：

buff效果：

```mermaid

graph TD;
	BATTLE_DEFAULT--创建battle-->BATTLE_START;
	BATTLE_START-->BATTLE_TURN_STAND_BY;
	BATTLE_TURN_STAND_BY-->F{check_battle_ready};
	F--Y-->BTTALE_TURN_EXECUTE;
	F--N-->AA-->BTTALE_TURN_NEXT
	BTTALE_TURN_EXECUTE--战斗指令序列处理-->BTTALE_TURN_NEXT;
	BTTALE_TURN_NEXT--是否战斗结束-->BATTLE_END;

```

Battle

AddActor
RemoveActor
GetTeamActors
AddCommand
RemoveCommand
ChangeBattleState


Command
type 吃药 暗器 物品。。。
data
skill

Actor

GetBattle
GetTeam
GetTargetTeam
GetSummon
ChangeSummon
SetSummon
Type : NPC player

Skill


Buff



存放图片
字符串
asset.xml
style.xml
value.xml
color.xml
类似android

id	name
1	枪矛
2	斧钺
3	剑
4	双短剑
5	飘带
6	爪刺
7	扇
8	魔棒
9	锤
10	鞭
11	环圈
12	刀
13	法杖
14	宝珠
15	弓弩

git -c filter.lfs.smudge= -c filter.lfs.required=false clone url path

下一步计划

1. 动画系统， 动画队列 动画插值
2. 渲染系统 支持层级 更好的渲染效率
3. 击飞动画 简单的技能拼装
4. actor属性代码生成
5. 技能编辑器
6. 战斗框架
7. 服务器尝试位移帧同步
8. 打包 解包 支持PNG
9. UI系统
10. 程序模块化 UI插件化 热更


红色伤害数字 WDF:misc  0x30F737D8
绿色伤害数字 WDF:misc  0x3CF8F9FE

文件类型
Sp  .was
BIN
TXT
RIFF  声音.wav
OggS 声音  .map3

File Type: MPEG ADTS, layer III, v2, 96 kbps, 22.05 kHz, JntStereo
MIME Type: application/octet-stream
Suggested file extension(s): bin dms lha lzh exe class so dll img iso

 PayPal - The safer, easier way to pay online! 
