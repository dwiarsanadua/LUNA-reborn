--[[
  Quest 23: Brutal Botany
  Level Required: 25
  NPC Start: 24, NPC Complete: 24
  Rewards: EXP=19326, Gold=2670
]]

local fsm = require('fsm_engine')

local quest_23 = fsm:new({
    id = 23,
    name = "Brutal Botany",
    level_required = 25,
    npc_start = 24,
    npc_complete = 24,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 23: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 23: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 23: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 24,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 23: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 23: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 23: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 23: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 23: Rewards given")
                self:give_rewards({
                    exp = 19326,
                    gold = 2670,
                    items = {
                        { item_id = 8, count = 1 },
                        { item_id = 21000008, count = 5 },
                        { item_id = 30000824, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_23