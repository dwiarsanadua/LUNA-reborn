--[[
  Quest 85: Evidence of Corruption
  Level Required: 12
  NPC Start: 13, NPC Complete: 52
  Prerequisites: [84]
  Rewards: EXP=964, Gold=750
]]

local fsm = require('fsm_engine')

local quest_85 = fsm:new({
    id = 85,
    name = "Evidence of Corruption",
    level_required = 12,
    npc_start = 13,
    npc_complete = 52,
    prerequisites = {84},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 85: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 52,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 85: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 85: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 52,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 85: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 85: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 85: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 52,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 85: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 85: Rewards given")
                self:give_rewards({
                    exp = 964,
                    gold = 750,
                    items = {
                        { item_id = 34, count = 1 },
                        { item_id = 34, count = 1 },
                        { item_id = 21000007, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_85