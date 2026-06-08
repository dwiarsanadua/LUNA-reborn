--[[
  Quest 157: Nutritious and Delicious
  Level Required: 28
  NPC Start: 28, NPC Complete: 29
  Prerequisites: [156]
  Rewards: EXP=13375, Gold=750
]]

local fsm = require('fsm_engine')

local quest_157 = fsm:new({
    id = 157,
    name = "Nutritious and Delicious",
    level_required = 28,
    npc_start = 28,
    npc_complete = 29,
    prerequisites = {156},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 157: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 157: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 157: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 157: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 157: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 157: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 157: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 157: Rewards given")
                self:give_rewards({
                    exp = 13375,
                    gold = 750,
                    items = {
                        { item_id = 71, count = 1 },
                        { item_id = 71, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_157