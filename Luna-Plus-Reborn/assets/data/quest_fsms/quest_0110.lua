--[[
  Quest 110: Food Delivering
  Level Required: 20
  NPC Start: 59, NPC Complete: 60
  Rewards: EXP=3733, Gold=750
]]

local fsm = require('fsm_engine')

local quest_110 = fsm:new({
    id = 110,
    name = "Food Delivering",
    level_required = 20,
    npc_start = 59,
    npc_complete = 60,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 110: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 59,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 110: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 110: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 59,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 110: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 110: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 110: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 60,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 110: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 110: Rewards given")
                self:give_rewards({
                    exp = 3733,
                    gold = 750,
                    items = {
                        { item_id = 50, count = 1 },
                        { item_id = 21000008, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_110