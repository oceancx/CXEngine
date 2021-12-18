function npc_on_show_dialog(player, target)
	npc_dialog_show(true,'神州上下祸劫频生，灵石是否重补苍天裂痕，', {
			{ 
				txt = 'PVP战斗',
				func = function()
					local msg = {}
					msg.atk = player:GetID()
					msg.def = target:GetID()
					net_send_message(PTO_C2S_COMBAT_START, cjson.encode(msg))

					local player = actor_manager_fetch_local_player()
					player:StopMove()
				end
			},
			{ 
				txt ='组队',
				func=function()
					player:OrganizeTeam(target)
				end
			},
			{ 
				txt='踢出队伍',
				func=function()
					player:RemoveTeamMember(target)
				end
			},
			{ 
				txt='PVE战斗',
				func=function()
					local req = {}
					req.pid = player:GetID()
					net_send_message(PTO_C2S_COMBAT_PVE_START, cjson.encode(req))
				end
			},
			{
				txt='取消',
				func=function()
					
				end
			}
		})
end