--[[
  Quest 33: Collecting Orc Weapons
  Level Required: 39
  NPC Start: 35, NPC Complete: 35
  Rewards: EXP=112861, Gold=5280
]]

local fsm = require('fsm_engine')

local quest_33 = fsm:new({
    id = 33,
    name = "Collecting Orc Weapons",
    level_required = 39,
    npc_start = 35,
    npc_complete = 35,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 33: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 35,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 33: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 33: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 43,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 33: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 35,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 33: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 33: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 35,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 33: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 33: Rewards given")
                self:give_rewards({
                    exp = 112861,
                    gold = 5280,
                    items = {
                        { item_id = 12, count = 5 },
                        { item_id = 30000017, count = 2 },
                        { item_id = 21000095, count = 7 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_33