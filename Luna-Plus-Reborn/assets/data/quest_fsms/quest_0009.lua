--[[
  Quest 9: Safety Report
  Level Required: 7
  NPC Start: 6, NPC Complete: 13
  Prerequisites: [8]
  Rewards: EXP=189, Gold=795
]]

local fsm = require('fsm_engine')

local quest_9 = fsm:new({
    id = 9,
    name = "Safety Report",
    level_required = 7,
    npc_start = 6,
    npc_complete = 13,
    prerequisites = {8},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 9: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 9: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 9: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 9: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 9: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 9: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 9: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 9: Rewards given")
                self:give_rewards({
                    exp = 189,
                    gold = 795,
                    items = {
                        { item_id = 4, count = 1 },
                        { item_id = 21000006, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_9