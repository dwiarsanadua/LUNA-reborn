--[[
  Quest 812: [Party] Threat of Trolls
  Level Required: 71
  NPC Start: 117, NPC Complete: 117
  Rewards: EXP=1237080, Gold=23174
]]

local fsm = require('fsm_engine')

local quest_812 = fsm:new({
    id = 812,
    name = "[Party] Threat of Trolls",
    level_required = 71,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 812: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 812: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 812: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 104,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 812: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 812: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 812: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 812: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 812: Rewards given")
                self:give_rewards({
                    exp = 1237080,
                    gold = 23174,
                })
            end,
            transitions = {},
        },
    },
})

return quest_812