--[[
  Quest 861: Lessons Fun
  Level Required: 89
  NPC Start: 124, NPC Complete: 124
  Rewards: EXP=4431094, Gold=57826
]]

local fsm = require('fsm_engine')

local quest_861 = fsm:new({
    id = 861,
    name = "Lessons Fun",
    level_required = 89,
    npc_start = 124,
    npc_complete = 124,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 861: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 124,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 861: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 861: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 709,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 861: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 124,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 861: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 861: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 124,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 861: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 861: Rewards given")
                self:give_rewards({
                    exp = 4431094,
                    gold = 57826,
                    items = {
                        { item_id = 21000513, count = 5 },
                        { item_id = 21000514, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_861