--[[
  Quest 250: Mineral Specimens
  Level Required: 150
  NPC Start: 55, NPC Complete: 55
  Rewards: EXP=100982, Gold=4000
]]

local fsm = require('fsm_engine')

local quest_250 = fsm:new({
    id = 250,
    name = "Mineral Specimens",
    level_required = 150,
    npc_start = 55,
    npc_complete = 55,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 250: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 250: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 250: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 127,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 250: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 250: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 250: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 250: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 250: Rewards given")
                self:give_rewards({
                    exp = 100982,
                    gold = 4000,
                    items = {
                        { item_id = 11007296, count = 1 },
                        { item_id = 162, count = 20 },
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_250