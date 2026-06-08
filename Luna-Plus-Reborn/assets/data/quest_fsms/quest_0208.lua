--[[
  Quest 208: [Revenge] Getting Even
  Level Required: 17
  NPC Start: 0, NPC Complete: 0
  Rewards: EXP=5000, Gold=1300
]]

local fsm = require('fsm_engine')

local quest_208 = fsm:new({
    id = 208,
    name = "[Revenge] Getting Even",
    level_required = 17,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 208: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "accept",
                    target = "IN_PROGRESS",
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 208: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 48,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 208: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 208: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 208: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 208: Rewards given")
                self:give_rewards({
                    exp = 5000,
                    gold = 1300,
                    items = {
                        { item_id = 21000008, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_208