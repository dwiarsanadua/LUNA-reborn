--[[
  Quest 87: Dwarf's Tools
  Level Required: 12
  NPC Start: 5, NPC Complete: 53
  Rewards: EXP=964, Gold=825
]]

local fsm = require('fsm_engine')

local quest_87 = fsm:new({
    id = 87,
    name = "Dwarf's Tools",
    level_required = 12,
    npc_start = 5,
    npc_complete = 53,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 87: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 53,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 87: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 87: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 53,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 87: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 87: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 87: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 53,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 87: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 87: Rewards given")
                self:give_rewards({
                    exp = 964,
                    gold = 825,
                    items = {
                        { item_id = 35, count = 1 },
                        { item_id = 35, count = 1 },
                        { item_id = 21000007, count = 9 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_87