--[[
  Quest 227: [Party] Continuing Threat: Sharny
  Level Required: 30
  NPC Start: 64, NPC Complete: 65
  Prerequisites: [224]
  Rewards: EXP=37033, Gold=5605
]]

local fsm = require('fsm_engine')

local quest_227 = fsm:new({
    id = 227,
    name = "[Party] Continuing Threat: Sharny",
    level_required = 30,
    npc_start = 64,
    npc_complete = 65,
    prerequisites = {224},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 227: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 227: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 227: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 136,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 227: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 9,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 227: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 227: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 65,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 227: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 227: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 65,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 227: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 227: Rewards given")
                self:give_rewards({
                    exp = 37033,
                    gold = 5605,
                    items = {
                        { item_id = 147, count = 1 },
                        { item_id = 151, count = 1 },
                        { item_id = 21000513, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_227