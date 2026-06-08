--[[
  Quest 960: Bloody Oath 2
  Level Required: 110
  NPC Start: 544, NPC Complete: 544
  Rewards: EXP=24326130, Gold=106426
]]

local fsm = require('fsm_engine')

local quest_960 = fsm:new({
    id = 960,
    name = "Bloody Oath 2",
    level_required = 110,
    npc_start = 544,
    npc_complete = 544,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 960: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 544,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 960: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 960: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 345,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 960: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 544,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 960: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 960: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 544,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 960: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 960: Rewards given")
                self:give_rewards({
                    exp = 24326130,
                    gold = 106426,
                    items = {
                        { item_id = 415, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_960