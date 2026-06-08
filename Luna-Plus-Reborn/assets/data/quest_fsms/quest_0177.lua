--[[
  Quest 177: Lost Potions
  Level Required: 39
  NPC Start: 38, NPC Complete: 38
  Rewards: EXP=112861, Gold=5280
]]

local fsm = require('fsm_engine')

local quest_177 = fsm:new({
    id = 177,
    name = "Lost Potions",
    level_required = 39,
    npc_start = 38,
    npc_complete = 38,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 177: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 38,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 177: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 177: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 43,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 177: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 38,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 177: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 177: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 38,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 177: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 177: Rewards given")
                self:give_rewards({
                    exp = 112861,
                    gold = 5280,
                    items = {
                        { item_id = 91, count = 1 },
                        { item_id = 91, count = 3 },
                        { item_id = 21000005, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_177