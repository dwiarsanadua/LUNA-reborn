--[[
  Quest 808: [Party] Tobe Drake Subjugation
  Level Required: 70
  NPC Start: 39, NPC Complete: 39
  Rewards: EXP=1198278, Gold=23174
]]

local fsm = require('fsm_engine')

local quest_808 = fsm:new({
    id = 808,
    name = "[Party] Tobe Drake Subjugation",
    level_required = 70,
    npc_start = 39,
    npc_complete = 39,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 808: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 808: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 808: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 152,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 808: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 808: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 808: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 808: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 808: Rewards given")
                self:give_rewards({
                    exp = 1198278,
                    gold = 23174,
                })
            end,
            transitions = {},
        },
    },
})

return quest_808