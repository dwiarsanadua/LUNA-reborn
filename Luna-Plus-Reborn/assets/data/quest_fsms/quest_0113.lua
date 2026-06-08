--[[
  Quest 113: [Repeatable] Louis Lua's Request
  Level Required: 22
  NPC Start: 58, NPC Complete: 61
  Rewards: EXP=11879, Gold=2940
]]

local fsm = require('fsm_engine')

local quest_113 = fsm:new({
    id = 113,
    name = "[Repeatable] Louis Lua's Request",
    level_required = 22,
    npc_start = 58,
    npc_complete = 61,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 113: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 61,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 113: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 113: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 52,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 113: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 61,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 113: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 58,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 113: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 113: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 61,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 113: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 113: Rewards given")
                self:give_rewards({
                    exp = 11879,
                    gold = 2940,
                    items = {
                        { item_id = 52, count = 5 },
                        { item_id = 21000008, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_113