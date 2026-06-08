--[[
  Quest 242: [Repeatable] Stone Menace
  Level Required: 29
  NPC Start: 29, NPC Complete: 29
  Prerequisites: [241]
  Rewards: EXP=25495, Gold=4500
]]

local fsm = require('fsm_engine')

local quest_242 = fsm:new({
    id = 242,
    name = "[Repeatable] Stone Menace",
    level_required = 29,
    npc_start = 29,
    npc_complete = 29,
    prerequisites = {241},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 242: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 242: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 242: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 26,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 242: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 242: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 242: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 242: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 242: Rewards given")
                self:give_rewards({
                    exp = 25495,
                    gold = 4500,
                    items = {
                        { item_id = 21000562, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_242