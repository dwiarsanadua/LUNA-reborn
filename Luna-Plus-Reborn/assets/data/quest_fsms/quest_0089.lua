--[[
  Quest 89: Recovery Work
  Level Required: 12
  NPC Start: 53, NPC Complete: 54
  Prerequisites: [88]
  Rewards: EXP=964, Gold=500
]]

local fsm = require('fsm_engine')

local quest_89 = fsm:new({
    id = 89,
    name = "Recovery Work",
    level_required = 12,
    npc_start = 53,
    npc_complete = 54,
    prerequisites = {88},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 89: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 53,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 89: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 89: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 53,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 89: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 54,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 89: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 89: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 54,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 89: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 89: Rewards given")
                self:give_rewards({
                    exp = 964,
                    gold = 500,
                    items = {
                        { item_id = 21000007, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_89