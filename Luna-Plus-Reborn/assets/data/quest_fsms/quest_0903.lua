--[[
  Quest 903: A Village in Ruins
  Level Required: 80
  NPC Start: 97, NPC Complete: 97
  Prerequisites: [902]
  Rewards: EXP=2353082
]]

local fsm = require('fsm_engine')

local quest_903 = fsm:new({
    id = 903,
    name = "A Village in Ruins",
    level_required = 80,
    npc_start = 97,
    npc_complete = 97,
    prerequisites = {902},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 903: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 903: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 903: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 74,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 903: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 903: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 903: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 903: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 903: Rewards given")
                self:give_rewards({
                    exp = 2353082,
                })
            end,
            transitions = {},
        },
    },
})

return quest_903